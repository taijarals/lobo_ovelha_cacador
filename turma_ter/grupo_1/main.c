#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "includes/simulacao.h"

int main() {
    int linhas, colunas;
    int total_rodadas = 10;

    // Inicializa
    srand(time(NULL));

    printf("=== SIMULADOR DE ECOSSISTEMA BA-VI (Bamor x TUI x PM) ===\n\n");

    // Validação do tamanho do tabuleiro
    do {
        printf("Digite a quantidade de linhas do tabuleiro (minimo 5): ");
        scanf("%d", &linhas);
        printf("Digite a quantidade de colunas do tabuleiro (minimo 5): ");
        scanf("%d", &colunas);

        if (linhas < 5 || colunas < 5) {
            printf("\n[ERRO] O tamanho minimo permitido eh 5x5. Tente novamente.\n\n");
        }
    } while (linhas < 5 || colunas < 5);

    printf("\nGerando o tabuleiro %dx%d...\n", linhas, colunas);

    // 1. Alocação
    Tabuleiro* meu_tabuleiro = criar_tabuleiro(linhas, colunas);
    if (meu_tabuleiro == NULL) {
        printf("[ERRO FATAL] Falha de alocacao de memoria.\n");
        return 1;
    }

    // 2. Posicionamento
    inicializar_elementos(meu_tabuleiro);

    printf("\n--- ESTADO INICIAL DO MAPA ---\n");
    exibir_tabuleiro(meu_tabuleiro);

    // Limpa o buffer do teclado deixado pelo scanf para o getchar() funcionar direito
    while(getchar() != '\n');

    // 3. Loop Principal de Simulação
    for (int i = 1; i <= total_rodadas; i++) {
        printf("\nPressione ENTER para iniciar a Rodada %d...", i);
        getchar(); // Pausa a tela até o usuário apertar Enter

        processar_rodada(meu_tabuleiro);
        
        printf("\n--- RESULTADO DA RODADA %d ---\n", i);
        exibir_tabuleiro(meu_tabuleiro);
    }

    printf("\nSimulacao de %d rodadas concluida!\n\n", total_rodadas);

    // 4. Liberação de Memória
    printf("Limpando a memoria e encerrando...\n");
    destruir_tabuleiro(meu_tabuleiro);
    printf("Memoria liberada com sucesso!\n");

    printf("\nPressione ENTER para fechar o simulador...");
    getchar(); 

    return 0;
}