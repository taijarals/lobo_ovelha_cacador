#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Enum para identificar o que está em cada casa do tabuleiro
typedef enum {
    VAZIO = 0,
    BAMOR,
    TUI,
    PM,
    OBSTACULO_MURO,
    OBSTACULO_RIO
} TipoEntidade;

// Estrutura para cada Entidade (Personagem da simulação)
typedef struct Entidade {
    int id;
    TipoEntidade tipo;
    int x, y;
    int ativo;             // 1 para vivo, 0 para morto
    struct Entidade* prox; // Ponteiro para o próximo (Lista Encadeada)
} Entidade;

// Estrutura do Tabuleiro (Gerenciador do ecossistema)
typedef struct {
    int linhas;
    int colunas;
    TipoEntidade** grade;      // Matriz dinâmica para checagem rápida de colisões
    Entidade* lista_entidades; // Cabeça da lista encadeada de personagens ativos
} Tabuleiro;

#endif // ESTRUTURAS_H