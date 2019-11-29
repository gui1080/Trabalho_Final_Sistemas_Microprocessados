#include <msp430.h> 

/*  LIGAÇÕES
 *
 *  - LEDS RGB
 *
 *  5 volts -> 330ohms -> perna maior
 *  P6.2 -> 330ohms -> BLUE (direita de GREEN)
 *  P6.1 -> 330ohms -> GREEN (direita de 5V)
 *  P6.0 -> 330ohms -> RED (esquerda de 5V)
 *
 *  P1.4 -> 330ohms -> BLUE
 *  P5.3 -> 330ohms -> GREEN
 *  P5.1 -> 330ohms -> RED
 *
 *
 *  -Potenciometro (de frente)
 *
 *  esquerda -> GND
 *  meio -> P1.0
 *  direita -> 3.3v
 *
 *  -LDR (5mm)
 *
 *  GND -> perna direita
 *  3.3V -> 10k ohm -> perna esquerda -> output em P1.1
 *
 *  HC-SR04
 *
 *  VCC -> 5V
 *  GND -> GND
 *  TRIGGER -> P2.0
 *  ECHO -> P2.2
 *
 */


#include <stdio.h>
#include <string.h>
#include "../source/funcoes_auxiliares.c"

// LIMITES
#define LED_REFRESH_RATE 100
#define LIMITE_PARA_ESPACO 500
// o espaço de atuação do sensor realmente dá +/- 5 centímetros

// POTENCIOMETRO
uint16_t amostras_A0[8];
float volatile posicao_potenciometro;

// SENSOR DE LUZ
uint16_t amostras_A1[8];
float volatile media_A1;

// CONTADOR DE LEITURAS
int contador_de_leituras = 0;

// DISTANCIA DO SENSOR
int tempo;
int tempo_leitura_secundaria;
int tempo_teste;

// UPDATE DO LED
int conta_updates;

// FLAGS QUE DITAM SE A LUZ ESTÁ LIGADA OU NÃO
uint8_t acionamento_proximidade;
uint8_t luz_baixa;

void main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;


    // envia pro ultrassom
    P2DIR |= BIT0;
    P2OUT &= ~BIT0;
    // entrada ultrassom
    P2DIR &= ~(BIT2);           // entrada


    // LEDS

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


    while(1)
    {

        if(luz_baixa == 0){

            // lógica para pegar a distância no sensor 1 vez
            //------------------------------------------------------------------------------------------
            P2OUT |= BIT0;
            wait(100);
            P2OUT &= ~BIT0;

            while(!(P2IN & BIT2));      //espero meu pino conectado ao echo receber um sinal

            TB0CTL = (TBSSEL__SMCLK | MC__CONTINOUS | TBCLR);

            while(P2IN & BIT2);     // espero parar de receber um sinal

            TB0CTL = 0;

            tempo = TB0R;    // batidas de clock
            //------------------------------------------------------------------------------------------

            if(tempo < LIMITE_PARA_ESPACO){

                trava_milissegundos(1000);

                P6OUT ^= (BIT0);
                P6OUT ^= (BIT1);
                P6OUT ^= (BIT2);

                trava_milissegundos(1000);
                trava_milissegundos(1000);
                trava_milissegundos(1000);

                P6OUT ^= (BIT0);
                P6OUT ^= (BIT1);
                P6OUT ^= (BIT2);

                // lógica para pegar a distância no sensor 1 vez
                //------------------------------------------------------------------------------------------
                P2OUT |= BIT0;
                wait(100);
                P2OUT &= ~BIT0;

                while(!(P2IN & BIT2));      //espero meu pino conectado ao echo receber um sinal

                TB0CTL = (TBSSEL__SMCLK | MC__CONTINOUS | TBCLR);

                while(P2IN & BIT2);     // espero parar de receber um sinal

                TB0CTL = 0;

                tempo_leitura_secundaria = TB0R;    // batidas de clock

                //while(1);

                if(tempo_leitura_secundaria > LIMITE_PARA_ESPACO){

                    // queremos mudar o estado da luz
                    if(acionamento_proximidade == 1){
                        acionamento_proximidade = 0;
                    }
                    else{
                        acionamento_proximidade = 1;
                    }

                }
                else{

                    //falso positivo
                    tempo_teste = 0;
                    while(tempo_teste < LIMITE_PARA_ESPACO){

                        P6OUT ^= (BIT0);
                        P6OUT ^= (BIT1);
                        P6OUT ^= (BIT2);

                        // lógica para pegar a distância no sensor 1 vez
                        //------------------------------------------------------------------------------------------
                        P2OUT |= BIT0;
                        wait(100);
                        P2OUT &= ~BIT0;

                        while(!(P2IN & BIT2));      //espero meu pino conectado ao echo receber um sinal

                        TB0CTL = (TBSSEL__SMCLK | MC__CONTINOUS | TBCLR);

                        while(P2IN & BIT2);     // espero parar de receber um sinal

                        TB0CTL = 0;

                        tempo_teste = TB0R;    // batidas de clock
                        //------------------------------------------------------------------------------------------

                        P6OUT ^= (BIT0);
                        P6OUT ^= (BIT1);
                        P6OUT ^= (BIT2);

                        trava_milissegundos(500);


                    }

                    trava_milissegundos(1000);       // folga para o programa retornar para sua rotina normal
                }

                //------------------------------------------------------------------------------------------


            }


        }

        // update de LEDs
        //--------------------------------------------------------------------------------------------------------

        if(conta_updates == LED_REFRESH_RATE){      // nem todo ciclo do while na main atualiza o led. O delay é proposital.

            conta_updates = 0;

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

                }
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

        }

    // update de LEDs
    //--------------------------------------------------------------------------------------------------------

    // leitura do potenciometro e do ldr
    if(contador_de_leituras != 8){          // se nao chegamos, atualizamos a leitura ADC
        amostras_A0[contador_de_leituras] = adcRead(0);   // single channel, single convertion
        amostras_A1[contador_de_leituras] = adcRead(1);
        contador_de_leituras++;
    }

    if (contador_de_leituras == 8) {        // se chegamos ao final das leituras, resetamos
        contador_de_leituras = 0;
        posicao_potenciometro = valor_normalizado_vetor_potenciometro(amostras_A0);
        media_A1 = valor_normalizado_vetor_LDR(amostras_A1, &luz_baixa);
    }

    conta_updates++;

    wait(20000);

    }

}

