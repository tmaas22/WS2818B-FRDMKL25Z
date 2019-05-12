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



uint32_t calculateWhiteBits(void){
	uint8_t bits = (WHITE_INTENSITY_PERCENT*255)/100;
	
	return (bits << 8*2) | (bits << 8*1) | (bits << 8*0);
}



uint32_t calculateRedBits(void){
	uint8_t bits = (RED_INTENSITY_PERCENT*255)/100;
	
	return (0x00 << 8*2) | (bits << 8*1) | (0x00 << 8*0);
}


void setPatterns(void){
	int j = 0;
	int w = 0;
	int temp = 0;
	
	uint32_t whiteBitsCopy = calculateWhiteBits();
	uint32_t redBitsCopy = calculateRedBits();
	uint32_t whiteBits, redBits;
	

	
	BYTE q;
	
	// Iterate through the entire LED_Count, adding 3 pixels (Pattern_size), each iteration.
	for(; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		
		whiteBits = whiteBitsCopy;
		// Define white Pixel
		for(j = 0; j < BITS_PER_LED; j++){
			if(whiteBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			whiteBits >>= 1;
			
			my_array[j+temp] = q;
			
		}
		
		
		// Define Red Pixel
		redBits = redBitsCopy;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(redBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			redBits >>= 1;
			
			my_array[j+24+temp] = q;
		}
		
		// Define our Black (Off Pixel)
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			my_array[j+48+temp] = q;
		}
	}
	

	// Pattern 2
	for(w=0; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		
		// Define Black (Off) Pixel
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			pattern_2[j+temp] = q;
		}
		
		
		// Define White Pixel
		whiteBits = whiteBitsCopy;
		for(j = 0; j < BITS_PER_LED; j++){	
			if(whiteBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			whiteBits >>= 1;
			
			pattern_2[j+24+temp] = q;
		}
		
		
		// Define Red Pixel
		redBits = redBitsCopy;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(redBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			redBits >>= 1;
			pattern_2[j+48+temp] = q;
		}
	}
	
	// Pattern 3
	for(w=0; w < LED_COUNT/PATTERN_SIZE; w++){
		temp = w*BITS_PER_LED*PATTERN_SIZE;
		
		// Define Red Pixel
		redBits = redBitsCopy;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(redBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			redBits >>= 1;
			pattern_3[j+temp] = q;
		}
		
		
		// Define Black (Off) Pixel
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			pattern_3[j+24+temp] = q;
		}
		
		// Define White Pixel
		whiteBits = whiteBitsCopy;
		q = 0xC0;
		for(j = 0; j < BITS_PER_LED; j++){
			if(whiteBits & 0x01){
				q = 0xF8;
			} else {
				q = 0xC0;
			}
			whiteBits >>= 1;
			
			pattern_3[j+48+temp] = q;
		}
	}
	
}

int main(void) {
	Init_Debug_Signals();
	Init_RGB_LEDs();
	Init_TPM(PATTERN_FREQ_MS);
	Control_RGB_LEDs(1,1,0);	// Yellow - starting up


	setPatterns();
	
	i = ARRAY_SIZE - 1;
	
	#ifdef USE_POLL
	#if USE_POLL
		int j = 0;
	#endif
	#endif
	
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
