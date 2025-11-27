import pandas as pd
import matplotlib.pyplot as plt

# -----------------------------------------------------------
#   Leitura dos CSVs 
# -----------------------------------------------------------

insercao = pd.read_csv("resultados_insercao_acumulado.csv")
remocao  = pd.read_csv("resultados_remocao_acumulado.csv")

# ===========================================================
#   GRÁFICO 1 — Inserção (escala linear)
# ===========================================================

plt.figure(figsize=(12,6))

plt.plot(insercao["tamanho"], insercao["avl"],  label="AVL", linewidth=2)
plt.plot(insercao["tamanho"], insercao["rb"],   label="Rubro-Negra", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b1"],   label="B-tree (ord. 1)", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b5"],   label="B-tree (ord. 5)", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b10"],  label="B-tree (ord.10)", linewidth=2)

plt.title("Custo ACUMULADO de Inserção por Estrutura")
plt.xlabel("Tamanho n")
plt.ylabel("Custo acumulado (operações)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_insercao_acumulado_linear.png", dpi=300)
plt.show()

# ===========================================================
#   GRÁFICO 2 — Inserção (escala log)
# ===========================================================

plt.figure(figsize=(12,6))

plt.plot(insercao["tamanho"], insercao["avl"],  label="AVL", linewidth=2)
plt.plot(insercao["tamanho"], insercao["rb"],   label="Rubro-Negra", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b1"],   label="B-tree (ord. 1)", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b5"],   label="B-tree (ord. 5)", linewidth=2)
plt.plot(insercao["tamanho"], insercao["b10"],  label="B-tree (ord.10)", linewidth=2)

plt.title("Custo ACUMULADO de Inserção (Escala Log)")
plt.xlabel("Tamanho n")
plt.ylabel("Custo acumulado (log)")
plt.yscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_insercao_acumulado_log.png", dpi=300)
plt.show()

# ===========================================================
#   GRÁFICO 3 — Remoção (escala linear)
# ===========================================================

plt.figure(figsize=(12,6))

plt.plot(remocao["tamanho"], remocao["avl"],  label="AVL", linewidth=2)
plt.plot(remocao["tamanho"], remocao["rb"],   label="Rubro-Negra", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b1"],   label="B-tree (ord. 1)", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b5"],   label="B-tree (ord. 5)", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b10"],  label="B-tree (ord.10)", linewidth=2)

plt.title("Custo ACUMULADO de Remoção (Esvaziamento)")
plt.xlabel("Tamanho n")
plt.ylabel("Custo acumulado (operações)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_remocao_acumulado_linear.png", dpi=300)
plt.show()

# ===========================================================
#   GRÁFICO 4 — Remoção (escala log)
# ===========================================================

plt.figure(figsize=(12,6))

plt.plot(remocao["tamanho"], remocao["avl"],  label="AVL", linewidth=2)
plt.plot(remocao["tamanho"], remocao["rb"],   label="Rubro-Negra", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b1"],   label="B-tree (ord. 1)", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b5"],   label="B-tree (ord. 5)", linewidth=2)
plt.plot(remocao["tamanho"], remocao["b10"],  label="B-tree (ord.10)", linewidth=2)

plt.title("Custo ACUMULADO de Remoção (Escala Log)")
plt.xlabel("Tamanho n")
plt.ylabel("Custo acumulado (log)")
plt.yscale("log")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_remocao_acumulado_log.png", dpi=300)
plt.show()
