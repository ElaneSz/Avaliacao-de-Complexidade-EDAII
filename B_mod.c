// Contadores: visitas (busca), moves (shifts), splits, merges, alloc, free, etc.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NoB {
    int *chaves;
    struct NoB **filhos;
    int n;        // número de chaves atualmente
    int folha;    // 1 se folha
} NoB;

typedef struct ArvoreB {
    NoB* raiz;
    int t; // ordem mínima (t)
} ArvoreB;

static long B_COUNT_VISIT = 0;
static long B_COUNT_MOVE  = 0;
static long B_COUNT_SPLIT = 0;
static long B_COUNT_MERGE = 0;
static long B_COUNT_ALLOC = 0;
static long B_COUNT_FREE  = 0;

long b_get_insercao_and_reset() {
    long v = B_COUNT_VISIT + B_COUNT_MOVE + B_COUNT_SPLIT + B_COUNT_ALLOC;
    B_COUNT_VISIT = B_COUNT_MOVE = B_COUNT_SPLIT = B_COUNT_MERGE = B_COUNT_ALLOC = B_COUNT_FREE = 0;
    return v;
}
long b_get_remocao_and_reset() {
    long v = B_COUNT_VISIT + B_COUNT_MOVE + B_COUNT_MERGE + B_COUNT_FREE;
    B_COUNT_VISIT = B_COUNT_MOVE = B_COUNT_SPLIT = B_COUNT_MERGE = B_COUNT_ALLOC = B_COUNT_FREE = 0;
    return v;
}

ArvoreB* b_criar(int ordem);
void b_inserir(ArvoreB*, int);
int b_remover_chave(ArvoreB*, int);
void b_remover_tudo(ArvoreB*);

#define B_VISIT() (B_COUNT_VISIT++)
#define B_MOVE()  (B_COUNT_MOVE++)
#define B_SPLIT() (B_COUNT_SPLIT++)
#define B_MERGE() (B_COUNT_MERGE++)
#define B_ALLOC() (B_COUNT_ALLOC++)
#define B_FREE()  (B_COUNT_FREE++)

NoB* b_novo_no(int t, int folha) {
    NoB* x = (NoB*) malloc(sizeof(NoB));
    x->folha = folha;
    int keys_size = (2 * t - 1 > 0) ? (2 * t - 1) : 1;
    int childs_size = (2 * t > 0) ? (2 * t) : 1;
    x->chaves = (int*) malloc(sizeof(int) * keys_size);
    x->filhos = (NoB**) malloc(sizeof(NoB*) * childs_size);
    x->n = 0;
    for (int i = 0; i < childs_size; i++) x->filhos[i] = NULL;
    B_ALLOC(); B_MOVE();
    return x;
}

ArvoreB* b_criar(int ordem) {
    if (ordem < 1) ordem = 1;
    ArvoreB* a = (ArvoreB*) malloc(sizeof(ArvoreB));
    a->t = ordem;
    a->raiz = b_novo_no(ordem, 1);
    B_ALLOC();
    return a;
}

/* busca se chave existe na subárvore (conta visitas) */
int b_buscar(NoB* x, int k) {
    if (x == NULL) return 0;
    int i = 0;
    while (i < x->n && k > x->chaves[i]) {
        B_VISIT();
        i++;
    }
    if (i < x->n && k == x->chaves[i]) return 1;
    if (x->folha) return 0;
    if (x->filhos[i] == NULL) return 0;
    return b_buscar(x->filhos[i], k);
}

