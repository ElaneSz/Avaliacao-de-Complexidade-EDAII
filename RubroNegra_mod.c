// Observações:
// - sentinel (nulo) é usado; sempre mantemos nulo->pai consistente
// - funções defensivas para evitar loops no fixup

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

static long contadorInsercaoRB = 0;
static long contadorRemocaoRB  = 0;

/* DEBUG guard para fixup (0 desativa, 1 ativa) */
#define DEBUG_FIXUP 0
#define FIXUP_MAX_ITERS 1000000

ArvoreRB* rb_criar();
void rb_inserir(ArvoreRB*, int);
int rb_remover_chave(ArvoreRB*, int);
void rb_remover_tudo(ArvoreRB*);
long rb_get_insercao_and_reset();
long rb_get_remocao_and_reset();


static NoRB* novo_no(ArvoreRB* arv, NoRB* pai, int valor) {
    NoRB* n = (NoRB*) malloc(sizeof(NoRB));
    n->pai = pai ? pai : arv->nulo;
    n->esquerda = arv->nulo;
    n->direita = arv->nulo;
    n->cor = Vermelho;
    n->valor = valor;
    n->quantidade = 1;
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
    return arv;
}

static void rotacao_esq(ArvoreRB* arv, NoRB* x) {
    contadorInsercaoRB++;
    if (!x || x == arv->nulo) return;
    NoRB* y = x->direita;
    if (!y || y == arv->nulo) return;
    x->direita = y->esquerda;
    if (y->esquerda != arv->nulo) y->esquerda->pai = x;
    y->pai = x->pai;
    if (x->pai == arv->nulo) arv->raiz = y;
    else if (x == x->pai->esquerda) x->pai->esquerda = y;
    else x->pai->direita = y;
    y->esquerda = x;
    x->pai = y;
}

static void rotacao_dir(ArvoreRB* arv, NoRB* x) {
    contadorInsercaoRB++;
    if (!x || x == arv->nulo) return;
    NoRB* y = x->esquerda;
    if (!y || y == arv->nulo) return;
    x->esquerda = y->direita;
    if (y->direita != arv->nulo) y->direita->pai = x;
    y->pai = x->pai;
    if (x->pai == arv->nulo) arv->raiz = y;
    else if (x == x->pai->esquerda) x->pai->esquerda = y;
    else x->pai->direita = y;
    y->direita = x;
    x->pai = y;
}

/* busca */
static NoRB* buscar_no(ArvoreRB* arv, int chave) {
    NoRB* cur = arv->raiz;
    while (cur != arv->nulo) {
        if (chave == cur->valor) return cur;
        if (chave < cur->valor) cur = cur->esquerda;
        else cur = cur->direita;
    }
    return NULL;
}

/* transplant: substitui u por v; garante pai de v apontando corretamente
   Mesmo se v for o sentinel, a função ajusta sentinel->pai para manter consistência. */
static void transplant(ArvoreRB* arv, NoRB* u, NoRB* v) {
    if (!u) return;
    if (u->pai == arv->nulo) {
        arv->raiz = v ? v : arv->nulo;
    } else if (u == u->pai->esquerda) {
        u->pai->esquerda = v ? v : arv->nulo;
    } else {
        u->pai->direita = v ? v : arv->nulo;
    }
    if (v) {
        v->pai = u->pai ? u->pai : arv->nulo;
    } else {
        /* se v == NULL usamos sentinel: garantir sentinel->pai consistente */
        if (arv->nulo) arv->nulo->pai = u->pai ? u->pai : arv->nulo;
    }
}

/* mínimo a partir de x */
static NoRB* minimo(ArvoreRB* arv, NoRB* x) {
    if (!x) return NULL;
    while (x->esquerda != arv->nulo) x = x->esquerda;
    return x;
}

/* delete_fixup com guard defensivo */
static void delete_fixup(ArvoreRB* arv, NoRB* x) {
    long iters = 0;
    NoRB* last_x = NULL;
    long same_count = 0;

    while (x != arv->raiz && x->cor == Preto) {
        iters++;
        if (iters > FIXUP_MAX_ITERS) {
            #if DEBUG_FIXUP
                        fprintf(stderr, "=== RB_DELETE_FIXUP GUARD === iters=%ld x=%p val=%d pai=%p\n",
                                iters, (void*)x, x?x->valor:-1, (void*)(x?x->pai:NULL));
            #endif
                        break;
                    }
                    /* detect repetition */
                    if (last_x == x) {
                        same_count++;
                        if (same_count > 100000) {
            #if DEBUG_FIXUP
                            fprintf(stderr,"rb_delete_fixup: repeated x, aborting\n");
            #endif
                            break;
                        }
        } else {
            last_x = x; same_count = 0;
        }

        contadorRemocaoRB++;
        if (!x || !x->pai) break; /* defensivo */

        if (x == x->pai->esquerda) {
            NoRB* w = x->pai->direita;
            if (!w) break;
            if (w->cor == Vermelho) {
                w->cor = Preto;
                x->pai->cor = Vermelho;
                rotacao_esq(arv, x->pai);
                w = x->pai->direita;
            }
            if ((!w->esquerda || w->esquerda->cor == Preto) &&
                (!w->direita || w->direita->cor == Preto)) {
                w->cor = Vermelho;
                x = x->pai;
            } else {
                if (!w->direita || w->direita->cor == Preto) {
                    if (w->esquerda) w->esquerda->cor = Preto;
                    w->cor = Vermelho;
                    rotacao_dir(arv, w);
                    w = x->pai->direita;
                }
                w->cor = x->pai->cor;
                x->pai->cor = Preto;
                if (w->direita) w->direita->cor = Preto;
                rotacao_esq(arv, x->pai);
                x = arv->raiz;
            }
        } else {
            NoRB* w = x->pai->esquerda;
            if (!w) break;
            if (w->cor == Vermelho) {
                w->cor = Preto;
                x->pai->cor = Vermelho;
                rotacao_dir(arv, x->pai);
                w = x->pai->esquerda;
            }
            if ((!w->direita || w->direita->cor == Preto) &&
                (!w->esquerda || w->esquerda->cor == Preto)) {
                w->cor = Vermelho;
                x = x->pai;
            } else {
                if (!w->esquerda || w->esquerda->cor == Preto) {
                    if (w->direita) w->direita->cor = Preto;
                    w->cor = Vermelho;
                    rotacao_esq(arv, w);
                    w = x->pai->esquerda;
                }
                w->cor = x->pai->cor;
                x->pai->cor = Preto;
                if (w->esquerda) w->esquerda->cor = Preto;
                rotacao_dir(arv, x->pai);
                x = arv->raiz;
            }
        }
    }
    if (x && x != arv->nulo) x->cor = Preto;
}


