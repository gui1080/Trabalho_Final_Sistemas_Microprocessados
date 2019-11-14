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
 * resistores de 1k ohm antes das entradas (devia ser 330ohms na moral...)
 *
 */


#include <stdio.h>
#include <string.h>

#include "../source/funcoes_auxiliares.c"

uint16_t amostras_A0[8];
float volatile media_A0;
float volatile posicao_potenciometro;

uint8_t ligado_potenciometro;

uint16_t adcFinished;
int contador_de_leituras = 0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P6DIR |= BIT6;              // habilita saida no P6.6 (LED VERDE)
    P6REN &= ~(BIT6);           // habilita resistor de pull up
    P6OUT &= ~(BIT6);           // zera saida

    TB0CTL = (TBSSEL__ACLK | MC__UP |  TBCLR);   // Clock

    TB0CCR0 = 0x0800;     // se 0x8000 conta 1 segundo, pegamos 1/16 disso para termos 16 amostras em 1 segundo
                            // precisamos disso para um precisão satisfatória
    TBCCTL0 = CCIE; // permite vermos a interrupção (local)

    __enable_interrupt();           // habilitamos a interrupcao do pragma vector



    P6DIR |= BIT0;              // habilita saida no P6.0 (LED VERMELHO)
    P6REN &= ~(BIT0);           // habilita resistor de pull up

    P6DIR |= BIT1;              // habilita saida no P6.1 (LED VERDE)
    P6REN &= ~(BIT1);           // habilita resistor de pull up

    P6DIR |= BIT2;              // habilita saida no P6.0 (LED AZUL)
    P6REN &= ~(BIT2);           // habilita resistor de pull up

    // escrever zero liga o led
    // setar desliga o led


    while(1)
    {

        // TESTE
        // -------------- botar numa função--------(param: media_A0 e estado da luz)----------------------------------------------------
        // a funcao vai receber o valor das amostras do potenciometro
        // se o potenciometro marca que ta desligado, não amostra o sensor de proximidade, e ignora o sensor de luz
        // executa só qnd um sensor fala que a luz esta ligada, se ta desligada executa:

        /* if(estado_da_luz == 0){
         *     apaga os pinos e dá break
         * }
         *
         *
         */

        if((media_A0 < 5) && (media_A0 > 4)){  // luz branca (LIGADO)
          P6OUT &= ~(BIT0);
          P6OUT &= ~(BIT1);
          P6OUT &= ~(BIT2);
          ligado_potenciometro = 1;
        }

        if((media_A0 < 4) && (media_A0 > 3)){  // red
          P6OUT &= ~(BIT0);
          P6OUT |= (BIT1);
          P6OUT |= (BIT2);
          ligado_potenciometro = 1;
        }

        if((media_A0 < 3) && (media_A0 > 2)){  // green
          P6OUT |= (BIT0);
          P6OUT &= ~(BIT1);
          P6OUT |= (BIT2);
          ligado_potenciometro = 1;
        }
        if((media_A0 < 2) && (media_A0 > 1)){  // blue
          P6OUT |= (BIT0);
          P6OUT |= (BIT1);
          P6OUT &= ~(BIT2);
          ligado_potenciometro = 1;
        }
        if((media_A0 < 1) && (media_A0 > 0)){   // apagado (DESLIGADO)
          P6OUT |= (BIT0);
          P6OUT |= (BIT1);
          P6OUT |= (BIT2);
          ligado_potenciometro = 0;
        }
        // -------------- botar numa função------------------------------------------------------------


    }

}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TB0_CCR0_ISR(){


    // leitura do potenciometro
    if(contador_de_leituras != 8){          // se nao chegamos, atualizamos a leitura ADC
        amostras_A0[contador_de_leituras] = adcRead(0);   // single channel, single convertion
        contador_de_leituras++;
    }

    if (contador_de_leituras == 8) {        // se chegamos ao final das leituras, resetamos
        contador_de_leituras = 0;
        adcFinished = 1;
        media_A0 = valor_normalizado_vetor(amostras_A0);
    }


    P6OUT ^= BIT6;                         // a luz pisca para termos ideia da frequencia da amostragem
                                                    // Cada vez que a luz pisca, cada canal fez 8 leituras

}
