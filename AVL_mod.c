// Conta de forma consistente:
// - comparações/visitas (COUNT_VISIT)
// - movimentação de ponteiros/atribuições estruturais (COUNT_MOVE)
// - atualizações de altura (COUNT_HEIGHT)
// - rotações (COUNT_ROT)
// - alocação/liberação de nós (COUNT_ALLOC / COUNT_FREE)
// A métrica final usada por main_experimento será a soma de todas as categorias.
// Exporta funções:
//   Arvore1* avl_criar();
//   void avl_inserir(Arvore1*, int);
//   int avl_remover_chave(Arvore1*, int); // remove 1 ocorrência
//   void avl_remover_tudo(Arvore1*);
//   long avl_get_insercao_and_reset();
//   long avl_get_remocao_and_reset();

#include <stdlib.h>
#include <stdio.h>

typedef struct no1 {
    struct no1* pai;
    struct no1* esquerda;
    struct no1* direita;
    int altura;
    int valor;
    int quantidade;
} No1;

typedef struct arvore1 {
    No1* raiz;
} Arvore1;


static long AVL_COUNT_VISIT = 0;   // comparações / visitas (navegação)
static long AVL_COUNT_MOVE  = 0;   // atribuições / mov. ponteiros (links)
static long AVL_COUNT_HEIGHT = 0;  // atualizações de altura
static long AVL_COUNT_ROT   = 0;   // rotações (cada rotação conta 1)
static long AVL_COUNT_ALLOC = 0;   // alocações de nós
static long AVL_COUNT_FREE  = 0;   // liberações de nós

/* wrappers para obter "esforço total" e reset */
long avl_get_insercao_and_reset() {
    long total = AVL_COUNT_VISIT + AVL_COUNT_MOVE + AVL_COUNT_HEIGHT + AVL_COUNT_ROT + AVL_COUNT_ALLOC;
    // reset dos contadores de insercao (assumimos que main chama isso após cada inserção em bloco)
    AVL_COUNT_VISIT = AVL_COUNT_MOVE = AVL_COUNT_HEIGHT = AVL_COUNT_ROT = AVL_COUNT_ALLOC = 0;
    return total;
}
long avl_get_remocao_and_reset() {
    long total = AVL_COUNT_VISIT + AVL_COUNT_MOVE + AVL_COUNT_HEIGHT + AVL_COUNT_ROT + AVL_COUNT_FREE;
    AVL_COUNT_VISIT = AVL_COUNT_MOVE = AVL_COUNT_HEIGHT = AVL_COUNT_ROT = AVL_COUNT_FREE = 0;
    return total;
}


static inline void COUNT_VISIT() { AVL_COUNT_VISIT++; }
static inline void COUNT_MOVE()  { AVL_COUNT_MOVE++; }
static inline void COUNT_HEIGHT(){ AVL_COUNT_HEIGHT++; }
static inline void COUNT_ROT()   { AVL_COUNT_ROT++; }
static inline void COUNT_ALLOC() { AVL_COUNT_ALLOC++; }
static inline void COUNT_FREEF() { AVL_COUNT_FREE++; }

int altura_no(No1* n) {
    if (!n) return 0;
    return n->altura;
}

int max(int a, int b) { return a > b ? a : b; }

Arvore1* avl_criar() {
    Arvore1* a = (Arvore1*) malloc(sizeof(Arvore1));
    a->raiz = NULL;
    return a;
}

No1* novo_no_avl(int valor, No1* pai) {
    No1* n = (No1*) malloc(sizeof(No1));
    n->valor = valor;
    n->pai = pai;
    n->esquerda = n->direita = NULL;
    n->quantidade = 1;
    n->altura = 1;
    COUNT_ALLOC();
    COUNT_MOVE(); // atribuições de ponteiro iniciais
    return n;
}

/* Rotações instrumentadas (RSD = rotacao simples direita, RSE = esquerda) */