void rb_inserir(ArvoreRB* arv, int valor) {
    contadorInsercaoRB++;
    NoRB* y = arv->nulo;
    NoRB* x = arv->raiz;
    while (x != arv->nulo) {
        y = x;
        if (valor == x->valor) { x->quantidade++; return; }
        if (valor < x->valor) x = x->esquerda;
        else x = x->direita;
        contadorInsercaoRB++; /* contar visita/comparacao */
    }
    NoRB* z = novo_no(arv, y==arv->nulo?arv->nulo:y, valor);
    if (y == arv->nulo) arv->raiz = z;
    else if (z->valor < y->valor) y->esquerda = z;
    else y->direita = z;

    while (z->pai != arv->nulo && z->pai->cor == Vermelho) {
        contadorInsercaoRB++;
        if (z->pai == z->pai->pai->esquerda) {
            NoRB* y2 = z->pai->pai->direita;
            if (y2->cor == Vermelho) {
                z->pai->cor = Preto;
                y2->cor = Preto;
                z->pai->pai->cor = Vermelho;
                z = z->pai->pai;
            } else {
                if (z == z->pai->direita) {
                    z = z->pai;
                    rotacao_esq(arv, z);
                }
                z->pai->cor = Preto;
                z->pai->pai->cor = Vermelho;
                rotacao_dir(arv, z->pai->pai);
            }
        } else {
            NoRB* y2 = z->pai->pai->esquerda;
            if (y2->cor == Vermelho) {
                z->pai->cor = Preto;
                y2->cor = Preto;
                z->pai->pai->cor = Vermelho;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esquerda) {
                    z = z->pai;
                    rotacao_dir(arv, z);
                }
                z->pai->cor = Preto;
                z->pai->pai->cor = Vermelho;
                rotacao_esq(arv, z->pai->pai);
            }
        }
    }
    arv->raiz->cor = Preto;
}

/* buscar público (conta visitas) */
static NoRB* rb_buscar_public(ArvoreRB* arv, int chave) {
    return buscar_no(arv, chave);
}

/* remoção por chave (CLRS com proteções) */
int rb_remover_chave(ArvoreRB* arv, int chave) {
    if (!arv || arv->raiz == arv->nulo) return 0;
    NoRB* z = rb_buscar_public(arv, chave);
    if (!z) return 0;

    if (z->quantidade > 1) {
        z->quantidade--;
        contadorRemocaoRB++;
        return 1;
    }

    NoRB* y = z;
    NoRB* x;
    Cor y_original_cor = y->cor;

    if (z->esquerda == arv->nulo) {
        x = z->direita;
        transplant(arv, z, z->direita);
    } else if (z->direita == arv->nulo) {
        x = z->esquerda;
        transplant(arv, z, z->esquerda);
    } else {
        y = minimo(arv, z->direita);
        if (!y) return 0; 
        y_original_cor = y->cor;
        x = y->direita;
        if (y->pai == z) {
            if (x) x->pai = y;
        } else {
            transplant(arv, y, y->direita);
            y->direita = z->direita;
            if (y->direita != arv->nulo) y->direita->pai = y;
        }
        transplant(arv, z, y);
        y->esquerda = z->esquerda;
        if (y->esquerda != arv->nulo) y->esquerda->pai = y;
        y->cor = z->cor;
    }

    free(z);
    contadorRemocaoRB++;

    if (y_original_cor == Preto) {
        if (!x) x = arv->nulo;
        delete_fixup(arv, x);
    }
    return 1;
}

/* liberar recursivo */
static void liberar_rec(ArvoreRB* arv, NoRB* n) {
    if (!n || n == arv->nulo) return;
    liberar_rec(arv, n->esquerda);
    liberar_rec(arv, n->direita);
    free(n);
}

void rb_remover_tudo(ArvoreRB* arv) {
    if (!arv) return;
    liberar_rec(arv, arv->raiz);
    arv->raiz = arv->nulo;
    contadorRemocaoRB = 0;
}

/* getters dos contadores */
long rb_get_insercao_and_reset() {
    long v = contadorInsercaoRB;
    contadorInsercaoRB = 0;
    return v;
}
long rb_get_remocao_and_reset() {
    long v = contadorRemocaoRB;
    contadorRemocaoRB = 0;
    return v;
}
