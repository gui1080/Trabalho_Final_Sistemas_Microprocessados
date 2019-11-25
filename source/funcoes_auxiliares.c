#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define LIMITE 10

void wait(uint16_t input){

    volatile uint16_t dt;
    dt = input;

    TB1CTL = TBSSEL__SMCLK | MC__UP | TBCLR;
    TB1CCR0 = dt;

    while(!(TB1CCTL0 & CCIFG));
    TB1CCTL0 &= ~CCIFG;


}

inline void trava_milissegundos(long long int milissegundos){

    // conta at� 8 segundos
    // recebo um long int

    // 8192 = ACLK / 4
    // milisegundos = ((batidas de clock * 1000) / 8192)
    // batidas de clock = ((milissegundos * 8192) / 1000)
    // o nosso CCR0!

    volatile unsigned long int batidas_de_clock;
    batidas_de_clock = (milissegundos * 8.192);

    TB1CTL = (TBSSEL__ACLK | ID__4 | MC__CONTINOUS | TBCLR);
    TB1CCR0 = batidas_de_clock - 1;

    while(!(TB0CCTL0 & CCIFG));
    TB1CCTL0 &= ~CCIFG;

}



float valor_normalizado_vetor_potenciometro(uint16_t vector[8]) {

    float media = 0;
    int k;
    for (k = 0; k < 8; k++) {
        media += vector[k];
    }
    return ((media/8000)* 1.22);       // 0.92 vem de uma regra de 3, o LCD estava lendo valores acima de 3,3V, então assim diminuimos o valor
    // dividimos por 8 pois temos 8 amostras
    // dividimos por mil pois os valores saiam na casa do milhares

}

float valor_normalizado_vetor_LDR(uint16_t vector[8], uint8_t *luz_baixa) {

    float media = 0;
    int k;
    for (k = 0; k < 8; k++) {
        media += vector[k];
    }

    if(media > 7400){
      *luz_baixa = 1;
    }
    else{
      *luz_baixa = 0;
    }

    return ((media));       // 0.92 vem de uma regra de 3, o LCD estava lendo valores acima de 3,3V, então assim diminuimos o valor
    // dividimos por 8 pois temos 8 amostras
    // dividimos por mil pois os valores saiam na casa do milhares

}

// função de leitura ADC single channel, single convertion
uint16_t adcRead(uint8_t pin)
{
    ADCCTL0 = ADCSHT_6 | ADCON;                   // ligando ADC para podermos atualizar os proximos vetores
    ADCCTL1 = ADCSHS_0 | ADCCONSEQ_0 | ADCSHP;    // 1 canal, 1 conversão
    ADCCTL2 = ADCRES_2;                           // resolução do sinal de saída vai ser 12 bits

    ADCMCTL0 = pin;                      // Seleciona o canal

    P1SEL0 |= 1 << pin;                   // seleciona o pino lido
    P1SEL1 |= 1 << pin;


    ADCCTL0 |= ADCENC;                          // Habilita
    ADCCTL0 &= ~ADCSC;                          // Gera flanco de subida
    ADCCTL0 |= ADCSC;

    while(!(ADCIFG & ADCIFG0));                 // esperamos o final da conversão
    return ADCMEM0;                             // retornamos o espaço de memória com o resultado
}

uint8_t verifica_falso_positivo(){

    volatile uint8_t teste;

    P6OUT |= BIT6;

    trava_milissegundos(1500);

    P6OUT &= ~BIT6;

    trava_milissegundos(500);

    teste = verifica_dist();

    if(teste > LIMITE){

        return 0;

    }

    else{

        return 1;

    }

}
