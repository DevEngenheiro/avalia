#ifndef EXPRESSAO_H
#define EXPRESSAO_H

typedef struct {
    char posFixa[512];     // Expressão na forma pós-fixa, como "3 12 4 + *"
    char inFixa[512];      // Expressão na forma infixa, como "3 * (12 + 4)"
    float Valor;           // Valor numérico da expressão   
} Expressao;

char *getFormaInFixa(char *Str);     // Retorna string alocada na heap
char *getFormaPosFixa(char *Str);    // Retorna string alocada na heap
float getValorPosFixa(char *StrPosFixa);
float getValorInFixa(char *StrInFixa);

#endif
