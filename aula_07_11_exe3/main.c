#include <msp430.h> 
#include <stdint.h>

// (canais A0 e A1)
// P1.0
// P1.1 (lemos as voltagens dessas entradas)

// -> SDA
// -> SDL
// GND -> GND
// 5V -> VCC

// falta o LCD

void adcConfig();

/**
 * main.c
 */

uint16_t *dataPtr;
uint16_t adcFinished;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;


    P1DIR |= BIT0;              // habilita saida no P1.0 (LED VERMELHO)
    P1REN &= ~(BIT0);           // habilita resistor de pull up
    P1OUT &= ~(BIT0);           // zera saida

	uint16_t inputData [16];
	adcFinished = 1;

	uint16_t media_0;
	uint16_t media_1;
	int i;

	dataPtr = &inputData[0];

	adcConfig();
	__enable_interrupt();

	while(1){

	    adcFinished = 0;
	    while(!adcFinished);

	    //media

	    for(i = 0 ; i<15; i++){
	        if((i % 2) == 0){
	            media_1 = media_1 + inputData[i];
	        }
	        else{
	            media_0 = media_0 + inputData[i];
	        }
	    }

	    media_0 = media_0 / 8;
	    media_1 = media_1 / 8;      // é isso que queremos imprimir

	    if(adcFinished == 1){
	        //escreve no LCD

	        P1OUT = ^BIT0;
	        adcFinished = 0;
	    }

	}
	
}

void adcConfig(){

    ADCCTL0 = ADCON | ADCMSC;
    ADCCTL1 = ADCSHS_2 | ADCCONSEQ_1 | ADCSHP;
    ADCCTL2 = ADCRES_1;

    ADCMCTL0 = 1;

    P1SEL0 |= BIT6;
    P1SEL1 |= BIT6;

    ADCCTL0 = ADCENC;
    TB1CTL = MC__UP | TBCLR | TBSSEL__ACLK;
    TB1CCR0 = 0x1000 - 1;        // 1 segundo pra converter tudo e 1/8 de 0x8000 pra 1 conversão
    TB1CCR1 = 0x0800 - 1;         // 50% duty cycle

    TB1CCTL1 = OUTMOD_7;

}

#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR()
{
    static uint8_t dataCnt;
    dataCnt = 0;

    *(dataPtr + dataCnt) = ADCMEM0;

    if(dataCnt++ == 15){
        adcFinished = 1;
        dataCnt = 0;
    }

}
