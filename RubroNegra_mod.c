// Contadores detalhados: RB_COUNT_VISIT, RB_COUNT_MOVE, RB_COUNT_HEIGHT, RB_COUNT_ROT, RB_COUNT_ALLOC, RB_COUNT_FREE

#include <stdlib.h>
#include <stdio.h>

enum coloracao {Vermelho, Preto};
typedef enum coloracao Cor;

typedef struct noRB {
    struct noRB* pai;
    struct noRB* esquerda;
    struct noRB* direita;
    Cor cor;
    int valor;
    int quantidade;
} NoRB;

typedef struct arvoreRB {
    NoRB* raiz;
    NoRB* nulo; /* sentinel */
} ArvoreRB;

static long RB_COUNT_VISIT  = 0;   // comparações / visitas (navegação)
static long RB_COUNT_MOVE   = 0;   // atribuições / mov. ponteiros (links)
static long RB_COUNT_HEIGHT = 0;   // atualizações estruturais (p/ compatibilidade)
static long RB_COUNT_ROT    = 0;   // rotações
static long RB_COUNT_ALLOC  = 0;   // alocações de nós
static long RB_COUNT_FREE   = 0;   // liberações de nós


long rb_get_insercao_and_reset() {
    long v = RB_COUNT_VISIT + RB_COUNT_MOVE + RB_COUNT_ROT + RB_COUNT_ALLOC;
    RB_COUNT_VISIT = RB_COUNT_MOVE = RB_COUNT_HEIGHT = RB_COUNT_ROT = RB_COUNT_ALLOC = 0;
    return v;
}

long rb_get_remocao_and_reset() {
    long v = RB_COUNT_VISIT + RB_COUNT_MOVE + RB_COUNT_ROT + RB_COUNT_FREE;
    RB_COUNT_VISIT = RB_COUNT_MOVE = RB_COUNT_HEIGHT = RB_COUNT_ROT = RB_COUNT_FREE = 0;
    return v;
}

ArvoreRB* rb_criar();
void rb_inserir(ArvoreRB*, int);
int rb_remover_chave(ArvoreRB*, int);
void rb_remover_tudo(ArvoreRB*);

/* macros internas para contagem */
#define RB_VISIT()  (RB_COUNT_VISIT++)
#define RB_MOVE()   (RB_COUNT_MOVE++)
#define RB_ROT()    (RB_COUNT_ROT++)
#define RB_ALLOC()  (RB_COUNT_ALLOC++)
#define RB_FREE()   (RB_COUNT_FREE++)
#define RB_HEIGHT() (RB_COUNT_HEIGHT++)


static NoRB* novo_no(ArvoreRB* arv, NoRB* pai, int valor) {
    NoRB* n = (NoRB*) malloc(sizeof(NoRB));
    n->pai = pai ? pai : arv->nulo;
    n->esquerda = arv->nulo;
    n->direita = arv->nulo;
    n->cor = Vermelho;
    n->valor = valor;
    n->quantidade = 1;
    RB_ALLOC();
    RB_MOVE(); /* ponteiros iniciais */
    return n;
}

ArvoreRB* rb_criar() {
    ArvoreRB* arv = (ArvoreRB*) malloc(sizeof(ArvoreRB));
    arv->nulo = (NoRB*) malloc(sizeof(NoRB));
    /* inicializa sentinel */
    arv->nulo->pai = arv->nulo;
    arv->nulo->esquerda = arv->nulo->direita = arv->nulo;
    arv->nulo->cor = Preto;
    arv->nulo->valor = 0;
    arv->nulo->quantidade = 0;
    arv->raiz = arv->nulo;
    RB_ALLOC(); RB_ALLOC(); /* uma para a arvore e outra para sentinel (contagem simbólica) */
    return arv;
}

/* rotações instrumentadas */
static void rotacao_esq(ArvoreRB* arv, NoRB* x) {
    if (!x || x == arv->nulo) return;
    RB_ROT();
    NoRB* y = x->direita;
    if (!y || y == arv->nulo) return;
    x->direita = y->esquerda; RB_MOVE();
    if (y->esquerda != arv->nulo) { y->esquerda->pai = x; RB_MOVE(); }
    y->pai = x->pai; RB_MOVE();
    if (x->pai == arv->nulo) { arv->raiz = y; RB_MOVE(); }
    else if (x == x->pai->esquerda) { x->pai->esquerda = y; RB_MOVE(); }
    else { x->pai->direita = y; RB_MOVE(); }
    y->esquerda = x; RB_MOVE();
    x->pai = y; RB_MOVE();
    RB_HEIGHT(); RB_HEIGHT();
}

