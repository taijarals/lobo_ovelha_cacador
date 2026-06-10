/*
 * Simulação de Ecossistema: Ovelha, Lobo e Caçador
 * Projeto Final - Estrutura de Dados em C
 *
 * Estruturas utilizadas:
 * - Matriz dinâmica (alocação com malloc) para o tabuleiro
 * - Structs para representar cada entidade
 * - Vetores dinâmicos para listas de entidades
 *
 * Interações:
 * - Lobo elimina Ovelha (predação)
 * - Caçador elimina Lobo (proteção)
 * - Ovelha foge do Lobo (vulnerabilidade)
 * - Entidades iguais próximas se reproduzem
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define dormir(ms) Sleep(ms)
#else
#include <unistd.h>
#define dormir(ms) usleep((ms) * 1000)
#endif

/* ========== LIMPAR TELA ========== */
void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ========== CONSTANTES ========== */
#define VAZIO     0
#define OVELHA    1
#define LOBO      2
#define CACADOR   3
#define ARVORE    4
#define PEDRA     5

#define PERC_OVELHA   0.12
#define PERC_LOBO     0.12
#define PERC_CACADOR  0.05
#define PERC_OBSTACULO 0.10

#define RODADAS_REPRODUCAO 3  /* a cada N rodadas, entidades se reproduzem */

/* ========== STRUCTS ========== */
typedef struct {
    int linha;
    int coluna;
    int tipo;       /* OVELHA, LOBO, CACADOR */
    int vivo;       /* 1 = vivo, 0 = morto */
    int rodadas;    /* contador de rodadas para reprodução */
} Entidade;

typedef struct {
    int **celulas;   /* tabuleiro: guarda o tipo de cada célula */
    int linhas;
    int colunas;
} Tabuleiro;

/* ========== VARIÁVEIS GLOBAIS ========== */
Tabuleiro tabuleiro;
Entidade *entidades;
int total_entidades;
int capacidade_entidades;

/* ========== PROTÓTIPOS ========== */
void inicializar_tabuleiro(int linhas, int colunas);
void liberar_tabuleiro();
void posicionar_obstaculos(int qtd);
void posicionar_entidade(int tipo, int qtd);
void adicionar_entidade(int linha, int coluna, int tipo);
void mover_entidades();
void mover_entidade(int idx);
void resolver_interacoes();
void reproduzir();
void imprimir_tabuleiro();
int celula_livre(int l, int c);
int contar_vivos(int tipo);
int verificar_fim();

/* ========== FUNÇÕES DE TABULEIRO ========== */

void inicializar_tabuleiro(int linhas, int colunas) {
    tabuleiro.linhas = linhas;
    tabuleiro.colunas = colunas;

    tabuleiro.celulas = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        tabuleiro.celulas[i] = (int *)calloc(colunas, sizeof(int));
    }
}

void liberar_tabuleiro() {
    for (int i = 0; i < tabuleiro.linhas; i++) {
        free(tabuleiro.celulas[i]);
    }
    free(tabuleiro.celulas);
}

int celula_livre(int l, int c) {
    if (l < 0 || l >= tabuleiro.linhas) return 0;
    if (c < 0 || c >= tabuleiro.colunas) return 0;
    return tabuleiro.celulas[l][c] == VAZIO;
}

void posicionar_obstaculos(int qtd) {
    int colocados = 0;
    while (colocados < qtd) {
        int l = rand() % tabuleiro.linhas;
        int c = rand() % tabuleiro.colunas;
        if (tabuleiro.celulas[l][c] == VAZIO) {
            /* alterna entre ARVORE e PEDRA */
            tabuleiro.celulas[l][c] = (colocados % 2 == 0) ? ARVORE : PEDRA;
            colocados++;
        }
    }
}

/* ========== FUNÇÕES DE ENTIDADES ========== */

void adicionar_entidade(int linha, int coluna, int tipo) {
    if (total_entidades >= capacidade_entidades) {
        capacidade_entidades *= 2;
        entidades = (Entidade *)realloc(entidades, capacidade_entidades * sizeof(Entidade));
    }
    entidades[total_entidades].linha   = linha;
    entidades[total_entidades].coluna  = coluna;
    entidades[total_entidades].tipo    = tipo;
    entidades[total_entidades].vivo    = 1;
    entidades[total_entidades].rodadas = 0;
    total_entidades++;
}

void posicionar_entidade(int tipo, int qtd) {
    int colocados = 0;
    while (colocados < qtd) {
        int l = rand() % tabuleiro.linhas;
        int c = rand() % tabuleiro.colunas;
        /* garante que nao nasce sobre obstaculos nem sobre outra entidade */
        if (tabuleiro.celulas[l][c] == VAZIO) {
            tabuleiro.celulas[l][c] = tipo;
            adicionar_entidade(l, c, tipo);
            colocados++;
        }
    }
}

