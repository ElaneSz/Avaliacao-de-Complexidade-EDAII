/*
    Compile:
    gcc main_experimento.c AVL_mod.c RubroNegra_mod.c B_mod.c -O2 -o experimento
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* --------------------------------------------------
   DEBUG via linha de comando (--debug)
   -------------------------------------------------- */
static int DEBUG_FLAG = 0;
#define DBG_PRINT(fmt, ...) \
    do { if (DEBUG_FLAG) { printf(fmt, ##__VA_ARGS__); fflush(stdout);} } while(0)

/* --------------------------------------------------
   PARÂMETROS DO EXPERIMENTO
   -------------------------------------------------- */
#define REPETICOES 10
#define N_MAX 10000
#define SAMPLE_STEP 200

/* AVL */
typedef struct arvore1 Arvore1;
Arvore1* avl_criar();
void avl_inserir(Arvore1*, int);
int avl_remover_chave(Arvore1*, int);
void avl_remover_tudo(Arvore1*);
long avl_get_insercao_and_reset();
long avl_get_remocao_and_reset();

/* RB */
typedef struct arvoreRB ArvoreRB;
ArvoreRB* rb_criar();
void rb_inserir(ArvoreRB*, int);
int rb_remover_chave(ArvoreRB*, int);
void rb_remover_tudo(ArvoreRB*);
long rb_get_insercao_and_reset();
long rb_get_remocao_and_reset();

/* B-tree */
typedef struct ArvoreB ArvoreB;
ArvoreB* b_criar(int);
void b_inserir(ArvoreB*, int);
int b_remover_chave(ArvoreB*, int);
void b_remover_tudo(ArvoreB*);
long b_get_insercao_and_reset();
long b_get_remocao_and_reset();


void gerar_chaves_unicas(int *arr, int n)
{
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}


int SAMPLES;
long *avl_ins_acc, *rb_ins_acc, *b1_ins_acc, *b5_ins_acc, *b10_ins_acc;
long *avl_rem_acc, *rb_rem_acc, *b1_rem_acc, *b5_rem_acc, *b10_rem_acc;


int main(int argc, char **argv)
{
    
    if (argc > 1 && strcmp(argv[1], "--debug") == 0) {
        DEBUG_FLAG = 1;
        printf("DEBUG ATIVADO\n");
    }

    srand(12345);
    SAMPLES = (N_MAX + SAMPLE_STEP - 1) / SAMPLE_STEP;

    /* Alocar acumuladores */
    avl_ins_acc = calloc(SAMPLES+2, sizeof(long));
    rb_ins_acc  = calloc(SAMPLES+2, sizeof(long));
    b1_ins_acc  = calloc(SAMPLES+2, sizeof(long));
    b5_ins_acc  = calloc(SAMPLES+2, sizeof(long));
    b10_ins_acc = calloc(SAMPLES+2, sizeof(long));

    avl_rem_acc = calloc(SAMPLES+2, sizeof(long));
    rb_rem_acc  = calloc(SAMPLES+2, sizeof(long));
    b1_rem_acc  = calloc(SAMPLES+2, sizeof(long));
    b5_rem_acc  = calloc(SAMPLES+2, sizeof(long));
    b10_rem_acc = calloc(SAMPLES+2, sizeof(long));

    int *chaves = malloc(sizeof(int) * N_MAX);

    /* Repete experimento */
    for (int rep = 0; rep < REPETICOES; rep++) {

        printf("Repeticao %d/%d\n", rep+1, REPETICOES);
        srand((unsigned)time(NULL) + rep*7919);
        gerar_chaves_unicas(chaves, N_MAX);

        
        Arvore1* avl = avl_criar();
        long avl_ins_total = 0;
        long avl_rem_total = 0;

        for (int n = 1; n <= N_MAX; n++)
        {
            avl_inserir(avl, chaves[n-1]);

            /* Não resetamos! Custo é acumulado. */
            if (n % SAMPLE_STEP == 0) {

                int idx = n / SAMPLE_STEP;

                /* obtém custo acumulado e zera */
                long ins_ops = avl_get_insercao_and_reset();
                avl_ins_acc[idx] += ins_ops;

                /* Remover n elementos */
                for (int k = 0; k < n; k++) {
                    DBG_PRINT("[AVL][REM] %d\n", chaves[k]);
                    avl_remover_chave(avl, chaves[k]);
                }

                long rem_ops = avl_get_remocao_and_reset();
                avl_rem_acc[idx] += rem_ops;

                /* Reconstruir a árvore */
                avl_remover_tudo(avl);
                free(avl);
                avl = avl_criar();

                for (int k = 0; k < n; k++)
                    avl_inserir(avl, chaves[k]);

                avl_get_insercao_and_reset(); 
            }
        }
        avl_remover_tudo(avl);
        avl_get_remocao_and_reset();
        free(avl);

        
        ArvoreRB* rb = rb_criar();
        for (int n = 1; n <= N_MAX; n++)
        {
            rb_inserir(rb, chaves[n-1]);

            if (n % SAMPLE_STEP == 0) {

                int idx = n / SAMPLE_STEP;

                long ins_ops = rb_get_insercao_and_reset();
                rb_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++) {
                    DBG_PRINT("[RB][REM] %d\n", chaves[k]);
                    rb_remover_chave(rb, chaves[k]);
                }

                long rem_ops = rb_get_remocao_and_reset();
                rb_rem_acc[idx] += rem_ops;

                rb_remover_tudo(rb);
                free(rb);
                rb = rb_criar();

                for (int k = 0; k < n; k++)
                    rb_inserir(rb, chaves[k]);

                rb_get_insercao_and_reset();
            }
        }
        rb_remover_tudo(rb);
        rb_get_remocao_and_reset();
        free(rb);

        
        ArvoreB* b1 = b_criar(1);
        for (int n = 1; n <= N_MAX; n++)
        {
            b_inserir(b1, chaves[n-1]);

            if (n % SAMPLE_STEP == 0) {

                int idx = n / SAMPLE_STEP;

                long ins_ops = b_get_insercao_and_reset();
                b1_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++)
                    b_remover_chave(b1, chaves[k]);

                long rem_ops = b_get_remocao_and_reset();
                b1_rem_acc[idx] += rem_ops;

                b_remover_tudo(b1);
                free(b1);
                b1 = b_criar(1);

                for (int k = 0; k < n; k++)
                    b_inserir(b1, chaves[k]);

                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b1);
        b_get_remocao_and_reset();
        free(b1);

       
        ArvoreB* b5 = b_criar(5);
        for (int n = 1; n <= N_MAX; n++)
        {
            b_inserir(b5, chaves[n-1]);

            if (n % SAMPLE_STEP == 0) {

                int idx = n / SAMPLE_STEP;

                long ins_ops = b_get_insercao_and_reset();
                b5_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++)
                    b_remover_chave(b5, chaves[k]);

                long rem_ops = b_get_remocao_and_reset();
                b5_rem_acc[idx] += rem_ops;

                b_remover_tudo(b5);
                free(b5);
                b5 = b_criar(5);

                for (int k = 0; k < n; k++)
                    b_inserir(b5, chaves[k]);

                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b5);
        b_get_remocao_and_reset();
        free(b5);

        ArvoreB* b10 = b_criar(10);
        for (int n = 1; n <= N_MAX; n++)
        {
            b_inserir(b10, chaves[n-1]);

            if (n % SAMPLE_STEP == 0) {

                int idx = n / SAMPLE_STEP;

                long ins_ops = b_get_insercao_and_reset();
                b10_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++)
                    b_remover_chave(b10, chaves[k]);

                long rem_ops = b_get_remocao_and_reset();
                b10_rem_acc[idx] += rem_ops;

                b_remover_tudo(b10);
                free(b10);
                b10 = b_criar(10);

                for (int k = 0; k < n; k++)
                    b_inserir(b10, chaves[k]);

                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b10);
        b_get_remocao_and_reset();
        free(b10);

    } 


    FILE* f_ins = fopen("resultados_insercao_acumulado.csv","w");
    FILE* f_rem = fopen("resultados_remocao_acumulado.csv","w");

    fprintf(f_ins, "tamanho,avl,rb,b1,b5,b10\n");
    fprintf(f_rem, "tamanho,avl,rb,b1,b5,b10\n");

    for (int s = SAMPLE_STEP; s <= N_MAX; s += SAMPLE_STEP)
    {
        int idx = s / SAMPLE_STEP;

        fprintf(f_ins,"%d,%ld,%ld,%ld,%ld,%ld\n",
            s,
            avl_ins_acc[idx] / REPETICOES,
            rb_ins_acc[idx]  / REPETICOES,
            b1_ins_acc[idx]  / REPETICOES,
            b5_ins_acc[idx]  / REPETICOES,
            b10_ins_acc[idx] / REPETICOES
        );

        fprintf(f_rem,"%d,%ld,%ld,%ld,%ld,%ld\n",
            s,
            avl_rem_acc[idx] / REPETICOES,
            rb_rem_acc[idx]  / REPETICOES,
            b1_rem_acc[idx]  / REPETICOES,
            b5_rem_acc[idx]  / REPETICOES,
            b10_rem_acc[idx] / REPETICOES
        );
    }

    fclose(f_ins);
    fclose(f_rem);

    printf("\nArquivos gerados:\n");
    printf(" - resultados_insercao_acumulado.csv\n");
    printf(" - resultados_remocao_acumulado.csv\n");

    free(chaves);
    return 0;
}
