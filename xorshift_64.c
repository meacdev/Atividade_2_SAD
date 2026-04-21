#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/*
    Tabela de shift triplets da página 347 de Numerical Recipes

    ID  a1  a2  a3
    A1  21  35  4
    A2  20  41  5
    A3  17  31  8
    A4  11  29  14
    A5  14  29  11  
    A6  30  35  13
    A7  21  37  4
    A8  21  43  4
    A9  23  41  18

    Abaixo estão os parâmetros escolhidos, podendo ser qualquer linha de valores da tabela

*/
#define A1 21
#define A2 35
#define A3 4

//========XORSHIFT========

static uint64_t state;

// Verifica se a seed é zero, retorna 1 caso for e a própria semente caso não for. 
// Já que o algoritmo fica em loop infinito tentando fazer shift em um número que é zero.

 uint64_t initialize(uint64_t seed){
    if (seed == 0){
        printf("Seed cannot be zero!");
        return 1;
    } else return seed;
}

// Função que faz sucessivos xor's e shifts, ou seja, o embaralhador em si 
uint64_t xorshift(){
    uint64_t temp = state;
    
    // Curiosidade: XOR (Ou exclusivo) em C é feito com o operador ^= e shift direto e esquerdo é feito com >> e << 

    temp ^= (temp >> A1); // XOR + Shift a direita
    temp ^= (temp << A2); // XOR + Shift a esquerda
    temp ^= (temp >> A3); // XOR + Shift a esquerda

    state = temp;

    return state;
}

//========QUI-QUADRADO========

// Utiliza a fórmula χ² = Σ (Oj - Ej)² / Ej de 1 até j
// Sendo Oj o número de amostras observadas dentro de cada intervalo j
// e Ej o número de medidas esperadas para o intervalo j

double chi_squared(int freq[], int n, int j){
    double Ej = (double) n/j;
    double chi2 = 0.0;

    for (int i=0; i < j; i++){
        double diff = freq[i] - Ej;
        chi2 += (diff*diff)/Ej;
    }

    return chi2;
}

//========TESTES DE TEMPO========

double rand_time(int n){
    srand((unsigned int) time(NULL));

    clock_t start = clock();
    for (int i=0; i < n; i++) rand() % 10;
    clock_t end = clock();

    return (double) (end-start) / CLOCKS_PER_SEC;
}

double xorshift_time(int n){
    state = (uint64_t) time(NULL) + 1;

    clock_t start = clock();
    for (int i = 0; i < n; i++) xorshift() % 10;
    clock_t end = clock();
    
    return (double)(end - start) / CLOCKS_PER_SEC;
}

//========TESTES COM QUI-QUADRADO========

// Teste com o algoritmo rand nativo de C
double rand_test(int n){
    int freq[10] = {0};

    srand((unsigned int) time(NULL));

    for (int i = 0; i < n; i++) freq[rand() %10]++;

    return chi_squared(freq, n, 10);
}

// Teste com o algoritmo xorshift
double xorshift_test(int n){
    int freq[10] = {0};
    
    state = (uint64_t) time(NULL) + 1;

    for (int i = 0; i < n; i++) freq[xorshift() % 10]++;
    
    return chi_squared(freq, n, 10);
}

int main(){

    int quantities[] = {100000, 1000000, 10000000, 100000000};
    int qtd_test = 4;

    FILE *results = fopen("test_values.csv", "w");
    if (results == NULL){
        printf("Error creating CSV file!");
        return 1;
    }

    fprintf(results, "TIME TESTS\n");
    fprintf(results, "Quantities, rand_time, xorshift_time, Winner\n");

    for(int i=0; i < 4; i++){
        int n = quantities[i];

        double sum_rand = 0;
        double sum_xorshift = 0;

        for (int j = 0; j < qtd_test; j++){
            sum_rand += rand_time(n);
            sum_xorshift += xorshift_time(n);
        }
        
        double avg_rand = sum_rand/qtd_test;
        double avg_xorshift = sum_xorshift/qtd_test;

        const char *winner = (avg_rand <= avg_xorshift) ? "rand" : "xorshift";

        fprintf(results, "%d, %.6f, %.6f, %.8s\n", n, avg_rand, avg_xorshift, winner);
    }

    fprintf(results, "CHI SQUARED TESTS\n");
    fprintf(results, "Quantities, chi2_rand, chi2_xorshift\n");

    for(int i = 0; i < 4; i++){
        int n = quantities[i];

        fprintf(results, "%d, %.6f, %.6f\n", n, rand_test(n), xorshift_test(n));
    }

    fclose(results);
}