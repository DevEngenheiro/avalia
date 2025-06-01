#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

int main(void) {
    // Exemplos de teste: cada linha mostra a expressão INFIXA, a correspondente PÓS-FIXA e o valor esperado.
    char inf1[] = "3 * ( 12 + 4 )";
    char *pos1 = getFormaPosFixa(inf1);
    float v1  = getValorInFixa(inf1);
    printf("INFIXA: %s\n", inf1);
    printf(" → PÓS-FIXA: %s\n", pos1);
    printf(" → VALOR: %.2f (esperado 35.00)\n\n", v1);
    free(pos1);

    char pos2[] = "7 2 * 4 + 7 * 2 + 4";
    char *in2 = getFormaInFixa(pos2);
    float v2   = getValorPosFixa(pos2);
    printf("PÓS-FIXA: %s\n", pos2);
    printf(" → INFIXA: %s\n", in2);
    printf(" → VALOR: %.2f (esperado 18.00)\n\n", v2);
    free(in2);

    // Teste 6: “2 3 + log 5 /”   => infixa: “( log ( ( 2 + 3 ) ) / 5 )” → valor ≈ 0.14 
    char pos3[] = "2 3 + log 5 /";
    char *in3 = getFormaInFixa(pos3);
    float v3 = getValorPosFixa(pos3);
    printf("PÓS-FIXA: %s\n", pos3);
    printf(" → INFIXA: %s\n", in3);
    printf(" → VALOR: %.4f (esperado ≈ 0.14)\n\n", v3);
    free(in3);

    // Teste 8: “45 60 + 30 cos *” → INFIXA: “( ( 45 + 60 ) * cos( 30 ) )” → valor ≈ 90.93
    char pos4[] = "45 60 + 30 cos *";
    char *in4 = getFormaInFixa(pos4);
    float v4 = getValorPosFixa(pos4);
    printf("PÓS-FIXA: %s\n", pos4);
    printf(" → INFIXA: %s\n", in4);
    printf(" → VALOR: %.2f (esperado ≈ 90.93)\n\n", v4);
    free(in4);

    // Teste 9: “0.5 45 sen 2 ^ +”
    char pos5[] = "0.5 45 sen 2 ^ +";
    char *in5 = getFormaInFixa(pos5);
    float v5 = getValorPosFixa(pos5);
    printf("PÓS-FIXA: %s\n", pos5);
    printf(" → INFIXA: %s\n", in5);
    printf(" → VALOR: %.2f (esperado ≈ 1.00)\n\n", v5);
    free(in5);

    return 0;
}