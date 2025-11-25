// AVL com remoção nó-a-nó real (contabiliza operações de remoção)
// Baseado na sua implementação (No1 / Arvore1)

#include <stdlib.h>
#include <stdio.h>

typedef struct no1{
    struct no1* pai;
    struct no1* esquerda;
    struct no1* direita;
    int quantidade;
    int altura;
    int valor;
} No1;

typedef struct arvore1{
    struct no1* raiz;
} Arvore1;


static long contadorInsercaoAVL = 0;
static long contadorRemocaoAVL  = 0;


Arvore1* avl_criar();
void avl_inserir(Arvore1*, int);

/* remove uma chave (uma ocorrência) e contabiliza operações */
int avl_remover_chave(Arvore1*, int);

/* remove tudo (destruir árvore) — não contabiliza rebalanceamentos,
   apenas libera memória (mantido por compatibilidade) */
void avl_remover_tudo(Arvore1*);

long avl_get_insercao_and_reset();
long avl_get_remocao_and_reset();



int altura(No1 *no) {
    if(no==NULL) return 0;
    return no->altura;
}

int maior(int a, int b){
    return a > b ? a : b;
}

Arvore1* avl_criar() {
    Arvore1 *arvore = (Arvore1*) malloc(sizeof(Arvore1));
    arvore->raiz = NULL;
    return arvore;
}

No1* criaNodo(int valor, No1* pai) {
    No1* novo = (No1*) malloc(sizeof(No1));
    novo->valor     = valor;
    novo->pai       = pai;
    novo->direita   = NULL;
    novo->esquerda  = NULL;
    novo->quantidade = 1;
    novo->altura     = 1;
    return novo;
}

No1* rse(Arvore1* arvore, No1* no);
No1* rsd(Arvore1* arvore, No1* no);
No1* rde(Arvore1* arvore, No1* no);
No1* rdd(Arvore1* arvore, No1* no);

int fb(No1* no) {
    return altura(no->esquerda) - altura(no->direita);
}

No1* adicionarNo(No1* no, int valor) {
    contadorInsercaoAVL++;

    if(valor > no->valor){
        if(no->direita == NULL){
            No1* novo = criaNodo(valor, no);
            no->direita = novo;
            return novo;
        }
        return adicionarNo(no->direita, valor);

    } else if(valor < no->valor){
        if(no->esquerda == NULL){
            No1* novo = criaNodo(valor, no);
            no->esquerda = novo;
            return novo;
        }
        return adicionarNo(no->esquerda, valor);

    } else {
        no->quantidade++;
        return no;
    }
}

No1* adicionar(Arvore1* arvore, int valor) {

    contadorInsercaoAVL++;

    if(arvore->raiz == NULL){
        No1* novo = criaNodo(valor, NULL);
        arvore->raiz = novo;
        return novo;
    }

    No1* no = adicionarNo(arvore->raiz, valor);

    /* faz rebalance subindo */
    while(no != NULL) {

        no->altura = 1 + maior(altura(no->esquerda), altura(no->direita));

        int fator = fb(no);

        if(fator > 1) {
            if(fb(no->esquerda) >= 0) rsd(arvore, no);
            else rdd(arvore, no);

        } else if(fator < -1) {
            if(fb(no->direita) <= 0) rse(arvore, no);
            else rde(arvore, no);
        }

        no = no->pai;
    }

    return arvore->raiz;
}

void avl_inserir(Arvore1* a, int valor){
    adicionar(a, valor);
}

void remover_no_libera(Arvore1* arvore, No1* no) {
    if(no == NULL) return;

    if(no->esquerda != NULL) remover_no_libera(arvore, no->esquerda);
    if(no->direita  != NULL) remover_no_libera(arvore, no->direita);

    /* Apenas conta frees como operações simples */
    contadorRemocaoAVL++;
    free(no);
}

void avl_remover_tudo(Arvore1* a){
    remover_no_libera(a, a->raiz);
    a->raiz = NULL;
}

long avl_get_insercao_and_reset(){
    long v = contadorInsercaoAVL;
    contadorInsercaoAVL = 0;
    return v;
}

long avl_get_remocao_and_reset(){
    long v = contadorRemocaoAVL;
    contadorRemocaoAVL = 0;
    return v;
}


No1* rse(Arvore1* arvore, No1* no){
    contadorInsercaoAVL++;

    No1* pai     = no->pai;
    No1* direita = no->direita;

    no->direita = direita->esquerda;
    if(direita->esquerda != NULL)
        direita->esquerda->pai = no;

    no->pai = direita;
    direita->esquerda = no;
    direita->pai = pai;

    no->altura = maior(altura(no->esquerda), altura(no->direita)) + 1;
    direita->altura =
        maior(altura(direita->esquerda), altura(direita->direita)) + 1;

    if(pai == NULL)
        arvore->raiz = direita;
    else if(pai->esquerda == no)
        pai->esquerda = direita;
    else
        pai->direita  = direita;

    return direita;
}

