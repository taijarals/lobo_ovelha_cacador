#include <stdio.h>
#include <stdlib.h>
#include "../includes/simulacao.h"

// 1. Função para criar e alocar o mapa na memória
Tabuleiro* criar_tabuleiro(int l, int c) {
    Tabuleiro* tab = (Tabuleiro*)malloc(sizeof(Tabuleiro));
    tab->linhas = l;
    tab->colunas = c;
    tab->lista_entidades = NULL; // Inicia a Lista Encadeada vazia

    // Aloca a matriz (linhas)
    tab->grade = (TipoEntidade**)malloc(l * sizeof(TipoEntidade*));
    for (int i = 0; i < l; i++) {
        // Aloca as colunas de cada linha e preenche com VAZIO
        tab->grade[i] = (TipoEntidade*)malloc(c * sizeof(TipoEntidade));
        for (int j = 0; j < c; j++) {
            tab->grade[i][j] = VAZIO;
        }
    }
    return tab;
}

// 2. Função auxiliar para adicionar personagens na Lista Encadeada
void adicionar_entidade(Tabuleiro* tab, int id, TipoEntidade tipo, int x, int y) {
    Entidade* nova = (Entidade*)malloc(sizeof(Entidade));
    nova->id = id;
    nova->tipo = tipo;
    nova->x = x;
    nova->y = y;
    nova->ativo = 1; // Nasce vivo
    
    // Adiciona no início da Lista Encadeada
    nova->prox = tab->lista_entidades;
    tab->lista_entidades = nova;
    
    // Marca na Matriz para colisões rápidas
    tab->grade[x][y] = tipo;
}

// 3. Função para povoar o tabuleiro com as proporções exigidas
void inicializar_elementos(Tabuleiro* tab) {
    int total_casas = tab->linhas * tab->colunas;
    
    // Calcula as porcentagens
    int qtd_bamor = (total_casas * 12) / 100;
    int qtd_tui = (total_casas * 12) / 100;
    int qtd_pm = (total_casas * 5) / 100;
    int qtd_obstaculos = (total_casas * 10) / 100;

    // Garante que pelo menos 1 de cada exista se o mapa for muito pequeno
    if (qtd_bamor == 0) qtd_bamor = 1;
    if (qtd_tui == 0) qtd_tui = 1;
    if (qtd_pm == 0) qtd_pm = 1;

    int id_geral = 1;
    int alocados, rx, ry;

    // Distribui a Bamor
    alocados = 0;
    while (alocados < qtd_bamor) {
        rx = rand() % tab->linhas;
        ry = rand() % tab->colunas;
        if (tab->grade[rx][ry] == VAZIO) {
            adicionar_entidade(tab, id_geral++, BAMOR, rx, ry);
            alocados++;
        }
    }

    // Distribui a TUI
    alocados = 0;
    while (alocados < qtd_tui) {
        rx = rand() % tab->linhas;
        ry = rand() % tab->colunas;
        if (tab->grade[rx][ry] == VAZIO) {
            adicionar_entidade(tab, id_geral++, TUI, rx, ry);
            alocados++;
        }
    }

    // Distribui a PM
    alocados = 0;
    while (alocados < qtd_pm) {
        rx = rand() % tab->linhas;
        ry = rand() % tab->colunas;
        if (tab->grade[rx][ry] == VAZIO) {
            adicionar_entidade(tab, id_geral++, PM, rx, ry);
            alocados++;
        }
    }

    // Distribui os Obstáculos (Não entram na Lista Encadeada, só na Matriz)
    alocados = 0;
    while (alocados < qtd_obstaculos) {
        rx = rand() % tab->linhas;
        ry = rand() % tab->colunas;
        if (tab->grade[rx][ry] == VAZIO) {
            TipoEntidade obs = (rand() % 2 == 0) ? OBSTACULO_MURO : OBSTACULO_RIO;
            tab->grade[rx][ry] = obs;
            alocados++;
        }
    }
}

// 4. Função para desenhar a interface no terminal
void exibir_tabuleiro(Tabuleiro* tab) {
    // Teto do tabuleiro
    for (int j = 0; j < tab->colunas; j++) printf("==");
    printf("====\n");

    for (int i = 0; i < tab->linhas; i++) {
        printf("||"); // Borda esquerda
        for (int j = 0; j < tab->colunas; j++) {
            switch (tab->grade[i][j]) {
                case VAZIO:          printf(" ."); break;
                case BAMOR:          printf(" B"); break;
                case TUI:            printf(" T"); break;
                case PM:             printf(" P"); break;
                case OBSTACULO_MURO: printf(" X"); break;
                case OBSTACULO_RIO:  printf(" ~"); break;
            }
        }
        printf(" ||\n"); // Borda direita
    }

    // Chão do tabuleiro
    for (int j = 0; j < tab->colunas; j++) printf("==");
    printf("====\n");

    printf("\nLegenda:\n");
    printf("B = Bamor (12%%)\n");
    printf("T = TUI (12%%)\n");
    printf("P = Policia Militar (5%%)\n");
    printf("X = Muro (Obstaculo)\n");
    printf("~ = Rio (Obstaculo)\n");
}

// 5. Função para limpar a memória ao fim do jogo
void destruir_tabuleiro(Tabuleiro* tab) {
    if (tab == NULL) return;

    // 5.1 Limpa a Lista Encadeada
    Entidade* atual = tab->lista_entidades;
    while (atual != NULL) {
        Entidade* prox = atual->prox;
        free(atual);
        atual = prox;
    }

    // 5.2 Limpa a Matriz
    for (int i = 0; i < tab->linhas; i++) {
        free(tab->grade[i]);
    }
    free(tab->grade);

    // 5.3 Limpa a estrutura principal
    free(tab);
}