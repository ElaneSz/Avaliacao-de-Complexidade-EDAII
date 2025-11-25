import pandas as pd
import matplotlib.pyplot as plt

# -------------------------------------------------------------
# CONFIGURAÇÕES GERAIS
# -------------------------------------------------------------

ARQUIVO_INS = "resultados_insercao_real_amostrado.csv"
ARQUIVO_REM = "resultados_remocao_real_amostrado.csv"

ESTRUTURAS = ["avl", "rb", "b1", "b5", "b10"]
ROTULOS = {
    "avl": "AVL",
    "rb": "Rubro-Negra",
    "b1": "B-Tree (ordem 1)",
    "b5": "B-Tree (ordem 5)",
    "b10": "B-Tree (ordem 10)"
}

# -------------------------------------------------------------
# FUNÇÃO PARA PLOTAR
# -------------------------------------------------------------
def plotar(df, titulo, nome_arquivo_base):
    plt.figure(figsize=(10,6))
    for col in ESTRUTURAS:
        plt.plot(df["tamanho"], df[col], marker='o', label=ROTULOS[col])
    
    plt.title(titulo)
    plt.xlabel("Número de Elementos (N)")
    plt.ylabel("Custo Total de Operações")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(nome_arquivo_base + "_linear.png", dpi=300)
    print(f"✔ Gráfico gerado: {nome_arquivo_base}_linear.png")

    # Log scale
    plt.yscale("log")
    plt.title(titulo + " (Escala Log)")
    plt.tight_layout()
    plt.savefig(nome_arquivo_base + "_log.png", dpi=300)
    print(f"✔ Gráfico gerado: {nome_arquivo_base}_log.png")


# -------------------------------------------------------------
# CARREGAR DADOS
# -------------------------------------------------------------
ins = pd.read_csv(ARQUIVO_INS)
rem = pd.read_csv(ARQUIVO_REM)

print("Arquivos carregados com sucesso!")

# -------------------------------------------------------------
# PLOTAR
# -------------------------------------------------------------
plotar(ins, 
       "Custo de Inserção — Arvores Balanceadas e B-Tree", 
       "grafico_insercao")

plotar(rem, 
       "Custo de Remoção — Arvores Balanceadas e B-Tree", 
       "grafico_remocao")

print("\nFinalizado.")