No1* rsd(Arvore1* arvore, No1* no){
    contadorInsercaoAVL++;

    No1* pai     = no->pai;
    No1* esquerda = no->esquerda;

    no->esquerda = esquerda->direita;
    if(esquerda->direita != NULL)
        esquerda->direita->pai = no;

    no->pai = esquerda;
    esquerda->direita = no;
    esquerda->pai = pai;

    no->altura =
        maior(altura(no->esquerda), altura(no->direita)) + 1;
    esquerda->altura =
        maior(altura(esquerda->esquerda), altura(esquerda->direita)) + 1;

    if(pai == NULL)
        arvore->raiz = esquerda;
    else if(pai->esquerda == no)
        pai->esquerda = esquerda;
    else
        pai->direita  = esquerda;

    return esquerda;
}

No1* rde(Arvore1* arvore, No1* no){
    no->direita = rsd(arvore, no->direita);
    return rse(arvore, no);
}

No1* rdd(Arvore1* arvore, No1* no){
    no->esquerda = rse(arvore, no->esquerda);
    return rsd(arvore, no);
}


/* Procura nó com valor dado (retorna ponteiro, sem contar operações) */
No1* avl_localizar(No1* raiz, int valor) {
    No1* cur = raiz;
    while(cur != NULL) {
        contadorRemocaoAVL++; /* conta comparação/visita */
        if(valor == cur->valor) return cur;
        if(valor < cur->valor) cur = cur->esquerda;
        else cur = cur->direita;
    }
    return NULL;
}

/* encontra o menor nó na subárvore (successor mínimo) */
No1* avl_minimo(No1* no) {
    No1* cur = no;
    while(cur->esquerda != NULL) {
        contadorRemocaoAVL++; /* visita */
        cur = cur->esquerda;
    }
    return cur;
}

/* atualiza alturas e faz rebalance a partir de node subindo até a raiz
   contabiliza rotações/atualizações em contadorRemocaoAVL */
void avl_balancear_depois_remocao(Arvore1* arvore, No1* no) {
    No1* cur = no;
    while(cur != NULL) {
        /* atualização de altura conta como operação */
        contadorRemocaoAVL++;
        cur->altura = 1 + maior(altura(cur->esquerda), altura(cur->direita));
        int fator = fb(cur);

        if(fator > 1) {
            if(fb(cur->esquerda) >= 0) {
                rsd(arvore, cur);
                contadorRemocaoAVL++; /* conta rotação */
            } else {
                rdd(arvore, cur);
                contadorRemocaoAVL += 2; /* duas rotações */
            }
        } else if(fator < -1) {
            if(fb(cur->direita) <= 0) {
                rse(arvore, cur);
                contadorRemocaoAVL++; /* conta rotação */
            } else {
                rde(arvore, cur);
                contadorRemocaoAVL += 2;
            }
        }

        cur = cur->pai;
    }
}

/* Remove nó apontado por z (BST deletion), mantendo links corretos.
   Não faz rebalance aqui — apenas remoção estrutural; retorna o pai
   onde o rebalance deve começar (ou NULL). */
No1* avl_delete_node(Arvore1* arvore, No1* z) {
    if(z == NULL) return NULL;

    No1* start_balance = NULL;

    if(z->esquerda == NULL || z->direita == NULL) {
        /* y é z */
        No1* y = z;
        No1* x = (y->esquerda != NULL) ? y->esquerda : y->direita;
        No1* p = y->pai;

        if(x != NULL) x->pai = p;

        if(p == NULL) {
            /* y era raiz */
            arvore->raiz = x;
        } else {
            if(p->esquerda == y) p->esquerda = x;
            else p->direita = x;
            start_balance = p;
        }

        free(y);
        contadorRemocaoAVL++; /* conta remoção do nó */

    } else {
        /* z tem dois filhos: substituir pelo sucessor (mínimo da subárvore direita) */
        No1* s = avl_minimo(z->direita);
        /* copiar valor e quantidade (aquí tratamos como chave única) */
        z->valor = s->valor;
        /* remover s (que tem no máximo um filho direito) */
        start_balance = avl_delete_node(arvore, s);
        /* nesse caso já liberou s e devolveu ponto de rebalance */
    }

    return start_balance;
}

/* Interface pública: remove uma ocorrência da chave.
   Retorna 1 se removido, 0 se não encontrado. */
int avl_remover_chave(Arvore1* arvore, int chave) {
    if(arvore == NULL || arvore->raiz == NULL) return 0;

    No1* nodo = avl_localizar(arvore->raiz, chave);
    if(nodo == NULL) return 0;

    /* caso quantidade > 1 apenas decrementa contagem */
    if(nodo->quantidade > 1) {
        nodo->quantidade--;
        contadorRemocaoAVL++; /* conta decremento */
        return 1;
    }

    /* remove fisicamente o nó e obtém ponto para rebalance */
    No1* start = avl_delete_node(arvore, nodo);

    /* rebalance a partir de 'start' (ou da raiz, se start NULL) */
    if(start == NULL) start = arvore->raiz;
    avl_balancear_depois_remocao(arvore, start);

    return 1;
}

/* Mantém compatibilidade exportando um nome curto */
int avl_remover(Arvore1* a, int chave){
    return avl_remover_chave(a, chave);
}