/* split child (instrumentado) */
void b_split_child(NoB* x, int i, int t) {
    B_SPLIT();
    NoB* y = x->filhos[i];
    NoB* z = b_novo_no(t, y->folha);
    z->n = t - 1;
    /* move chaves */
    for (int j = 0; j < t - 1; j++) {
        z->chaves[j] = y->chaves[j + t];
        B_MOVE();
    }
    /* move filhos se houver */
    if (!y->folha) {
        for (int j = 0; j < t; j++) {
            z->filhos[j] = y->filhos[j + t];
            B_MOVE();
            y->filhos[j + t] = NULL; /* evita ponteiro pendente */
        }
    }
    y->n = t - 1;

    /* shift filhos em x */
    int child_cap = 2 * t > 0 ? 2 * t : 1;
    for (int j = child_cap - 2; j >= i + 1; j--) {
        x->filhos[j + 1] = x->filhos[j]; B_MOVE();
    }
    x->filhos[i + 1] = z; B_MOVE();

    /* shift chaves em x */
    int keys_cap = (2 * t - 1 > 0) ? (2 * t - 1) : 1;
    for (int j = keys_cap - 2; j >= i; j--) {
        x->chaves[j + 1] = x->chaves[j]; B_MOVE();
    }
    x->chaves[i] = y->chaves[t - 1]; B_MOVE();
    x->n = x->n + 1; B_MOVE();
}

void b_insert_nonfull(NoB* x, int k, int t) {
    B_VISIT();
    int i = x->n - 1;
    if (x->folha) {
        while (i >= 0 && x->chaves[i] > k) {
            x->chaves[i + 1] = x->chaves[i]; B_MOVE();
            i--;
            B_VISIT();
        }
        x->chaves[i + 1] = k; B_MOVE();
        x->n = x->n + 1; B_MOVE();
    } else {
        while (i >= 0 && x->chaves[i] > k) {
            i--;
            B_VISIT();
        }
        i++;
        /* proteção: se filho é NULL, cria um novo nó */
        if (x->filhos[i] == NULL) {
            x->filhos[i] = b_novo_no(t, 1); B_MOVE();
        }
        if (x->filhos[i]->n == 2 * t - 1) {
            b_split_child(x, i, t);
            if (x->chaves[i] < k) i++;
        }
        b_insert_nonfull(x->filhos[i], k, t);
    }
}

void b_inserir(ArvoreB* a, int k) {
    B_VISIT();
    NoB* r = a->raiz;
    if (r->n == 2 * a->t - 1) {
        NoB* s = b_novo_no(a->t, 0); B_MOVE();
        a->raiz = s; B_MOVE();
        s->filhos[0] = r; B_MOVE();
        b_split_child(s, 0, a->t);
        int i = 0;
        if (s->chaves[0] < k) i++;
        b_insert_nonfull(s->filhos[i], k, a->t);
    } else {
        b_insert_nonfull(r, k, a->t);
    }
}

int b_get_pred(NoB* x, int idx, ArvoreB* a) {
    NoB* cur = x->filhos[idx];
    if (!cur) return 0;
    while (!cur->folha) {
        B_VISIT();
        if (cur->filhos[cur->n]) cur = cur->filhos[cur->n];
        else break;
    }
    return cur->chaves[cur->n - 1];
}

int b_get_succ(NoB* x, int idx) {
    NoB* cur = x->filhos[idx + 1];
    if (!cur) return 0;
    while (!cur->folha) {
        B_VISIT();
        if (cur->filhos[0]) cur = cur->filhos[0];
        else break;
    }
    return cur->chaves[0];
}