static No1* rotacao_esq(Arvore1* a, No1* x) {
    if (!x) return x;
    COUNT_ROT();
    No1* y = x->direita;
    // conte visitas/movimentos
    COUNT_VISIT(); COUNT_MOVE();

    x->direita = y->esquerda; COUNT_MOVE();
    if (y->esquerda) { y->esquerda->pai = x; COUNT_MOVE(); }
    y->pai = x->pai; COUNT_MOVE();

    if (!x->pai) {
        a->raiz = y; COUNT_MOVE();
    } else if (x == x->pai->esquerda) {
        x->pai->esquerda = y; COUNT_MOVE();
    } else {
        x->pai->direita = y; COUNT_MOVE();
    }

    y->esquerda = x; COUNT_MOVE();
    x->pai = y; COUNT_MOVE();

    // atualiza alturas (contabilizar)
    x->altura = 1 + max(altura_no(x->esquerda), altura_no(x->direita)); COUNT_HEIGHT();
    y->altura = 1 + max(altura_no(y->esquerda), altura_no(y->direita)); COUNT_HEIGHT();

    return y;
}

static No1* rotacao_dir(Arvore1* a, No1* y) {
    if (!y) return y;
    COUNT_ROT();
    No1* x = y->esquerda;
    COUNT_VISIT(); COUNT_MOVE();

    y->esquerda = x->direita; COUNT_MOVE();
    if (x->direita) { x->direita->pai = y; COUNT_MOVE(); }
    x->pai = y->pai; COUNT_MOVE();

    if (!y->pai) {
        a->raiz = x; COUNT_MOVE();
    } else if (y == y->pai->esquerda) {
        y->pai->esquerda = x; COUNT_MOVE();
    } else {
        y->pai->direita = x; COUNT_MOVE();
    }

    x->direita = y; COUNT_MOVE();
    y->pai = x; COUNT_MOVE();

    // atualiza alturas
    y->altura = 1 + max(altura_no(y->esquerda), altura_no(y->direita)); COUNT_HEIGHT();
    x->altura = 1 + max(altura_no(x->esquerda), altura_no(x->direita)); COUNT_HEIGHT();

    return x;
}

static int fator_balanceamento(No1* n) {
    if (!n) return 0;
    COUNT_VISIT();
    return altura_no(n->esquerda) - altura_no(n->direita);
}

/* Inserção (BST) com rebalanceamento AVL */

static No1* avl_inserir_rec(Arvore1* a, No1* node, int chave, No1* pai, int* criado_novo) {
    if (!node) {
        *criado_novo = 1;
        No1* novo = novo_no_avl(chave, pai);
        return novo;
    }
    COUNT_VISIT(); // visitando node
    if (chave < node->valor) {
        node->esquerda = avl_inserir_rec(a, node->esquerda, chave, node, criado_novo);
        if (node->esquerda) node->esquerda->pai = node, COUNT_MOVE();
    } else if (chave > node->valor) {
        node->direita = avl_inserir_rec(a, node->direita, chave, node, criado_novo);
        if (node->direita) node->direita->pai = node, COUNT_MOVE();
    } else {
        node->quantidade++;
        COUNT_MOVE();
        return node;
    }

    // atualiza altura
    node->altura = 1 + max(altura_no(node->esquerda), altura_no(node->direita));
    COUNT_HEIGHT();

    // balanceamento
    int fb = fator_balanceamento(node);

    // Left Left
    if (fb > 1 && chave < node->esquerda->valor) {
        return rotacao_dir(a, node);
    }
    // Right Right
    if (fb < -1 && chave > node->direita->valor) {
        return rotacao_esq(a, node);
    }
    // Left Right
    if (fb > 1 && chave > node->esquerda->valor) {
        node->esquerda = rotacao_esq(a, node->esquerda);
        if (node->esquerda) node->esquerda->pai = node, COUNT_MOVE();
        return rotacao_dir(a, node);
    }
    // Right Left
    if (fb < -1 && chave < node->direita->valor) {
        node->direita = rotacao_dir(a, node->direita);
        if (node->direita) node->direita->pai = node, COUNT_MOVE();
        return rotacao_esq(a, node);
    }

    return node;
}

void avl_inserir(Arvore1* a, int chave) {
    int criado = 0;
    a->raiz = avl_inserir_rec(a, a->raiz, chave, NULL, &criado);
    if (a->raiz) a->raiz->pai = NULL, COUNT_MOVE();
}


