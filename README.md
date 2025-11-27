# Avaliação de Complexidade

## Inserção e Remoção em Árvores AVL, Rubro-Negra e B-Tree

------------------------------------------------------------------------

## 1. Introdução

Este trabalho analisa e compara o custo computacional das operações de
**inserção** e **remoção** em três estruturas de dados balanceadas:

-   **Árvore AVL**
-   **Árvore Rubro-Negra**
-   **Árvore B** (avaliadas nas ordens 1, 5 e 10)

O foco está na **contabilização do esforço interno**, como rotações,
acessos, reestruturações e movimentações de ponteiros.
A análise prática é feita com base no número total de operações
realizadas internamente por cada estrutura.

Os experimentos foram executados com quantidades variando de **1 a
10.000 chaves**, repetidos **10 vezes por tamanho**, usando valores
aleatórios **sem repetição**.

------------------------------------------------------------------------

## 2. Metodologia

### 2.1 Geração das chaves

Para cada tamanho `n`, o processo é:

1.  Geram-se `n` inteiros únicos usando Fisher-Yates.
2.  A estrutura recebe as chaves para inserção.
3.  A estrutura é completamente esvaziada via remoções individuais.
4.  Cada etapa tem suas contagens de operações medidas e acumuladas.

### 2.2 Amostragem estatística

Cada experimento é repetido **10 vezes**, e as médias são calculadas.

Os resultados são salvos automaticamente em:

    resultados_insercao_amostrado.csv
    resultados_remocao_amostrado.csv

Formato:

    tamanho,avl,rb,b1,b5,b10

------------------------------------------------------------------------

## 3. Técnicas Implementadas

### 3.1 Instrumentação detalhada

#### **AVL**

Contadores: 
- VISIT
- MOVE
- HEIGHT
- ROT
- ALLOC
- FREE

#### **Rubro-Negra**

-   visitas
-   movimentos
-   rotações
-   recolorizações
-   alocações/liberações
-   delete_fixup

#### **B-Tree**

-   splits
-   merges
-   redistribuições
-   movimentação de chaves
-   alocações/liberações

### 3.2 Execução automatizada completa

### 3.3 Medição acumulada (construção inteira)

### 3.4 Depuração ativável

### 3.5 Geração automática dos gráficos

------------------------------------------------------------------------

## 4. Implementação

Módulos:

-   AVL_mod.c
-   RubroNegra_mod.c
-   B_mod.c
-   main_experimento.c
-   graficos.py

Fluxo: geração → inserção → medição → remoção → reconstrução → CSV

------------------------------------------------------------------------

## 5. Resultados

### 5.1 Inserção

-   AVL = maior custo
-   Rubro-Negra = menor custo
-   B-tree: custo inversamente proporcional à ordem

### 5.2 Remoção

-   Rubro-Negra = melhor
-   AVL = cara
-   B-tree: t=1 muito caro; t=10 barato

------------------------------------------------------------------------

## 6. Discussão

  Estrutura     Característica          Resultado Esperado   Observado
  ------------- ----------------------- -------------------- ------------
  AVL           balanceamento estrito   custo maior          confirmado
  Rubro-Negra   relaxado                custo menor          confirmado
  B-tree t=1    degenerado              pior custo           confirmado
  B-tree t=10   poucos splits           custo reduzido       confirmado

------------------------------------------------------------------------

## 7. Conclusão

O sistema:

✔ mede esforço real\
✔ gera CSVs automaticamente\
✔ executa todo o experimento sozinho\
✔ gera gráficos lineares/log e acumulados\
✔ é validado até 10.000 chaves