static void rotacao_dir(ArvoreRB* arv, NoRB* x) {
    if (!x || x == arv->nulo) return;
    RB_ROT();
    NoRB* y = x->esquerda;
    if (!y || y == arv->nulo) return;
    x->esquerda = y->direita; RB_MOVE();
    if (y->direita != arv->nulo) { y->direita->pai = x; RB_MOVE(); }
    y->pai = x->pai; RB_MOVE();
    if (x->pai == arv->nulo) { arv->raiz = y; RB_MOVE(); }
    else if (x == x->pai->esquerda) { x->pai->esquerda = y; RB_MOVE(); }
    else { x->pai->direita = y; RB_MOVE(); }
    y->direita = x; RB_MOVE();
    x->pai = y; RB_MOVE();
    RB_HEIGHT(); RB_HEIGHT();
}

/* busca (visitas contadas) */
static NoRB* buscar_no(ArvoreRB* arv, int chave) {
    NoRB* cur = arv->raiz;
    while (cur != arv->nulo) {
        RB_VISIT();
        if (chave == cur->valor) return cur;
        if (chave < cur->valor) cur = cur->esquerda;
        else cur = cur->direita;
    }
    return NULL;
}

/* transplant (instrumentado) */
static void transplant(ArvoreRB* arv, NoRB* u, NoRB* v) {
    if (!u) return;
    if (u->pai == arv->nulo) { arv->raiz = v ? v : arv->nulo; RB_MOVE(); }
    else if (u == u->pai->esquerda) { u->pai->esquerda = v ? v : arv->nulo; RB_MOVE(); }
    else { u->pai->direita = v ? v : arv->nulo; RB_MOVE(); }
    if (v) { v->pai = u->pai; RB_MOVE(); }
    else { if (arv->nulo) arv->nulo->pai = u->pai; RB_MOVE(); }
}

/* minimo */
static NoRB* minimo(ArvoreRB* arv, NoRB* x) {
    if (!x) return NULL;
    while (x->esquerda != arv->nulo) {
        RB_VISIT();
        x = x->esquerda;
    }
    return x;
}

/* delete_fixup com contagem */
static void delete_fixup(ArvoreRB* arv, NoRB* x) {
    long iters = 0;
    NoRB* last_x = NULL;
    long same_count = 0;

    while (x != arv->raiz && x->cor == Preto) {
        iters++;
        if (last_x == x) {
            same_count++;
            if (same_count > 100000) break;
        } else { last_x = x; same_count = 0; }

        RB_VISIT();

        if (!x || !x->pai) break;

        if (x == x->pai->esquerda) {
            NoRB* w = x->pai->direita;
            if (!w) break;
            if (w->cor == Vermelho) {
                w->cor = Preto; RB_MOVE();
                x->pai->cor = Vermelho; RB_MOVE();
                rotacao_esq(arv, x->pai);
                w = x->pai->direita;
            }
            if ((!w->esquerda || w->esquerda->cor == Preto) && (!w->direita || w->direita->cor == Preto)) {
                w->cor = Vermelho; RB_MOVE();
                x = x->pai;
            } else {
                if (!w->direita || w->direita->cor == Preto) {
                    if (w->esquerda) { w->esquerda->cor = Preto; RB_MOVE(); }
                    w->cor = Vermelho; RB_MOVE();
                    rotacao_dir(arv, w);
                    w = x->pai->direita;
                }
                w->cor = x->pai->cor; RB_MOVE();
                x->pai->cor = Preto; RB_MOVE();
                if (w->direita) { w->direita->cor = Preto; RB_MOVE(); }
                rotacao_esq(arv, x->pai);
                x = arv->raiz;
            }
        } else {
            NoRB* w = x->pai->esquerda;
            if (!w) break;
            if (w->cor == Vermelho) {
                w->cor = Preto; RB_MOVE();
                x->pai->cor = Vermelho; RB_MOVE();
                rotacao_dir(arv, x->pai);
                w = x->pai->esquerda;
            }
            if ((!w->direita || w->direita->cor == Preto) && (!w->esquerda || w->esquerda->cor == Preto)) {
                w->cor = Vermelho; RB_MOVE();
                x = x->pai;
            } else {
                if (!w->esquerda || w->esquerda->cor == Preto) {
                    if (w->direita) { w->direita->cor = Preto; RB_MOVE(); }
                    w->cor = Vermelho; RB_MOVE();
                    rotacao_esq(arv, w);
                    w = x->pai->esquerda;
                }
                w->cor = x->pai->cor; RB_MOVE();
                x->pai->cor = Preto; RB_MOVE();
                if (w->esquerda) { w->esquerda->cor = Preto; RB_MOVE(); }
                rotacao_dir(arv, x->pai);
                x = arv->raiz;
            }
        }
    }
    if (x && x != arv->nulo) { x->cor = Preto; RB_MOVE(); }
}

