/*
 Compile:
 gcc main_experimento.c AVL_mod.c RubroNegra_mod.c B_mod.c -O2 -o experimento
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define REPETICOES 10
#define N_MAX 10000
#define SAMPLE_STEP 200

//----------------------------------------------------------
//  ATIVAR / DESATIVAR DEBUG
//----------------------------------------------------------
//
//  DEBUG = 0 → silencioso (produção)
//  DEBUG = 1 → imprime cada inserção e remoção
//----------------------------------------------------------

#define DEBUG 1

#if DEBUG
    #define DBG_PRINT(fmt, ...)  \
        do { printf(fmt, ##__VA_ARGS__); fflush(stdout); } while(0)
#else
    #define DBG_PRINT(fmt, ...)  \
        do {} while(0)
#endif


/* protótipos das funções exportadas pelos módulos */
 /* AVL (usando tipos Arvore1) */
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

 /* B-Tree */
typedef struct ArvoreB ArvoreB;
ArvoreB* b_criar(int);
void b_inserir(ArvoreB*, int);
int b_remover_chave(ArvoreB*, int);
void b_remover_tudo(ArvoreB*);
long b_get_insercao_and_reset();
long b_get_remocao_and_reset();

/* acumuladores de amostras (index = sample idx = n/SAMPLE_STEP) */
int SAMPLES = (N_MAX + SAMPLE_STEP - 1) / SAMPLE_STEP;
long *avl_ins_acc, *rb_ins_acc, *b1_ins_acc, *b5_ins_acc, *b10_ins_acc;
long *avl_rem_acc, *rb_rem_acc, *b1_rem_acc, *b5_rem_acc, *b10_rem_acc;

