#include <stdio.h>
#include <stdlib.h>
#include "board.h"

/*
 * Aloca o tabuleiro como array de arrays (grid[linha][coluna])
 * uso calloc nas linhas porque ja inicializa com zero
 * assim entity=NULL e obstacle=NONE automaticamente
 */
Board* board_create(int rows, int cols) {
    Board* board = (Board*)malloc(sizeof(Board));
    if (!board) {
        fprintf(stderr, "Erro: falha ao alocar Board.\n");
        return NULL;
    }

    board->rows = rows;
    board->cols = cols;

    board->grid = (Cell**)malloc(rows * sizeof(Cell*));
    if (!board->grid) {
        fprintf(stderr, "Erro: falha ao alocar grid.\n");
        free(board);
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        board->grid[i] = (Cell*)calloc(cols, sizeof(Cell));
        if (!board->grid[i]) {
            fprintf(stderr, "Erro: falha ao alocar linha %d.\n", i);
            /* libera o que ja alocou antes de retornar NULL */
            for (int j = 0; j < i; j++) free(board->grid[j]);
            free(board->grid);
            free(board);
            return NULL;
        }
    }

    return board;
}

/* libera na ordem inversa da alocacao */
void board_destroy(Board* board) {
    if (!board) return;
    for (int i = 0; i < board->rows; i++) free(board->grid[i]);
    free(board->grid);
    free(board);
}

/* checa se a posicao esta dentro do mapa */
int board_is_valid(const Board* board, int row, int col) {
    return (row >= 0 && row < board->rows &&
            col >= 0 && col < board->cols);
}

/* retorna 1 se tem obstaculo ou ta fora do mapa */
int board_is_blocked(const Board* board, int row, int col) {
    if (!board_is_valid(board, row, col)) return 1;
    return (board->grid[row][col].obstacle != OBSTACLE_NONE);
}

/* retorna 1 se a celula esta completamente vazia */
int board_is_empty(const Board* board, int row, int col) {
    if (!board_is_valid(board, row, col)) return 0;
    const Cell* c = &board->grid[row][col];
    return (c->entity == NULL && c->obstacle == OBSTACLE_NONE);
}

void board_place_entity(Board* board, Entity* e) {
    if (!e || !board_is_valid(board, e->row, e->col)) return;
    board->grid[e->row][e->col].entity = e;
}

/* so remove se o ponteiro na celula for realmente essa entidade */
void board_remove_entity(Board* board, Entity* e) {
    if (!e || !board_is_valid(board, e->row, e->col)) return;
    if (board->grid[e->row][e->col].entity == e) {
        board->grid[e->row][e->col].entity = NULL;
    }
}

void board_move_entity(Board* board, Entity* e, int new_row, int new_col) {
    if (!e) return;
    board_remove_entity(board, e);
    e->row = new_row;
    e->col = new_col;
    board->grid[new_row][new_col].entity = e;
}

void board_place_obstacle(Board* board, int row, int col, ObstacleType type) {
    if (!board_is_valid(board, row, col)) return;
    board->grid[row][col].obstacle = type;
}

/*
 * Popula o mapa usando Fisher-Yates shuffle
 * crio um array com todos os indices lineares [0, rows*cols-1],
 * embaralho ele, e distribuo em ordem: obstaculos -> lobos -> ovelhas -> cacadores
 * assim nao tem sobreposicao e a distribuicao e uniforme
 *
 * o jeito "naive" de sortear posicoes com rand() tem colisoes (aniversario)
 * e fica lento quando o mapa enche - o Fisher-Yates e O(n) garantido
 */
void board_populate(Board* board, EntityList* list) {
    int total = board->rows * board->cols;

    int* pos = (int*)malloc(total * sizeof(int));
    if (!pos) {
        fprintf(stderr, "Erro: falha ao alocar array de posicoes.\n");
        return;
    }

    for (int i = 0; i < total; i++) pos[i] = i;

    /* Fisher-Yates shuffle */
    for (int i = total - 1; i > 0; i--) {
        int j   = rand() % (i + 1);
        int tmp = pos[i];
        pos[i]  = pos[j];
        pos[j]  = tmp;
    }

    int n_obs    = (total * PCT_OBSTACLE) / 100;
    int n_wolf   = (total * PCT_WOLF)     / 100;
    int n_sheep  = (total * PCT_SHEEP)    / 100;
    int n_hunter = (total * PCT_HUNTER)   / 100;

    int idx = 0;

    /* obstaculos (alterna arvore/pedra) */
    for (int i = 0; i < n_obs && idx < total; i++, idx++) {
        int r = pos[idx] / board->cols;
        int c = pos[idx] % board->cols;
        ObstacleType tipo = (i % 2 == 0) ? OBSTACLE_TREE : OBSTACLE_ROCK;
        board_place_obstacle(board, r, c, tipo);
    }

    /* lobos */
    for (int i = 0; i < n_wolf && idx < total; i++, idx++) {
        int r = pos[idx] / board->cols;
        int c = pos[idx] % board->cols;
        Entity* e = entity_create(list, FACTION_WOLF, r, c);
        if (e) board_place_entity(board, e);
    }

    /* ovelhas */
    for (int i = 0; i < n_sheep && idx < total; i++, idx++) {
        int r = pos[idx] / board->cols;
        int c = pos[idx] % board->cols;
        Entity* e = entity_create(list, FACTION_SHEEP, r, c);
        if (e) board_place_entity(board, e);
    }

    /* cacadores */
    for (int i = 0; i < n_hunter && idx < total; i++, idx++) {
        int r = pos[idx] / board->cols;
        int c = pos[idx] % board->cols;
        Entity* e = entity_create(list, FACTION_HUNTER, r, c);
        if (e) board_place_entity(board, e);
    }

    free(pos);
}