/* ========== MOVIMENTAÇÃO ========== */

void mover_entidade(int idx) {
    if (!entidades[idx].vivo) return;

    int l = entidades[idx].linha;
    int c = entidades[idx].coluna;

    /* direções: cima, baixo, esquerda, direita */
    int dl[] = {-1, 1,  0, 0};
    int dc[] = { 0, 0, -1, 1};

    /* embaralha direções para movimento aleatório */
    int ordem[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = ordem[i]; ordem[i] = ordem[j]; ordem[j] = tmp;
    }

    for (int i = 0; i < 4; i++) {
        int nl = l + dl[ordem[i]];
        int nc = c + dc[ordem[i]];

        if (nl < 0 || nl >= tabuleiro.linhas)  continue;
        if (nc < 0 || nc >= tabuleiro.colunas) continue;

        int destino = tabuleiro.celulas[nl][nc];

        /* não atravessa obstáculos */
        if (destino == ARVORE || destino == PEDRA) continue;

        /* move para célula vazia ou ocupada por inimigo (interação depois) */
        tabuleiro.celulas[l][c] = VAZIO;
        entidades[idx].linha = nl;
        entidades[idx].coluna = nc;
        tabuleiro.celulas[nl][nc] = entidades[idx].tipo;
        entidades[idx].rodadas++;
        break;
    }
}

void mover_entidades() {
    for (int i = 0; i < total_entidades; i++) {
        mover_entidade(i);
    }
}

/* ========== INTERAÇÕES ========== */

void resolver_interacoes() {
    for (int i = 0; i < total_entidades; i++) {
        if (!entidades[i].vivo) continue;

        int l = entidades[i].linha;
        int c = entidades[i].coluna;

        for (int j = 0; j < total_entidades; j++) {
            if (i == j) continue;
            if (!entidades[j].vivo) continue;
            if (entidades[j].linha != l || entidades[j].coluna != c) continue;

            int ti = entidades[i].tipo;
            int tj = entidades[j].tipo;

            /* Lobo elimina Ovelha */
            if (ti == LOBO && tj == OVELHA) {
                entidades[j].vivo = 0;
                tabuleiro.celulas[l][c] = LOBO;
            }
            /* Caçador elimina Lobo */
            else if (ti == CACADOR && tj == LOBO) {
                entidades[j].vivo = 0;
                tabuleiro.celulas[l][c] = CACADOR;
            }
            /* Ovelha foge: se Lobo está na mesma célula, ovelha morre */
            else if (ti == OVELHA && tj == LOBO) {
                entidades[i].vivo = 0;
                tabuleiro.celulas[l][c] = LOBO;
            }
        }
    }
}

/* ========== REPRODUÇÃO ========== */

void reproduzir() {
    int novos_l[256], novos_c[256], novos_t[256];
    int total_novos = 0;

    int dl[] = {-1, 1,  0, 0};
    int dc[] = { 0, 0, -1, 1};

    for (int i = 0; i < total_entidades; i++) {
        if (!entidades[i].vivo) continue;
        if (entidades[i].rodadas < RODADAS_REPRODUCAO) continue;

        int l = entidades[i].linha;
        int c = entidades[i].coluna;
        int ti = entidades[i].tipo;

        /* verifica vizinho do mesmo tipo */
        for (int d = 0; d < 4; d++) {
            int nl = l + dl[d];
            int nc = c + dc[d];
            if (nl < 0 || nl >= tabuleiro.linhas)  continue;
            if (nc < 0 || nc >= tabuleiro.colunas) continue;
            if (tabuleiro.celulas[nl][nc] != ti) continue;

            /* procura célula vazia ao redor para nascer */
            for (int d2 = 0; d2 < 4; d2++) {
                int bl = l + dl[d2];
                int bc = c + dc[d2];
                if (celula_livre(bl, bc)) {
                    if (total_novos < 256) {
                        novos_l[total_novos] = bl;
                        novos_c[total_novos] = bc;
                        novos_t[total_novos] = ti;
                        total_novos++;
                        entidades[i].rodadas = 0;
                    }
                    break;
                }
            }
            break;
        }
    }

    for (int i = 0; i < total_novos; i++) {
        tabuleiro.celulas[novos_l[i]][novos_c[i]] = novos_t[i];
        adicionar_entidade(novos_l[i], novos_c[i], novos_t[i]);
    }
}

/* ========== IMPRESSÃO ========== */