/* gera chaves únicas e embaralha (Fisher-Yates) */
void gerar_chaves_unicas(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    for (int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

int main() {
    srand(12345);
    SAMPLES = (N_MAX + SAMPLE_STEP - 1) / SAMPLE_STEP;

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

    for (int rep = 0; rep < REPETICOES; rep++) {
        printf("Repeticao %d/%d\n", rep+1, REPETICOES);
        fflush(stdout);
        srand((unsigned) time(NULL) + rep * 7919);
        gerar_chaves_unicas(chaves, N_MAX);

        /* AVL */
        Arvore1* avl = avl_criar();
        for (int n = 1; n <= N_MAX; n++) {
            avl_inserir(avl, chaves[n-1]);
            long ins_ops = avl_get_insercao_and_reset();
            if (n % SAMPLE_STEP == 0) {
                int idx = n / SAMPLE_STEP;
                avl_ins_acc[idx] += ins_ops;

                /* REMOÇÃO REAL: remover chave por chave (na mesma ordem de inserção) */
                for (int k = 0; k < n; k++) {
                    DBG_PRINT("[AVL][REM] chave=%d (n=%d)\n", chaves[k], n);
                    avl_remover_chave(avl, chaves[k]);
                }
                long rem_ops = avl_get_remocao_and_reset();
                avl_rem_acc[idx] += rem_ops;

                /* reconstruir até n para continuar testes (inserir de novo as n chaves) */
                avl_remover_tudo(avl);
                free(avl);
                avl = avl_criar();
                for (int k = 0; k < n; k++) { 
                    DBG_PRINT("[AVL][INS] chave=%d (n=%d)\n", chaves[k], n);
                    avl_inserir(avl, chaves[k]);
                }
                avl_get_insercao_and_reset();
            } else {
                
            }
        }
        avl_remover_tudo(avl); avl_get_remocao_and_reset(); free(avl);

        /* RB */
        ArvoreRB* rb = rb_criar();
        for (int n = 1; n <= N_MAX; n++) {
            rb_inserir(rb, chaves[n-1]);
            long ins_ops = rb_get_insercao_and_reset();
            if (n % SAMPLE_STEP == 0) {
                int idx = n / SAMPLE_STEP;
                rb_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++){
                    DBG_PRINT("[RN][REM] chave=%d (n=%d)\n", chaves[k], n);
                    rb_remover_chave(rb, chaves[k]);
                }
                long rem_ops = rb_get_remocao_and_reset();
                rb_rem_acc[idx] += rem_ops;

                rb_remover_tudo(rb); free(rb);
                rb = rb_criar();
                for (int k = 0; k < n; k++){
                    DBG_PRINT("[RN][INS] chave=%d (n=%d)\n", chaves[k], n);
                    rb_inserir(rb, chaves[k]);
                }
                rb_get_insercao_and_reset();
            }
        }
        rb_remover_tudo(rb); rb_get_remocao_and_reset(); free(rb);

        /* B-tree ordem 1 */
        ArvoreB* b1 = b_criar(1);
        for (int n = 1; n <= N_MAX; n++) {
            b_inserir(b1, chaves[n-1]);
            long ins_ops = b_get_insercao_and_reset();
            if (n % SAMPLE_STEP == 0) {
                int idx = n / SAMPLE_STEP;
                b1_ins_acc[idx] += ins_ops;


                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][REM] chave=%d (n=%d)\n", 1, chaves[k], n);
                    b_remover_chave(b1, chaves[k]);
                }
                long rem_ops = b_get_remocao_and_reset();
                b1_rem_acc[idx] += rem_ops;

                b_remover_tudo(b1); free(b1);
                b1 = b_criar(1);
                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][INS] chave=%d (n=%d)\n", 1, chaves[k], n);
                    b_inserir(b1, chaves[k]);
                } 
                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b1); b_get_remocao_and_reset(); free(b1);

        /* B-tree ordem 5 */
        ArvoreB* b5 = b_criar(5);
        for (int n = 1; n <= N_MAX; n++) {
            b_inserir(b5, chaves[n-1]);
            long ins_ops = b_get_insercao_and_reset();
            if (n % SAMPLE_STEP == 0) {
                int idx = n / SAMPLE_STEP;
                b5_ins_acc[idx] += ins_ops;


                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][REM] chave=%d (n=%d)\n", 5, chaves[k], n);
                    b_remover_chave(b5, chaves[k]);
                }
                long rem_ops = b_get_remocao_and_reset();
                b5_rem_acc[idx] += rem_ops;

                b_remover_tudo(b5); free(b5);
                b5 = b_criar(5);
                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][INS] chave=%d (n=%d)\n", 5, chaves[k], n);
                    b_inserir(b5, chaves[k]);
                }
                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b5); b_get_remocao_and_reset(); free(b5);

        /* B-tree ordem 10 */
        ArvoreB* b10 = b_criar(10);
        for (int n = 1; n <= N_MAX; n++) {
            b_inserir(b10, chaves[n-1]);
            long ins_ops = b_get_insercao_and_reset();
            if (n % SAMPLE_STEP == 0) {
                int idx = n / SAMPLE_STEP;
                b10_ins_acc[idx] += ins_ops;

                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][REM] chave=%d (n=%d)\n", 10, chaves[k], n);
                    b_remover_chave(b10, chaves[k]);
                }
                long rem_ops = b_get_remocao_and_reset();
                b10_rem_acc[idx] += rem_ops;

                b_remover_tudo(b10); free(b10);
                b10 = b_criar(10);
                for (int k = 0; k < n; k++){
                    DBG_PRINT("[B-tree ord=%d][INS] chave=%d (n=%d)\n", 10, chaves[k], n);
                    b_inserir(b10, chaves[k]);
                }
                b_get_insercao_and_reset();
            }
        }
        b_remover_tudo(b10); b_get_remocao_and_reset(); free(b10);

    } /* fim repetições */

    /* salvar CSVs amostrados */
    FILE* f_ins = fopen("resultados_insercao_real_amostrado.csv","w");
    FILE* f_rem = fopen("resultados_remocao_real_amostrado.csv","w");
    if (!f_ins || !f_rem) { perror("fopen"); return 1; }

    fprintf(f_ins, "tamanho,avl,rb,b1,b5,b10\n");
    fprintf(f_rem, "tamanho,avl,rb,b1,b5,b10\n");

    for (int s = SAMPLE_STEP; s <= N_MAX; s += SAMPLE_STEP) {
        int idx = s / SAMPLE_STEP;
        long a_avl = avl_ins_acc[idx] / REPETICOES;
        long a_rb  = rb_ins_acc[idx]  / REPETICOES;
        long a_b1  = b1_ins_acc[idx]  / REPETICOES;
        long a_b5  = b5_ins_acc[idx]  / REPETICOES;
        long a_b10 = b10_ins_acc[idx] / REPETICOES;

        long r_avl = avl_rem_acc[idx] / REPETICOES;
        long r_rb  = rb_rem_acc[idx]  / REPETICOES;
        long r_b1  = b1_rem_acc[idx]  / REPETICOES;
        long r_b5  = b5_rem_acc[idx]  / REPETICOES;
        long r_b10 = b10_rem_acc[idx] / REPETICOES;

        fprintf(f_ins, "%d,%ld,%ld,%ld,%ld,%ld\n", s, a_avl, a_rb, a_b1, a_b5, a_b10);
        fprintf(f_rem, "%d,%ld,%ld,%ld,%ld,%ld\n", s, r_avl, r_rb, r_b1, r_b5, r_b10);
    }

    fclose(f_ins);
    fclose(f_rem);
    free(chaves);

    printf("Arquivos gerados:\n - resultados_insercao_real_amostrado.csv\n - resultados_remocao_real_amostrado.csv\n");
    return 0;
}
