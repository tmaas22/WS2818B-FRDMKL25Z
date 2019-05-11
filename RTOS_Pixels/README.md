# "RTOS"_Pixels
This is a small library to control WS2818B LED RGB Pixel Tape using an NXP-FRDM KL25Z board.


## How Is it Done
This library currently runs a simple sequence of a Red, White, and Black (Off) pattern (GO State!).
There are 3 patterns defined which creates an implied counterclockwise chase. The code cycles between
the 3 patterns based on a timeout set by TPM0.

WS2818B uses a really stupid communication protocol. In order to work, the KL25Z development board is set
to a **32 MHz** clock frequency and uses a gross SPI interface over SPI1 clocked at **8 MHz** to send the 
proper 1/0 signals. 

Due to strict timing sequences, minimal computing can be done within the SPI interrupt. (Trust me, I tried
to place a simple if/else to loop back around on an array, and it caused LEDs expected to be off to set the
highest bit for the Green). Therefore, all of the patterns are calculated prior to starting the SPI interface
and the patterns define the entire array of LEDs and every bit.


Currently, the SPI IRQ iterates through the patterns backwards, starting with index _1223_ and working to _0_.
When setting the patterns, this means that the **8th** bit set for specific LED in each color is the MSB.
If you want ~50% intensity, you can have bits 1-7 be set to 0 and bit 8 set to 1.


## Code Structure
There is none. It's very hacky to get something to work.

### Program Flow
A brief look and explanation of how the code works.

1. The Code first initializes Debug Signals the On-Board RGB and Timer.
2. The 3 patterns are intialized with the values needed. These patterns are arrays of **24** (bits needed per LED, 8 per color) times **51** (the # of LEDs)
3. Start the Timer to interrupt every X ms.
4. Start the SPI device
5. `while(1) // main OS`
6. SPI Interrupts, disables TPM interrupt, and sends all **24x51** bits of data to control the LEDs. Enabling TPM interrupt on exit
7. TPM0 Interrupts every _X_ milliseconds to switch to the next pattern.