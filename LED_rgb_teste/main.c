#include <msp430.h> 


/*
 * Ligações com o sensor RGB
 *
 * Sentido: MSP430 FR2355 -> RGB
 *
 * 5 volts -> perna maior
 * P6.2 -> BLUE (direita de GREEN)
 * P6.1 -> GREEN (direita de 5V)
 * P6.0 -> RED (esquerda de 5V)
 *
 * resistores de 1k ohm antes das entradas
 *
 */


/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // destrava os pinos digitais

    P6DIR |= BIT0;              // habilita saida no P6.0 (LED VERMELHO)
    P6REN &= ~(BIT0);           // habilita resistor de pull up
    P6OUT &= ~(BIT0);           // zera saida
	
    P6DIR |= BIT1;              // habilita saida no P6.1 (LED VERDE)
    P6REN &= ~(BIT1);           // habilita resistor de pull up
    P6OUT &= ~(BIT1);           // zera saida

    P6DIR |= BIT2;              // habilita saida no P6.0 (LED AZUL)
    P6REN &= ~(BIT2);           // habilita resistor de pull up
    P6OUT &= ~(BIT2);           // zera saida

    // aparentemente escrever zero liga o led


    while(1);

}
