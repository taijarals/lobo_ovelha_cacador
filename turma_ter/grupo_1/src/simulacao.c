#include <stdlib.h>
#include "../includes/simulacao.h"

// Função auxiliar para encontrar quem está na casa alvo
Entidade* buscar_entidade(Tabuleiro* tab, int x, int y) {
    Entidade* atual = tab->lista_entidades;
    while (atual != NULL) {
        if (atual->ativo && atual->x == x && atual->y == y) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

// O motor do jogo: processa um turno completo
void processar_rodada(Tabuleiro* tab) {
    Entidade* atual = tab->lista_entidades;

    while (atual != NULL) {
        // Se a entidade foi eliminada em um combate anterior, ela não joga
        if (atual->ativo == 0) {
            atual = atual->prox;
            continue;
        }

        // Sorteia a direção: 0=Cima, 1=Baixo, 2=Esquerda, 3=Direita
        int direcao = rand() % 4;
        int nx = atual->x;
        int ny = atual->y;

        if (direcao == 0) nx--;      // Cima
        else if (direcao == 1) nx++; // Baixo
        else if (direcao == 2) ny--; // Esquerda
        else if (direcao == 3) ny++; // Direita

        // 1. Regra de Limites: Não pode sair do mapa
        if (nx < 0 || nx >= tab->linhas || ny < 0 || ny >= tab->colunas) {
            atual = atual->prox;
            continue;
        }

        TipoEntidade alvo = tab->grade[nx][ny];

        // 2. Regra de Obstáculos: Bloqueiam movimentação
        if (alvo == OBSTACULO_MURO || alvo == OBSTACULO_RIO) {
            atual = atual->prox;
            continue; // Fica onde está
        }

        // 3. Regra de Movimento Livre
        if (alvo == VAZIO) {
            tab->grade[atual->x][atual->y] = VAZIO; // Libera a casa antiga
            atual->x = nx;
            atual->y = ny;
            tab->grade[nx][ny] = atual->tipo;       // Ocupa a casa nova
        } 
        // 4. Regras de Interação e Combate
        else {
            Entidade* defensor = buscar_entidade(tab, nx, ny);
            if (defensor == NULL) { atual = atual->prox; continue; }

            // Lógica de Predação/Combate
            if ((atual->tipo == BAMOR && defensor->tipo == TUI) ||
                (atual->tipo == TUI && defensor->tipo == BAMOR) ||
                (atual->tipo == PM && (defensor->tipo == BAMOR || defensor->tipo == TUI))) {
                
                // O atacante vence: elimina o alvo e toma a posição
                defensor->ativo = 0; 
                tab->grade[atual->x][atual->y] = VAZIO;
                atual->x = nx;
                atual->y = ny;
                tab->grade[nx][ny] = atual->tipo;

            } 
            // Lógica de Vulnerabilidade ("Foge da PM")
            else if ((atual->tipo == BAMOR || atual->tipo == TUI) && defensor->tipo == PM) {
                // Ao tentar esbarrar na PM, a torcida recua (movimento cancelado)
            } 
            // Lógica de Reprodução
            else if (atual->tipo == defensor->tipo && atual->tipo != PM) {
                // Mesma facção: Por enquanto, apenas esbarram amigavelmente. 
                // A regra de gerar uma nova entidade será ativada aqui depois!
            }
        }

        atual = atual->prox; // Passa a vez para o próximo
    }
}

// teste 