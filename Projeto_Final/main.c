#include <msp430.h> 


/*
 * Ligações com o sensor RGB
 *
 * Sentido: MSP430 FR2355 -> RGB
 *
 * 5 volts -> 330ohms -> perna maior
 * P6.2 -> 330ohms -> BLUE (direita de GREEN)
 * P6.1 -> 330ohms -> GREEN (direita de 5V)
 * P6.0 -> 330ohms -> RED (esquerda de 5V)
 *
 * P1.4 -> 330ohms -> BLUE
 * P5.3 -> 330ohms -> GREEN
 * P5.1 -> 330ohms -> RED
 *
 *
 * -Potenciometro (de frente)
 *
 * esquerda -> GND
 * meio -> P1.0
 * direita -> 3.3v
 *
 * -LDR (5mm)
 *
 * GND -> perna direita
 * 5V -> 10k ohm -> perna esquerda -> output em P1.1
 *
 * HC-SR04
 *
 */


#include <stdio.h>
#include <string.h>

#include "../source/funcoes_auxiliares.c"

uint16_t amostras_A0[8];
float volatile posicao_potenciometro;

uint16_t amostras_A1[8];
float volatile media_A1;
uint8_t luz_baixa;

uint16_t adcFinished;
int contador_de_leituras = 0;

uint8_t tempo;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;


    // envia pro ultrassom no P6.0
    P2DIR |= BIT0;
    P2OUT |= BIT0;

    // entrada ultrassom
    P2DIR &= ~(BIT2);           // entrada
    P2REN |= BIT2;              // habilitamos sua leitura
    P2OUT |= BIT2;              // inicializamos com zero
    P2SEL1 |= (BIT2);



    P6DIR |= BIT6;              // habilita saida no P6.6 (LED VERDE)
    P6REN &= ~(BIT6);           // habilita resistor de pull up
    P6OUT &= ~(BIT6);           // zera saida

    TB0CTL = (TBSSEL__ACLK | MC__UP |  TBCLR);   // Clock

    TB0CCR0 = 0x0800;     // se 0x8000 conta 1 segundo, pegamos 1/16 disso para termos 16 amostras em 1 segundo
                            // precisamos disso para um precisão satisfatória
    TBCCTL0 = CCIE; // permite vermos a interrupção (local)

    __enable_interrupt();           // habilitamos a interrupcao do pragma vector


    P6DIR |= BIT0;              // habilita saida no P6.0 (LED VERMELHO LED 1)
    P6REN &= ~(BIT0);           // habilita resistor de pull up

    P6DIR |= BIT1;              // habilita saida no P6.1 (LED VERDE LED 1)
    P6REN &= ~(BIT1);           // habilita resistor de pull up

    P6DIR |= BIT2;              // habilita saida no P6.0 (LED AZUL LED 1)
    P6REN &= ~(BIT2);           // habilita resistor de pull up

    P5DIR |= BIT1;              // habilita saida no P (LED VERMELHO LED 2)
    P5REN &= ~(BIT1);           // habilita resistor de pull up

    P5DIR |= BIT3;              // habilita saida no P (LED VERDE LED 2)
    P5REN &= ~(BIT3);           // habilita resistor de pull up

    P1DIR |= BIT4;              // habilita saida no P (LED AZUL LED 2)
    P1REN &= ~(BIT4);           // habilita resistor de pull up
    // escrever zero liga o led
    // setar desliga o led

    // ideia: potencialmente usar 2 clocks, fazendo o do sensor de proximidade ser executado a cada 1 segundo e os outros sendo executados muito mais rapidamente
    // um clock maior para o potenciometro deixa um baita delay chato nas mudanças de cor

    while(1)
    {

        __disable_interrupt();

        P2OUT |= BIT0;
        wait(20);
        P2OUT &= ~BIT0;

        while(!(P2IN & BIT2));      //espero meu pino conectado ao echo receber um sinal

        TB1CTL = (TBSSEL__SMCLK | MC__CONTINOUS | TBCLR);

        while(P2IN & BIT2);     // espero parar de receber um sinal

        tempo = TB1R;    // batidas de clock


        while(1);   // no lugar disso aqui eu ligaria os leds de acordo e tal


        /*


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
           /////////

        if(luz_baixa == 1){

            if((posicao_potenciometro < 5) && (posicao_potenciometro > 4)){  // luz branca (LIGADO)

              // led 1
              P6OUT &= ~(BIT0);
              P6OUT &= ~(BIT1);
              P6OUT &= ~(BIT2);

              // led 2
              P5OUT &= ~(BIT1);
              P5OUT &= ~(BIT3);
              P1OUT &= ~(BIT4);

            }

            if((posicao_potenciometro < 4) && (posicao_potenciometro > 3)){  // red

              // led 1
              P6OUT &= ~(BIT0);
              P6OUT |= (BIT1);
              P6OUT |= (BIT2);

              // led 2
              P5OUT &= ~(BIT1);
              P5OUT |= (BIT3);
              P1OUT |= (BIT4);

            }

            if((posicao_potenciometro < 3) && (posicao_potenciometro > 2)){  // green

              // led 1
              P6OUT |= (BIT0);
              P6OUT &= ~(BIT1);
              P6OUT |= (BIT2);

              // led 2
              P5OUT |= (BIT1);
              P5OUT &= ~(BIT3);
              P1OUT |= (BIT4);

            }
            if((posicao_potenciometro < 2) && (posicao_potenciometro > 1)){  // blue

              // led 1
              P6OUT |= (BIT0);
              P6OUT |= (BIT1);
              P6OUT &= ~(BIT2);

              // led 2
              P5OUT |= (BIT1);
              P5OUT |= (BIT3);
              P1OUT &= ~(BIT4);

            }
            if((posicao_potenciometro < 1) && (posicao_potenciometro > 0)){   // apagado (DESLIGADO)

              // led 1
              P6OUT |= (BIT0);
              P6OUT |= (BIT1);
              P6OUT |= (BIT2);

              // led 2
              P5OUT |= (BIT1);
              P5OUT |= (BIT3);
              P1OUT |= (BIT4);

              while((posicao_potenciometro < 1) && (posicao_potenciometro > 0));
                // ideia: para travar o programa, aqui entraria um while eterno que só acaba quando a media_A0 muda
            }
        // -------------- botar numa função------------------------------------------------------------
        }

        else{
            // led 1
            P6OUT |= (BIT0);
            P6OUT |= (BIT1);
            P6OUT |= (BIT2);

            // led 2
            P5OUT |= (BIT1);
            P5OUT |= (BIT3);
            P1OUT |= (BIT4);
        }

        */
    }

}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TB0_CCR0_ISR(){


    // leitura do potenciometro
    if(contador_de_leituras != 8){          // se nao chegamos, atualizamos a leitura ADC
        amostras_A0[contador_de_leituras] = adcRead(0);   // single channel, single convertion
        amostras_A1[contador_de_leituras] = adcRead(1);
        contador_de_leituras++;
    }

    if (contador_de_leituras == 8) {        // se chegamos ao final das leituras, resetamos
        contador_de_leituras = 0;
        adcFinished = 1;
        posicao_potenciometro = valor_normalizado_vetor_potenciometro(amostras_A0);
        media_A1 = valor_normalizado_vetor_LDR(amostras_A1, &luz_baixa);
    }


    P6OUT ^= BIT6;                         // a luz pisca para termos ideia da frequencia da amostragem
                                                    // Cada vez que a luz pisca, cada canal fez 8 leituras

}
