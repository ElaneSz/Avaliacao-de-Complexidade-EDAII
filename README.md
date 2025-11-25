# Relatório de Avaliação de Complexidade

## Inserção e Remoção em Árvores AVL, Rubro-Negra e B-Tree

---

## 1. Introdução

Este trabalho compara o custo computacional das operações de inserção e remoção em três estruturas de dados balanceadas:

* **Árvore AVL**
* **Árvore Rubro-Negra**
* **Árvore B** (avaliada nas ordens 1, 5 e 10)

O experimento mede o total de operações internas de cada algoritmo, incluindo rotações, acessos, reestruturações e movimentações realizadas para garantir o balanceamento.

Foram executados testes variando o número de chaves de **1 a 10.000**, repetidos **10 vezes para cada tamanho**, utilizando valores aleatórios sem repetição.

---

## 2. Metodologia

### 2.1 Geração das chaves

Para cada tamanho `n`:

1. Geram-se `n` chaves aleatórias únicas utilizando `rand()`.
2. Cada estrutura:

   * recebe todas as chaves para inserção;
   * tem o custo registrado;
   * é totalmente esvaziada via remoções individuais;
   * tem o custo de remoção também registrado.

### 2.2 Amostragem estatística

Cada valor de `n` é repetido **10 vezes**, sendo calculadas as médias.

Os resultados são salvos em dois arquivos CSV:

```
resultados_insercao_amostrado.csv
resultados_remocao_amostrado.csv
```

Cada arquivo contém colunas no formato:

```
tamanho,avl,rb,b1,b5,b10
```

---

## 3. Implementação

Foram escritos quatro módulos principais:

* `AVL_mod.c`
* `RubroNegra_mod.c`
* `B_mod.c`
* `main_experimento.c`

O programa principal:

1. executa todos os testes de 1 a 10.000 elementos;
2. repete 10 vezes cada tamanho;
3. acumula contagens de operações;
4. salva em CSV automaticamente;
5. não exige interação do usuário após compilado.

### 3.1 Geração dos gráficos

Os gráficos são gerados por `graficos.py`, produzindo:

* `grafico_insercao_linear.png`
* `grafico_remocao_linear.png`
* `grafico_insercao_log.png`
* `grafico_remocao_log.png`

Com escala linear e logarítmica no eixo Y.

---

## 4. Ajustes e Depuração

Durante o desenvolvimento:

* a versão inicial da **Rubro-Negra** apresentava falhas em remoção devido a estados ilegais no `delete_fixup`.
  → Foi revisada e tornou-se mais robusta, incluindo tratamento para ponteiros nulos e casos extremos.

* a **B-tree de ordem 1** apresentava loops infinitos durante fusão.
  → `B_mod.c` foi corrigido para operar corretamente mesmo no limite mínimo (`t=1`).

* Mensagens de depuração (`DEBUG`) foram incluídas e podem ser habilitadas com:

```c
#define DEBUG 1
```

Após correções, todos os testes até 10.000 chaves executaram sem travamentos.

---

## 5. Resultados

### 5.1 Inserção

O gráfico de inserção (`grafico_insercao_linear.png` ou `grafico_insercao_log.png`) mostra o comportamento das estruturas.

#### Conclusões:

* **AVL** apresentou maior custo devido ao rebalanceamento rigoroso.
* **Rubro-Negra** teve custo menor e mais estável.
* **B-Tree** depende fortemente da ordem:

  * `t=1`: mais divisões → maior custo,
  * `t=10`: menos divisões → menor custo.

### 5.2 Remoção

O gráfico de remoção (`grafico_remocao_linear.png` ou `grafico_remocao_log.png`) apresenta o esforço para esvaziar as estruturas.

#### Resultados:

* **Rubro-Negra** novamente obteve o menor custo médio.
* **AVL** exigiu muitas operações de rebalanceamento.
* **B-Tree** apresentou:

  * custo decrescente conforme `t` aumenta,
  * pois menos fusões e empréstimos são necessários.

---

## 6. Discussão

Os resultados estão consistentes com a teoria clássica:

| Estrutura   | Observação Principal                          |
| ----------- | --------------------------------------------- |
| AVL         | Alto custo devido a balanceamento forte       |
| Rubro-Negra | Melhor compromisso entre esforço e eficiência |
| B-Tree      | Quanto maior a ordem, menor o custo           |

A Rubro-Negra se mostrou a estrutura mais eficiente para memória primária, enquanto B-Trees de ordem maior são extremamente vantajosas em cenários de armazenamento secundário como bancos de dados.

---

## 7. Conclusão

O experimento permitiu:

* medir empiricamente os custos de inserção e remoção;
* comprovar a influência da ordem da B-tree no desempenho;
* gerar gráficos comparativos claros;
* automatizar completamente a geração dos dados e estatísticas.

A partir de uma única execução, o sistema:

1. gera dados,
2. executa todos os experimentos,
3. coleta operações,
4. grava CSV,
5. permite gerar gráficos imediatamente.