/* inserção pública */
void rb_inserir(ArvoreRB* arv, int valor) {
    RB_VISIT();
    NoRB* y = arv->nulo;
    NoRB* x = arv->raiz;
    while (x != arv->nulo) {
        y = x;
        RB_VISIT();
        if (valor == x->valor) { x->quantidade++; RB_MOVE(); return; }
        if (valor < x->valor) x = x->esquerda;
        else x = x->direita;
    }
    NoRB* z = novo_no(arv, y==arv->nulo?arv->nulo:y, valor);
    if (y == arv->nulo) { arv->raiz = z; RB_MOVE(); }
    else if (z->valor < y->valor) { y->esquerda = z; RB_MOVE(); }
    else { y->direita = z; RB_MOVE(); }

    while (z->pai != arv->nulo && z->pai->cor == Vermelho) {
        RB_VISIT();
        if (z->pai == z->pai->pai->esquerda) {
            NoRB* y2 = z->pai->pai->direita;
            if (y2->cor == Vermelho) {
                z->pai->cor = Preto; RB_MOVE();
                y2->cor = Preto; RB_MOVE();
                z->pai->pai->cor = Vermelho; RB_MOVE();
                z = z->pai->pai;
            } else {
                if (z == z->pai->direita) {
                    z = z->pai;
                    rotacao_esq(arv, z);
                }
                z->pai->cor = Preto; RB_MOVE();
                z->pai->pai->cor = Vermelho; RB_MOVE();
                rotacao_dir(arv, z->pai->pai);
            }
        } else {
            NoRB* y2 = z->pai->pai->esquerda;
            if (y2->cor == Vermelho) {
                z->pai->cor = Preto; RB_MOVE();
                y2->cor = Preto; RB_MOVE();
                z->pai->pai->cor = Vermelho; RB_MOVE();
                z = z->pai->pai;
            } else {
                if (z == z->pai->esquerda) {
                    z = z->pai;
                    rotacao_dir(arv, z);
                }
                z->pai->cor = Preto; RB_MOVE();
                z->pai->pai->cor = Vermelho; RB_MOVE();
                rotacao_esq(arv, z->pai->pai);
            }
        }
    }
    arv->raiz->cor = Preto; RB_MOVE();
}

/* buscar público */
static NoRB* rb_buscar_public(ArvoreRB* arv, int chave) {
    return buscar_no(arv, chave);
}

/* remoção por chave (CLRS com proteções) */
int rb_remover_chave(ArvoreRB* arv, int chave) {
    if (!arv || arv->raiz == arv->nulo) return 0;
    NoRB* z = rb_buscar_public(arv, chave);
    if (!z) return 0;

    if (z->quantidade > 1) {
        z->quantidade--; RB_MOVE();
        RB_VISIT();
        return 1;
    }

    NoRB* y = z;
    NoRB* x;
    Cor y_original_cor = y->cor;

    if (z->esquerda == arv->nulo) {
        x = z->direita; RB_VISIT();
        transplant(arv, z, z->direita);
    } else if (z->direita == arv->nulo) {
        x = z->esquerda; RB_VISIT();
        transplant(arv, z, z->esquerda);
    } else {
        y = minimo(arv, z->direita);
        if (!y) return 0;
        y_original_cor = y->cor;
        x = y->direita;
        if (y->pai == z) {
            if (x) { x->pai = y; RB_MOVE(); }
        } else {
            transplant(arv, y, y->direita);
            y->direita = z->direita; RB_MOVE();
            if (y->direita != arv->nulo) { y->direita->pai = y; RB_MOVE(); }
        }
        transplant(arv, z, y);
        y->esquerda = z->esquerda; RB_MOVE();
        if (y->esquerda != arv->nulo) { y->esquerda->pai = y; RB_MOVE(); }
        y->cor = z->cor; RB_MOVE();
    }

    free(z); RB_FREE();

    if (y_original_cor == Preto) {
        if (!x) x = arv->nulo;
        delete_fixup(arv, x);
    }
    return 1;
}

static void liberar_rec(ArvoreRB* arv, NoRB* n) {
    if (!n || n == arv->nulo) return;
    liberar_rec(arv, n->esquerda);
    liberar_rec(arv, n->direita);
    free(n); RB_FREE();
}

void rb_remover_tudo(ArvoreRB* arv) {
    if (!arv) return;
    liberar_rec(arv, arv->raiz);
    arv->raiz = arv->nulo;
    RB_COUNT_VISIT = RB_COUNT_MOVE = RB_COUNT_HEIGHT = RB_COUNT_ROT = RB_COUNT_ALLOC = RB_COUNT_FREE = 0;
}