static No1* avl_minimo(No1* node) {
    if (!node) return NULL;
    No1* cur = node;
    while (cur->esquerda) { COUNT_VISIT(); cur = cur->esquerda; }
    return cur;
}

/* função recursiva que remove e reequilibra, retorna nova raiz do subárvore */
static No1* avl_remover_rec(Arvore1* a, No1* node, int chave, int* removed) {
    if (!node) return NULL;
    COUNT_VISIT();
    if (chave < node->valor) {
        node->esquerda = avl_remover_rec(a, node->esquerda, chave, removed);
        if (node->esquerda) node->esquerda->pai = node, COUNT_MOVE();
    } else if (chave > node->valor) {
        node->direita = avl_remover_rec(a, node->direita, chave, removed);
        if (node->direita) node->direita->pai = node, COUNT_MOVE();
    } else {
        // encontramos
        *removed = 1;
        if (node->quantidade > 1) {
            node->quantidade--; COUNT_MOVE();
            return node;
        }
        // nó com um ou nenhum filho
        if (!node->esquerda || !node->direita) {
            No1* temp = node->esquerda ? node->esquerda : node->direita;
            if (!temp) {
                // sem filhos
                temp = node;
                node = NULL;
            } else {
                // um filho: copiar conteúdo
                COUNT_MOVE();
                *node = *temp;
            }
            COUNT_FREEF(); free(temp);
        } else {
            // dois filhos: pegar sucessor (mínimo na direita)
            No1* temp = avl_minimo(node->direita);
            COUNT_VISIT();
            node->valor = temp->valor; COUNT_MOVE();
            node->quantidade = temp->quantidade; COUNT_MOVE();
            // remover sucessor (uma cópia)
            node->direita = avl_remover_rec(a, node->direita, temp->valor, removed);
            if (node->direita) node->direita->pai = node, COUNT_MOVE();
        }
    }

    if (!node) return node;

    // atualizar altura
    node->altura = 1 + max(altura_no(node->esquerda), altura_no(node->direita));
    COUNT_HEIGHT();

    // balance
    int fb = fator_balanceamento(node);

    // Left Left
    if (fb > 1 && fator_balanceamento(node->esquerda) >= 0) {
        return rotacao_dir(a, node);
    }
    // Left Right
    if (fb > 1 && fator_balanceamento(node->esquerda) < 0) {
        node->esquerda = rotacao_esq(a, node->esquerda);
        if (node->esquerda) node->esquerda->pai = node, COUNT_MOVE();
        return rotacao_dir(a, node);
    }
    // Right Right
    if (fb < -1 && fator_balanceamento(node->direita) <= 0) {
        return rotacao_esq(a, node);
    }
    // Right Left
    if (fb < -1 && fator_balanceamento(node->direita) > 0) {
        node->direita = rotacao_dir(a, node->direita);
        if (node->direita) node->direita->pai = node, COUNT_MOVE();
        return rotacao_esq(a, node);
    }

    return node;
}

int avl_remover_chave(Arvore1* a, int chave) {
    int removed = 0;
    a->raiz = avl_remover_rec(a, a->raiz, chave, &removed);
    if (a->raiz) a->raiz->pai = NULL, COUNT_MOVE();
    return removed;
}

/* Esvaziar a árvore removendo nodos um a um (usado para medir custo real de remoção) */
void avl_remover_tudo(Arvore1* a) {
    if (!a) return;
    
    int capacity = 1024;
    int *vals = (int*) malloc(sizeof(int) * capacity);
    int cnt = 0;
    No1** stack = (No1**) malloc(sizeof(No1*) * 1024);
    int top = 0;
    No1* cur = a->raiz;
    while ((cur != NULL)) {
        while (cur) {
            if (top % 1024 == 0) {
                ;
            }
            stack[top++] = cur;
            cur = cur->esquerda;
        }
        if (top == 0) break;
        No1* node = stack[--top];
        for (int q = 0; q < node->quantidade; ++q) {
            if (cnt >= capacity) {
                capacity *= 2;
                vals = (int*) realloc(vals, sizeof(int) * capacity);
            }
            vals[cnt++] = node->valor;
        }
        cur = node->direita;
    }
    free(stack);
    for (int i = 0; i < cnt; ++i) {
        avl_remover_chave(a, vals[i]);
    }
    free(vals);
}