/* b_fill (instrumentado) */
void b_fill(NoB* x, int idx, int t) {
    if (x == NULL) return;
    if (idx < 0 || idx > x->n) return;

    if (idx != 0 && x->filhos[idx - 1] != NULL && x->filhos[idx - 1]->n >= t) {
        NoB* child = x->filhos[idx];
        NoB* sibling = x->filhos[idx - 1];
        for (int i = child->n - 1; i >= 0; --i) {
            child->chaves[i + 1] = child->chaves[i]; B_MOVE();
        }
        if (!child->folha) {
            for (int i = child->n; i >= 0; --i) {
                child->filhos[i + 1] = child->filhos[i]; B_MOVE();
            }
        }
        child->chaves[0] = x->chaves[idx - 1]; B_MOVE();
        if (!child->folha) child->filhos[0] = sibling->filhos[sibling->n], B_MOVE();
        x->chaves[idx - 1] = sibling->chaves[sibling->n - 1]; B_MOVE();
        child->n += 1; B_MOVE();
        sibling->n -= 1; B_MOVE();
        return;
    }

    if (idx != x->n && x->filhos[idx + 1] != NULL && x->filhos[idx + 1]->n >= t) {
        NoB* child = x->filhos[idx];
        NoB* sibling = x->filhos[idx + 1];
        child->chaves[child->n] = x->chaves[idx]; B_MOVE();
        if (!child->folha) child->filhos[child->n + 1] = sibling->filhos[0], B_MOVE();
        x->chaves[idx] = sibling->chaves[0]; B_MOVE();
        for (int i = 0; i < sibling->n - 1; ++i) {
            sibling->chaves[i] = sibling->chaves[i + 1]; B_MOVE();
        }
        if (!sibling->folha) {
            for (int i = 0; i < sibling->n; ++i) {
                sibling->filhos[i] = sibling->filhos[i + 1]; B_MOVE();
            }
        }
        child->n += 1; B_MOVE();
        sibling->n -= 1; B_MOVE();
        return;
    }

    if (idx != x->n && x->filhos[idx + 1] != NULL) {
        NoB* child = x->filhos[idx];
        NoB* sibling = x->filhos[idx + 1];
        if (t - 1 >= 0) child->chaves[t - 1] = x->chaves[idx], B_MOVE();
        for (int i = 0; i < sibling->n; ++i) {
            child->chaves[i + t] = sibling->chaves[i]; B_MOVE(); B_MERGE();
        }
        if (!child->folha) {
            for (int i = 0; i <= sibling->n; ++i) {
                child->filhos[i + t] = sibling->filhos[i]; B_MOVE(); B_MERGE();
            }
        }
        for (int i = idx + 1; i < x->n; ++i) {
            x->chaves[i - 1] = x->chaves[i]; B_MOVE();
            x->filhos[i] = x->filhos[i + 1]; B_MOVE();
        }
        x->filhos[x->n] = NULL; B_MOVE();
        child->n += sibling->n + 1; B_MOVE();
        x->n--; B_MOVE();
        if (sibling) {
            if (sibling->chaves) free(sibling->chaves), B_FREE();
            if (sibling->filhos) free(sibling->filhos), B_FREE();
            free(sibling); B_FREE();
            x->filhos[idx + 1] = NULL; B_MOVE();
        }
        return;
    } else if (idx - 1 >= 0 && x->filhos[idx - 1] != NULL) {
        NoB* child = x->filhos[idx - 1];
        NoB* sibling = x->filhos[idx];
        if (t - 1 >= 0) child->chaves[t - 1] = x->chaves[idx - 1], B_MOVE();
        for (int i = 0; i < sibling->n; ++i) {
            child->chaves[i + t] = sibling->chaves[i]; B_MOVE(); B_MERGE();
        }
        if (!child->folha) {
            for (int i = 0; i <= sibling->n; ++i) {
                child->filhos[i + t] = sibling->filhos[i]; B_MOVE(); B_MERGE();
            }
        }
        for (int i = idx; i < x->n; ++i) {
            x->chaves[i - 1] = x->chaves[i]; B_MOVE();
            x->filhos[i] = x->filhos[i + 1]; B_MOVE();
        }
        x->filhos[x->n] = NULL; B_MOVE();
        child->n += sibling->n + 1; B_MOVE();
        x->n--; B_MOVE();
        if (sibling) {
            if (sibling->chaves) free(sibling->chaves), B_FREE();
            if (sibling->filhos) free(sibling->filhos), B_FREE();
            free(sibling); B_FREE();
            x->filhos[idx] = NULL; B_MOVE();
        }
        return;
    }
}

