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

uint8_t falso_positivo;

uint8_t acionamento_proximidade;

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;


    // envia pro ultrassom no P2.0
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


        trava_milissegundos(100); 

        tempo = verifica_dist();

        trava_milissegundos(100);        

        /*

        if(tempo > LIMITE){

            falso_positivo = verifica_falso_positivo();

            if(falso_positivo){
                acionamento_proximidade = 0;
            }

            else{

                if(acionamento_proximidade == 0){

                    acionamento_proximidade = 1;

                }
                if(acionamento_proximidade == 1){

                    acionamento_proximidade = 0;

                }

            }



        }


        */

        if((luz_baixa == 1) || (acionamento_proximidade == 1)){

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

    P6OUT ^= BIT6;  

    }

}