void imprimir_tabuleiro() {
    printf("\n");
    for (int i = 0; i < tabuleiro.linhas; i++) {
        for (int j = 0; j < tabuleiro.colunas; j++) {
            switch (tabuleiro.celulas[i][j]) {
                case VAZIO:   printf(". "); break;
                case OVELHA:  printf("O "); break;
                case LOBO:    printf("L "); break;
                case CACADOR: printf("C "); break;
                case ARVORE:  printf("A "); break;
                case PEDRA:   printf("P "); break;
                default:      printf("? "); break;
            }
        }
        printf("\n");
    }
    printf("Ovelhas: %d | Lobos: %d | Cacadores: %d\n",
        contar_vivos(OVELHA), contar_vivos(LOBO), contar_vivos(CACADOR));
}

/* ========== CONTAGEM E FIM DE JOGO ========== */

int contar_vivos(int tipo) {
    int count = 0;
    for (int i = 0; i < total_entidades; i++) {
        if (entidades[i].vivo && entidades[i].tipo == tipo) count++;
    }
    return count;
}

int verificar_fim() {
    int ovelhas  = contar_vivos(OVELHA);
    int lobos    = contar_vivos(LOBO);
    int cacadores = contar_vivos(CACADOR);

    if (ovelhas == 0 && lobos == 0) {
        printf("\n=== FIM: Apenas cacadores sobreviveram! ===\n");
        return 1;
    }
    if (lobos == 0 && cacadores == 0) {
        printf("\n=== FIM: Apenas ovelhas sobreviveram! ===\n");
        return 1;
    }
    if (ovelhas == 0 && cacadores == 0) {
        printf("\n=== FIM: Apenas lobos sobreviveram! ===\n");
        return 1;
    }
    if (ovelhas == 0) {
        printf("\n=== FIM: Todas as ovelhas foram eliminadas! ===\n");
        return 1;
    }
    return 0;
}

/* ========== MAIN ========== */

int main() {
    srand((unsigned int)time(NULL));

    int linhas, colunas;

    printf("=== Simulacao de Ecossistema: Ovelha, Lobo e Cacador ===\n");
    printf("Digite o numero de linhas do tabuleiro (minimo 5): ");
    scanf("%d", &linhas);
    printf("Digite o numero de colunas do tabuleiro (minimo 5): ");
    scanf("%d", &colunas);

    if (linhas < 5)  linhas = 5;
    if (colunas < 5) colunas = 5;

    int total_celulas = linhas * colunas;

    int qtd_ovelha   = (int)(total_celulas * PERC_OVELHA);
    int qtd_lobo     = (int)(total_celulas * PERC_LOBO);
    int qtd_cacador  = (int)(total_celulas * PERC_CACADOR);
    int qtd_obstaculo = (int)(total_celulas * PERC_OBSTACULO);

    /* inicializa tabuleiro e entidades */
    inicializar_tabuleiro(linhas, colunas);

    capacidade_entidades = qtd_ovelha + qtd_lobo + qtd_cacador + 64;
    entidades = (Entidade *)malloc(capacidade_entidades * sizeof(Entidade));
    total_entidades = 0;

    /* posiciona obstáculos primeiro, depois entidades */
    posicionar_obstaculos(qtd_obstaculo);
    posicionar_entidade(OVELHA,  qtd_ovelha);
    posicionar_entidade(LOBO,    qtd_lobo);
    posicionar_entidade(CACADOR, qtd_cacador);

    printf("\nTabuleiro inicial:\n");
    printf("Legenda: O=Ovelha  L=Lobo  C=Cacador  A=Arvore  P=Pedra  .=Vazio\n");
    imprimir_tabuleiro();

    int rodada = 1;
    int max_rodadas = 1000;

    while (rodada <= max_rodadas) {
        printf("\n--- Rodada %d ---", rodada);

        mover_entidades();
        resolver_interacoes();
        reproduzir();

        limpar_tela();

        /* remove mortos do tabuleiro */
        for (int i = 0; i < total_entidades; i++) {
            if (!entidades[i].vivo) {
                int l = entidades[i].linha;
                int c = entidades[i].coluna;
                if (tabuleiro.celulas[l][c] == entidades[i].tipo) {
                    tabuleiro.celulas[l][c] = VAZIO;
                }
            }
        }

        imprimir_tabuleiro();

        if (verificar_fim()) break;

        rodada++;

        /* aguarda 5 segundos antes da próxima rodada */
        dormir(5000);
    }

    if (rodada > max_rodadas) {
        printf("\n=== Simulacao encerrada apos %d rodadas ===\n", max_rodadas);
    }

    /* libera memória */
    liberar_tabuleiro();
    free(entidades);

    return 0;
}