void b_remove_from_node(NoB* x, int k, ArvoreB* a) {
    if (x == NULL) return;
    int t = a->t;
    int idx = 0;
    while (idx < x->n && x->chaves[idx] < k) {
        idx++;
        B_VISIT();
    }

    if (idx < x->n && x->chaves[idx] == k) {
        if (x->folha) {
            /* remove from leaf */
            for (int i = idx + 1; i < x->n; ++i) {
                x->chaves[i - 1] = x->chaves[i]; B_MOVE();
            }
            x->n--; B_MOVE();
            return;
        } else {
            /* non-leaf */
            if (x->filhos[idx] != NULL && x->filhos[idx]->n >= t) {
                int pred = b_get_pred(x, idx, a); B_VISIT();
                x->chaves[idx] = pred; B_MOVE();
                b_remove_from_node(x->filhos[idx], pred, a);
            } else if (x->filhos[idx + 1] != NULL && x->filhos[idx + 1]->n >= t) {
                int succ = b_get_succ(x, idx); B_VISIT();
                x->chaves[idx] = succ; B_MOVE();
                b_remove_from_node(x->filhos[idx + 1], succ, a);
            } else {
                if (x->filhos[idx] == NULL || x->filhos[idx + 1] == NULL) {
                    return;
                }
                NoB* child = x->filhos[idx];
                NoB* sibling = x->filhos[idx + 1];
                if (t - 1 >= 0) child->chaves[t - 1] = x->chaves[idx], B_MOVE();
                for (int i = 0; i < sibling->n; ++i) {
                    child->chaves[i + t] = sibling->chaves[i]; B_MOVE(); B_MERGE();
                }
                if (!child->folha) {
                    for (int i = 0; i <= sibling->n; ++i) {
                        child->filhos[i + t] = sibling->filhos[i]; B_MOVE(); B_MERGE();
                    }
                }
                for (int i = idx + 1; i < x->n; ++i) {
                    x->chaves[i - 1] = x->chaves[i]; B_MOVE();
                    x->filhos[i] = x->filhos[i + 1]; B_MOVE();
                }
                x->filhos[x->n] = NULL; B_MOVE();
                child->n += sibling->n + 1; B_MOVE();
                x->n--; B_MOVE();
                if (sibling) {
                    if (sibling->chaves) free(sibling->chaves), B_FREE();
                    if (sibling->filhos) free(sibling->filhos), B_FREE();
                    free(sibling); B_FREE();
                    x->filhos[idx + 1] = NULL; B_MOVE();
                }
                b_remove_from_node(child, k, a);
            }
        }
    } else {
        if (x->folha) {
            return;
        }
        int flag = ((idx == x->n) ? 1 : 0);
        if (x->filhos[idx] == NULL) return;
        if (x->filhos[idx]->n < t) {
            b_fill(x, idx, t);
        }

        if (flag && idx > x->n) {
            if (idx - 1 >= 0 && x->filhos[idx - 1] != NULL)
                b_remove_from_node(x->filhos[idx - 1], k, a);
            else
                return;
        } else {
            if (idx > x->n) idx = x->n;
            if (x->filhos[idx] != NULL)
                b_remove_from_node(x->filhos[idx], k, a);
            else
                return;
        }
    }
}

int b_remover_chave(ArvoreB* a, int k) {
    if (!a || !a->raiz) return 0;
    if (!b_buscar(a->raiz, k)) return 0;

    b_remove_from_node(a->raiz, k, a);

    if (a->raiz->n == 0) {
        NoB* tmp = a->raiz;
        if (a->raiz->folha) {
            if (tmp->chaves) free(tmp->chaves), B_FREE();
            if (tmp->filhos) free(tmp->filhos), B_FREE();
            free(tmp); B_FREE();
            a->raiz = b_novo_no(a->t, 1); B_ALLOC();
        } else {
            NoB* newroot = a->raiz->filhos[0];
            if (tmp->chaves) free(tmp->chaves), B_FREE();
            if (tmp->filhos) free(tmp->filhos), B_FREE();
            free(tmp); B_FREE();
            a->raiz = newroot ? newroot : b_novo_no(a->t, 1); B_ALLOC();
        }
    }
    B_VISIT();
    return 1;
}

/* remover tudo (liberação recursiva) */
void b_liberar(NoB* no) {
    if (!no) return;
    if (!no->folha) {
        for (int i = 0; i <= no->n; ++i) {
            if (no->filhos[i]) b_liberar(no->filhos[i]);
        }
    }
    if (no->chaves) free(no->chaves), B_FREE();
    if (no->filhos) free(no->filhos), B_FREE();
    free(no); B_FREE();
}

void b_remover_tudo(ArvoreB* a) {
    if (!a) return;
    if (a->raiz) b_liberar(a->raiz);
    a->raiz = b_novo_no(a->t, 1); B_ALLOC();
}
