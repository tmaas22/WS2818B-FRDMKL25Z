#include "integer.h"
#include <MKL25Z4.h>
#include "spi_io.h"
#include "LEDs.h"
#include "debug.h"


volatile BYTE * whichArrayPtr = &my_array[0];

volatile BYTE * patterns[] = {&my_array[0], &pattern_2[0], &pattern_3[0]};
volatile uint8_t patternIdx = 1;

void Init_TPM(uint32_t period_ms)
{
	//turn on clock to TPM 
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//set clock source for tpm to be TPM Source 3 which is MCGIR (32 kHz)
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3);

	// disable TPM
	TPM0->SC = 0;

	//set TPM to count up and divide by 32 with prescaler and use clock mode
	// will count at 1 kHz
	TPM0->SC = (TPM_SC_PS(5));
	
	//load the counter and mod. Note: 16-bit counter, not 32-bit
	TPM0->MOD = TPM_MOD_MOD(period_ms);
	TPM0->SC |= TPM_SC_TOIE_MASK;

	// Configure NVIC 
	NVIC_SetPriority(TPM0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);	
}

void TPM0_IRQHandler() {
	NVIC_DisableIRQ(SPI1_IRQn);
	//clear pending IRQ
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	TPM0->SC |= TPM_SC_TOF_MASK; 

	switch(++patternIdx){
		case 3:
			patternIdx = 0;
		case 0:
			whichArrayPtr = &my_array[0];
			break;
		case 2:
			whichArrayPtr = &pattern_2[0];
			break;
		case 1:
			whichArrayPtr = &pattern_3[0];
			break;
		default:
			whichArrayPtr = &my_array[0];
			
	}
	
	i = ARRAY_SIZE - 1;
	NVIC_EnableIRQ(SPI1_IRQn);
	// Do ISR work
}

void Start_TPM(void) {
// Enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
}


#define PATTERN_FREQ_MS	100
#define INTENSITY_IDX_OFFSET 4
#define RED_INTENSITY_OFFSET 5

void setPatterns(void){
		int j = 0;
	int w = 0;
	int temp = 0;
	BYTE q;
	for(; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			// Set the last bit of each RGB color to 1 (SPI
			if((j+INTENSITY_IDX_OFFSET)%8 == 0){
				my_array[j+temp] = 0xF8;
				continue;
			}
			my_array[j+temp] = q;
		}
		q = 0xC0;
			for(j = 0; j < BITS_PER_LED; j++){
				if(j==8){
					q = 0xF8;
				}
				if(j==(8+RED_INTENSITY_OFFSET)){
					q=0xC0;
				}
			my_array[j+24+temp] = q;
		}
			q = 0xC0;
			for(j = 0; j < BITS_PER_LED; j++){

			my_array[j+48+temp] = q;
		}
	}
	
	for(w=0; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){

			pattern_2[j+temp] = q;
		}
		
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){			
			// Set the last bit of each RGB color to 1 (SPI
			if((j+INTENSITY_IDX_OFFSET)%8 == 0){
				pattern_2[j+24+temp] = 0xF8;
				continue;
			}
			pattern_2[j+24+temp] = q;
		}
		
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(j==8){
				q = 0xF8;
			}
			if(j==(8+RED_INTENSITY_OFFSET)){
				q=0xC0;
			}
			pattern_2[j+48+temp] = q;
		}
	}
		
	for(w=0; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(j==8){
				q = 0xF8;
			}
			if(j==(8+RED_INTENSITY_OFFSET)){
				q=0xC0;
			}
			pattern_3[j+temp] = q;
		}
		
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){

			pattern_3[j+24+temp] = q;
		}
		
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){			
			// Set the last bit of each RGB color to 1 (SPI
			if((j+INTENSITY_IDX_OFFSET)%8 == 0){
				pattern_3[j+48+temp] = 0xF8;
				continue;
			}
			pattern_3[j+48+temp] = q;
		}
	}
	
}

int main(void) {
	Init_Debug_Signals();
	Init_RGB_LEDs();
	Init_TPM(PATTERN_FREQ_MS);
	Control_RGB_LEDs(1,1,0);	// Yellow - starting up

	int j = 0;
	setPatterns();
	
	i = ARRAY_SIZE - 1;
	j = 0;
	
	Control_RGB_LEDs(0,0,0);
	Start_TPM();
	SPI_Init();
	while (1) {
		#ifdef USE_POLL
			#if USE_POLL
					if(j < 56){
						SPI_Write(my_array[i--]);
					if(i < 0) { 
					i = 71; 
						j++;
					}}
			#endif
		#endif
//		Thread_Test_SD();
		
	}
}
