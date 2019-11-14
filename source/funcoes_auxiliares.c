#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

float valor_normalizado_vetor(uint16_t vector[8]) {

    float media = 0;
    int k;
    for (k = 0; k < 8; k++) {
        media += vector[k];
    }
    return ((media/8000)* 1.22);       // 0.92 vem de uma regra de 3, o LCD estava lendo valores acima de 3,3V, então assim diminuimos o valor
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

void convert_deci_hex(int n_decimal, char* resposta){

    static int quociente, restante;
    unsigned int i, j = 0;
    char dec_hex[4];            // tamanho do que queremos converter

    quociente = n_decimal;

    while (quociente != 0)          // fazemos para o numero inteiro
    {
        restante = quociente % 16;      // dividimos em potencias de 16
        if (restante < 10)
            dec_hex[j++] = 48 + restante;     // colocamos de acordo com a tablea ascii
        else
            dec_hex[j++] = 55 + restante;
        quociente = quociente / 16;
    }

    for(i = 0; i < j; i++) {
        resposta[i] = dec_hex[j - 1 - i];                           // colocamos a respostas no vetor
    }
    resposta[j] = '\0';

}
