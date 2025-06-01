#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <math.h>
#include "expressao.h"

/*
 * AUXILIARES PARA PILHAS
 */

/* Pilha de strings (usada em pósFixa→inFixa) */
typedef struct StrNode {
    char *str;
    struct StrNode *next;
} StrNode;

static void pushStr(StrNode **topo, const char *s) {
    StrNode *novo = malloc(sizeof(StrNode));
    if (!novo) return;
    novo->str = malloc(strlen(s) + 1);
    strcpy(novo->str, s);
    novo->next = *topo;
    *topo = novo;
}

static char *popStr(StrNode **topo) {
    if (!*topo) return NULL;
    StrNode *rem = *topo;
    char *r = rem->str;
    *topo = rem->next;
    free(rem);
    return r;
}

static int isStrStackEmpty(StrNode *topo) {
    return (topo == NULL);
}

/* Pilha de floats (usada em avaliação) */
typedef struct FloatNode {
    float val;
    struct FloatNode *next;
} FloatNode;

static void pushFloat(FloatNode **topo, float v) {
    FloatNode *novo = malloc(sizeof(FloatNode));
    if (!novo) return;
    novo->val = v;
    novo->next = *topo;
    *topo = novo;
}

static float popFloat(FloatNode **topo) {
    if (!*topo) return 0.0f; // sobras invalidadas
    FloatNode *rem = *topo;
    float r = rem->val;
    *topo = rem->next;
    free(rem);
    return r;
}

static int isFloatStackEmpty(FloatNode *topo) {
    return (topo == NULL);
}

/*
 * AUXILIAR: verifica se string é número decimal (inteiro ou ponto flutuante).
 * Retorna 1 se for número, 0 caso contrário.
 */
static int isNumero(const char *token) {
    if (!token) return 0;
    char *endp;
    strtod(token, &endp);
    return (*endp == '\0');
}

/*
 * AUXILIAR: prioridade de um operador ou função. 
 * Maior número = maior prioridade.
 * Retornamos 5 para funções unárias “raiz”, “sen”, “cos”, “tg”, “log”. 
 * 4 para '^', 3 para '*', '/', '%', 2 para '+', '-', 
 * 1 para '(' (sempre menor), 0 para qualquer outro.
 */
static int prioridade(const char *op) {
    if (!strcmp(op, "raiz") || !strcmp(op, "sen") ||
        !strcmp(op, "cos")  || !strcmp(op, "tg")  ||
        !strcmp(op, "log"))
    {
        return 5;
    }
    if (!strcmp(op, "^")) return 4;
    if (!strcmp(op, "*") || !strcmp(op, "/") || !strcmp(op, "%")) return 3;
    if (!strcmp(op, "+") || !strcmp(op, "-")) return 2;
    if (!strcmp(op, "(")) return 1;
    return 0;
}

/*
 * AUXILIAR: determina se op1 é associativo à esquerda (retorna 1) 
 * ou se é associativo à direita (retorna 0). Apenas '^' é associativo à direita.
 * Funções unárias consideramos serem associativas “à direita” (por convenção),
 * mas isso pouco importa – elas sempre "saltam" da pilha quando encontramos token ou ')'.
 */
static int isAssociativoEsq(const char *op) {
    if (!strcmp(op, "^")) return 0;  // associativo à direita
    if (!strcmp(op, "raiz") || !strcmp(op, "sen") ||
        !strcmp(op, "cos")  || !strcmp(op, "tg")  ||
        !strcmp(op, "log"))
    {
        // funções unárias → queremos que sejam tratadas como uma unidade inteira
        return 0;  
    }
    // '+', '-', '*', '/', '%'
    return 1;
}

/*
 * getFormaPosFixa:
 * Converte expressão infixa (tokens separados por espaço) em pós-fixa. 
 * Retorna string alocada no heap. Caller must free().
 */
char *getFormaPosFixa(char *Str) {
    // Buffer de saída (pode ter até 512 chars) 
    char *saida = malloc(512);
    if (!saida) return NULL;
    saida[0] = '\0';

    // Pilha de operadores (cadeia de strings)
    StrNode *opTopo = NULL;

    // Fazemos a tokenização por espaço
    char buffer[512];
    strcpy(buffer, Str);
    char *tok = strtok(buffer, " ");
    while (tok) {
        if (isNumero(tok)) {
            // número → envia diretamente para saída
            strcat(saida, tok);
            strcat(saida, " ");
        }
        else if (!strcmp(tok, "(")) {
            // '(' → empilha
            pushStr(&opTopo, tok);
        }
        else if (!strcmp(tok, ")")) {
            // ')' → desempilha até encontrar '('
            while (!isStrStackEmpty(opTopo) && strcmp(opTopo->str, "(") != 0) {
                char *q = popStr(&opTopo);
                strcat(saida, q);
                strcat(saida, " ");
                free(q);
            }
            // Descarta '('
            if (!isStrStackEmpty(opTopo)) {
                char *q = popStr(&opTopo);
                free(q);
            }
            // Se houver função unária logo acima de '(', desempilha e manda para saída
            if (!isStrStackEmpty(opTopo)) {
                int pr = prioridade(opTopo->str);
                if (pr == 5) {
                    char *f = popStr(&opTopo);
                    strcat(saida, f);
                    strcat(saida, " ");
                    free(f);
                }
            }
        }
        else {
            // token é operador ou função unária
            int prTok = prioridade(tok);
            // Enquanto o topo da pilha tenha operador de prioridade maior (ou igual, se à esquerda)...
            while (!isStrStackEmpty(opTopo)) {
                char *topOp = opTopo->str;
                int prTop = prioridade(topOp);
                if (prTop > prTok || (prTop == prTok && isAssociativoEsq(topOp))) {
                    // desempilha e manda para saída
                    char *q = popStr(&opTopo);
                    strcat(saida, q);
                    strcat(saida, " ");
                    free(q);
                }
                else break;
            }
            // Empilha o token
            pushStr(&opTopo, tok);
        }
        tok = strtok(NULL, " ");
    }

    // esvazia pilha de operadores -> saída
    while (!isStrStackEmpty(opTopo)) {
        char *q = popStr(&opTopo);
        strcat(saida, q);
        strcat(saida, " ");
        free(q);
    }

    return saida;
}

/*
 * getFormaInFixa:
 * Converte expressão pós-fixa (tokens separados por espaço) em infixa.
 * Retorna string alocada na heap. Caller must free().
 */
char *getFormaInFixa(char *Str) {
    StrNode *pilha = NULL;
    char buffer[512];
    strcpy(buffer, Str);
    char *tok = strtok(buffer, " ");
    while (tok) {
        if (isNumero(tok)) {
            // empilha o próprio número como string
            pushStr(&pilha, tok);
        }
        else {
            // pode ser operador binário ou função unária
            int pr = prioridade(tok);
            if (pr == 5) {
                // função unária → ocupa um operando
                char *a = popStr(&pilha);
                // monta a string “tok(a)”
                char temp[512];
                snprintf(temp, sizeof(temp), "%s(%s)", tok, a);
                free(a);
                pushStr(&pilha, temp);
            }
            else {
                // operador binário
                // desempilha dois operandos
                char *b = popStr(&pilha);
                char *a = popStr(&pilha);
                char temp[512];
                // monta a string “( a <op> b )”
                snprintf(temp, sizeof(temp), "( %s %s %s )", a, tok, b);
                free(a);
                free(b);
                pushStr(&pilha, temp);
            }
        }
        tok = strtok(NULL, " ");
    }
    // no final, sobra exatamente 1 string na pilha
    char *res = popStr(&pilha);
    return res;  // Caller faz free(res);
}

/*
 * getValorPosFixa:
 * Avalia expressão em pós-fixa (tokens separados por espaço) e retorna o valor float.
 */
float getValorPosFixa(char *StrPosFixa) {
    FloatNode *pilha = NULL;
    char buffer[512];
    strcpy(buffer, StrPosFixa);
    char *tok = strtok(buffer, " ");
    while (tok) {
        if (isNumero(tok)) {
            // converte e empilha
            float v = strtof(tok, NULL);
            pushFloat(&pilha, v);
        }
        else {
            int pr = prioridade(tok);
            if (pr == 5) {
                // função unária
                float a = popFloat(&pilha);
                float r = 0.0f;
                if (!strcmp(tok, "raiz")) {
                    if (a < 0) r = 0.0f; 
                    else r = sqrtf(a);
                }
                else if (!strcmp(tok, "sen")) {
                    r = sinf(a * (float)M_PI / 180.0f);
                }
                else if (!strcmp(tok, "cos")) {
                    r = cosf(a * (float)M_PI / 180.0f);
                }
                else if (!strcmp(tok, "tg")) {
                    r = tanf(a * (float)M_PI / 180.0f);
                }
                else if (!strcmp(tok, "log")) {
                    if (a <= 0) r = 0.0f;
                    else r = log10f(a);
                }
                pushFloat(&pilha, r);
            }
            else if (pr > 0) {
                // operador binário
                float b = popFloat(&pilha);
                float a = popFloat(&pilha);
                float r = 0.0f;
                if (!strcmp(tok, "+"))      r = a + b;
                else if (!strcmp(tok, "-")) r = a - b;
                else if (!strcmp(tok, "*")) r = a * b;
                else if (!strcmp(tok, "/")) r = b != 0.0f ? a / b : 0.0f;
                else if (!strcmp(tok, "%")) r = (float)((int)a % (int)b);
                else if (!strcmp(tok, "^")) r = powf(a, b);
                pushFloat(&pilha, r);
            }
            else {
                // token desconhecido, ignorar ou tratar erro
            }
        }
        tok = strtok(NULL, " ");
    }
    float resultado = popFloat(&pilha);
    return resultado;
}

/*
 * getValorInFixa:
 * Converte a string infixa em pós-fixa e então chama getValorPosFixa().
 */
float getValorInFixa(char *StrInFixa) {
    char *pos = getFormaPosFixa(StrInFixa);
    if (!pos) return 0.0f;
    float v = getValorPosFixa(pos);
    free(pos);
    return v;
}
