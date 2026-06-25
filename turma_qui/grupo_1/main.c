#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* ============================================================
 * SECAO: CONSTANTES
 * ============================================================ */

#define MUNICAO_INICIAL_CACADOR 5
#define VELOCIDADE_MS           1500   /* delay entre rodadas (1.5 segundos) */

/* ============================================================
 * SECAO: ENTIDADE - tipos e prototipos
 * ============================================================ */

typedef enum {
    TIPO_OVELHA,
    TIPO_LOBO,
    TIPO_CACADOR
} TipoEntidade;

typedef struct {
    int x;
    int y;
    TipoEntidade tipo;
    int viva;
    int municao;
} Entidade;

Entidade criar_entidade(int x, int y, TipoEntidade tipo);
void matar_entidade(Entidade *e);
void decrementar_municao(Entidade *e);
int tem_municao(Entidade *e);

/* ============================================================
 * SECAO: TABULEIRO - tipos e prototipos
 * ============================================================ */

typedef enum {
    CELULA_VAZIA,
    CELULA_ARVORE,
    CELULA_CASA,
    CELULA_ENTIDADE
} TipoCelula;

typedef struct {
    TipoCelula tipo;
    int entidade_idx;
} Celula;

Celula **criar_tabuleiro(int N);
void destruir_tabuleiro(Celula **tab, int N);
int posicao_valida(int x, int y, int N);
int celula_vazia(Celula **tab, int x, int y);
void colocar_entidade(Celula **tab, int x, int y, int entidade_idx);
void colocar_obstaculo(Celula **tab, int x, int y, TipoCelula tipo);
void limpar_celula(Celula **tab, int x, int y);

/* ============================================================
 * SECAO: MUNDO - tipos e prototipos
 * ============================================================ */

typedef struct {
    Celula **tabuleiro;
    Entidade *entidades;
    int n_entidades;
    int capacidade;
    int N;
} Mundo;

Mundo *criar_mundo(int N);
void destruir_mundo(Mundo *m);
int adicionar_entidade(Mundo *m, int x, int y, TipoEntidade tipo);
void remover_entidade(Mundo *m, int idx);
void mover_entidade_no_mundo(Mundo *m, int idx, int novo_x, int novo_y);
void imprimir_mundo(Mundo *m);
void contar_entidades(Mundo *m, int *n_ovelhas, int *n_lobos, int *n_cacadores);

/* ============================================================
 * SECAO: SIMULACAO - tipos e prototipos
 * ============================================================ */

typedef enum {
    FIM_LIMITE_RODADAS,
    FIM_OVELHAS_EXTINTAS,
    FIM_LOBOS_EXTINTOS,
    FIM_CACADORES_EXTINTOS
} MotivoFim;

typedef struct {
    int total_rodadas;
    MotivoFim motivo_fim;
    int ovelhas_final;
    int lobos_final;
    int cacadores_final;
} ResultadoSimulacao;

void executar_rodada(Mundo *m);
void imprimir_resultado(ResultadoSimulacao r);

/* ============================================================
 * SECAO: ENTIDADE - implementacao
 * ============================================================ */

Entidade criar_entidade(int x, int y, TipoEntidade tipo) {
    Entidade e;
    e.x = x;
    e.y = y;
    e.tipo = tipo;
    e.viva = 1;
    e.municao = (tipo == TIPO_CACADOR) ? MUNICAO_INICIAL_CACADOR : 0;
    return e;
}

void matar_entidade(Entidade *e) {
    e->viva = 0;
}

void decrementar_municao(Entidade *e) {
    if (e->tipo == TIPO_CACADOR && e->municao > 0) {
        e->municao--;
    }
}

int tem_municao(Entidade *e) {
    return (e->tipo == TIPO_CACADOR && e->municao > 0);
}

/* ============================================================
 * SECAO: TABULEIRO - implementacao
 * ============================================================ */

Celula **criar_tabuleiro(int N) {
    Celula **tab = malloc(N * sizeof(Celula *));
    if (tab == NULL) return NULL;

    for (int i = 0; i < N; i++) {
        tab[i] = malloc(N * sizeof(Celula));
        if (tab[i] == NULL) {
            for (int j = 0; j < i; j++) free(tab[j]);
            free(tab);
            return NULL;
        }
        for (int j = 0; j < N; j++) {
            tab[i][j].tipo = CELULA_VAZIA;
            tab[i][j].entidade_idx = -1;
        }
    }
    return tab;
}

void destruir_tabuleiro(Celula **tab, int N) {
    if (tab == NULL) return;
    for (int i = 0; i < N; i++) free(tab[i]);
    free(tab);
}

int posicao_valida(int x, int y, int N) {
    return (x >= 0 && x < N && y >= 0 && y < N);
}

int celula_vazia(Celula **tab, int x, int y) {
    return tab[x][y].tipo == CELULA_VAZIA;
}

void colocar_entidade(Celula **tab, int x, int y, int entidade_idx) {
    tab[x][y].tipo = CELULA_ENTIDADE;
    tab[x][y].entidade_idx = entidade_idx;
}

void colocar_obstaculo(Celula **tab, int x, int y, TipoCelula tipo) {
    tab[x][y].tipo = tipo;
    tab[x][y].entidade_idx = -1;
}

void limpar_celula(Celula **tab, int x, int y) {
    tab[x][y].tipo = CELULA_VAZIA;
    tab[x][y].entidade_idx = -1;
}

/* ============================================================
 * SECAO: MUNDO - implementacao
 * ============================================================ */

static void sortear_posicao_vazia(Mundo *m, int *x, int *y) {
    do {
        *x = rand() % m->N;
        *y = rand() % m->N;
    } while (!celula_vazia(m->tabuleiro, *x, *y));
}

static void popular_obstaculos(Mundo *m, int quantidade, TipoCelula tipo) {
    for (int i = 0; i < quantidade; i++) {
        int x, y;
        sortear_posicao_vazia(m, &x, &y);
        colocar_obstaculo(m->tabuleiro, x, y, tipo);
    }
}

static void popular_entidades(Mundo *m, int quantidade, TipoEntidade tipo) {
    for (int i = 0; i < quantidade; i++) {
        int x, y;
        sortear_posicao_vazia(m, &x, &y);
        adicionar_entidade(m, x, y, tipo);
    }
}

Mundo *criar_mundo(int N) {
    Mundo *m = malloc(sizeof(Mundo));
    if (m == NULL) return NULL;

    m->N = N;
    m->n_entidades = 0;

    m->tabuleiro = criar_tabuleiro(N);
    if (m->tabuleiro == NULL) {
        free(m);
        return NULL;
    }

    /* Porcentagens REDUZIDAS */
    int n_ovelhas   = (int)(N * N * 0.08);
    int n_lobos     = (int)(N * N * 0.08);
    int n_cacadores = (int)(N * N * 0.03);
    int n_arvores   = (int)(N * N * 0.03);
    int n_casas     = (int)(N * N * 0.03);
    int n_iniciais  = n_ovelhas + n_lobos + n_cacadores;

    /* Garantir pelo menos 1 de cada tipo */
    if (n_ovelhas < 1) n_ovelhas = 1;
    if (n_lobos < 1) n_lobos = 1;
    if (n_cacadores < 1) n_cacadores = 1;

    m->capacidade = n_iniciais * 2;
    if (m->capacidade < 10) m->capacidade = 10;
    m->entidades = malloc(m->capacidade * sizeof(Entidade));
    if (m->entidades == NULL) {
        destruir_tabuleiro(m->tabuleiro, N);
        free(m);
        return NULL;
    }

    popular_obstaculos(m, n_arvores, CELULA_ARVORE);
    popular_obstaculos(m, n_casas, CELULA_CASA);

    popular_entidades(m, n_ovelhas, TIPO_OVELHA);
    popular_entidades(m, n_lobos, TIPO_LOBO);
    popular_entidades(m, n_cacadores, TIPO_CACADOR);

    return m;
}

void destruir_mundo(Mundo *m) {
    if (m == NULL) return;
    destruir_tabuleiro(m->tabuleiro, m->N);
    free(m->entidades);
    free(m);
}

int adicionar_entidade(Mundo *m, int x, int y, TipoEntidade tipo) {
    int idx = -1;

    for (int i = 0; i < m->n_entidades; i++) {
        if (m->entidades[i].viva == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        if (m->n_entidades >= m->capacidade) {
            int nova_capacidade = m->capacidade * 2;
            Entidade *novo = realloc(m->entidades, nova_capacidade * sizeof(Entidade));
            if (novo == NULL) return -1;
            m->entidades = novo;
            m->capacidade = nova_capacidade;
        }
        idx = m->n_entidades;
        m->n_entidades++;
    }

    m->entidades[idx] = criar_entidade(x, y, tipo);
    colocar_entidade(m->tabuleiro, x, y, idx);
    return idx;
}

void remover_entidade(Mundo *m, int idx) {
    if (idx < 0 || idx >= m->n_entidades) return;
    if (!m->entidades[idx].viva) return;

    Entidade *e = &m->entidades[idx];
    limpar_celula(m->tabuleiro, e->x, e->y);
    matar_entidade(e);
}

void mover_entidade_no_mundo(Mundo *m, int idx, int novo_x, int novo_y) {
    if (idx < 0 || idx >= m->n_entidades) return;
    if (!m->entidades[idx].viva) return;

    Entidade *e = &m->entidades[idx];
    limpar_celula(m->tabuleiro, e->x, e->y);
    e->x = novo_x;
    e->y = novo_y;
    colocar_entidade(m->tabuleiro, novo_x, novo_y, idx);
}

void imprimir_mundo(Mundo *m) {
    int N = m->N;
    int largura_tracos = 3 * N;

    printf("    ");
    for (int j = 0; j < N; j++) printf("%2d ", j);
    printf("\n");

    printf("   +");
    for (int i = 0; i < largura_tracos; i++) printf("-");
    printf("+\n");

    for (int i = 0; i < N; i++) {
        printf("%2d |", i);
        for (int j = 0; j < N; j++) {
            const char *cor = "";
            char c = '?';
            Celula *cel = &m->tabuleiro[i][j];

            switch (cel->tipo) {
                case CELULA_VAZIA:
                    c = '.';  cor = "\033[90m";       /* cinza */
                    break;
                case CELULA_ARVORE:
                    c = 'A';  cor = "\033[1;32m";     /* verde - ARVORE */
                    break;
                case CELULA_CASA:
                    c = 'H';  cor = "\033[1;33m";     /* amarelo - CASA */
                    break;
                case CELULA_ENTIDADE: {
                    Entidade *e = &m->entidades[cel->entidade_idx];
                    switch (e->tipo) {
                        case TIPO_OVELHA:  c = 'O';  cor = "\033[1;37m"; break; /* BRANCO */
                        case TIPO_LOBO:    c = 'L';  cor = "\033[1;31m"; break; /* VERMELHO */
                        case TIPO_CACADOR: c = 'C';  cor = "\033[1;36m"; break; /* CIANO */
                    }
                    break;
                }
            }
            printf(" %s%c\033[0m ", cor, c);
        }
        printf("|\n");
    }

    printf("   +");
    for (int i = 0; i < largura_tracos; i++) printf("-");
    printf("+\n");
}

void contar_entidades(Mundo *m, int *n_ovelhas, int *n_lobos, int *n_cacadores) {
    int ov = 0, lo = 0, ca = 0;

    for (int i = 0; i < m->n_entidades; i++) {
        if (!m->entidades[i].viva) continue;
        switch (m->entidades[i].tipo) {
            case TIPO_OVELHA:  ov++; break;
            case TIPO_LOBO:    lo++; break;
            case TIPO_CACADOR: ca++; break;
        }
    }

    if (n_ovelhas)   *n_ovelhas   = ov;
    if (n_lobos)     *n_lobos     = lo;
    if (n_cacadores) *n_cacadores = ca;
}

/* ============================================================
 * SECAO: SIMULACAO - implementacao
 * ============================================================ */

static const int DX[] = {-1, 1,  0, 0};
static const int DY[] = { 0, 0, -1, 1};

static int tem_adjacente_do_tipo(Celula **tab, int x, int y, int N, TipoCelula tipo) {
    for (int d = 0; d < 4; d++) {
        int nx = x + DX[d];
        int ny = y + DY[d];
        if (posicao_valida(nx, ny, N) && tab[nx][ny].tipo == tipo) {
            return 1;
        }
    }
    return 0;
}

/* ============================================================
 * CORRECAO: A arvore protege o lobo se estiver ADJACENTE a ele
 * (mesma logica da casa protegendo a ovelha)
 * ============================================================ */

static void agir_ovelha(Mundo *m, int idx) {
    Entidade *e = &m->entidades[idx];
    int dir = rand() % 4;
    int nx = e->x + DX[dir];
    int ny = e->y + DY[dir];

    if (!posicao_valida(nx, ny, m->N)) return;
    if (!celula_vazia(m->tabuleiro, nx, ny)) return;

    mover_entidade_no_mundo(m, idx, nx, ny);
}

static void agir_lobo(Mundo *m, int idx) {
    Entidade *e = &m->entidades[idx];
    int dir = rand() % 4;
    int nx = e->x + DX[dir];
    int ny = e->y + DY[dir];

    if (!posicao_valida(nx, ny, m->N)) return;

    Celula *destino = &m->tabuleiro[nx][ny];

    if (destino->tipo == CELULA_VAZIA) {
        mover_entidade_no_mundo(m, idx, nx, ny);
        return;
    }

    if (destino->tipo != CELULA_ENTIDADE) return;

    Entidade *alvo = &m->entidades[destino->entidade_idx];
    if (!alvo->viva) return;

    if (alvo->tipo == TIPO_OVELHA) {
        /* Casa protege a ovelha (adjacente a casa) */
        if (tem_adjacente_do_tipo(m->tabuleiro, alvo->x, alvo->y, m->N, CELULA_CASA)) {
            return;
        }
        int alvo_idx = destino->entidade_idx;
        remover_entidade(m, alvo_idx);
        mover_entidade_no_mundo(m, idx, nx, ny);
        return;
    }

    if (alvo->tipo == TIPO_CACADOR && !tem_municao(alvo)) {
        int alvo_idx = destino->entidade_idx;
        remover_entidade(m, alvo_idx);
        mover_entidade_no_mundo(m, idx, nx, ny);
        return;
    }
}

static void agir_cacador(Mundo *m, int idx) {
    Entidade *e = &m->entidades[idx];
    int dir = rand() % 4;
    int nx = e->x + DX[dir];
    int ny = e->y + DY[dir];

    if (!posicao_valida(nx, ny, m->N)) return;

    Celula *destino = &m->tabuleiro[nx][ny];

    if (destino->tipo == CELULA_VAZIA) {
        mover_entidade_no_mundo(m, idx, nx, ny);
        return;
    }

    if (destino->tipo != CELULA_ENTIDADE) return;

    Entidade *alvo = &m->entidades[destino->entidade_idx];
    if (!alvo->viva) return;

    if (alvo->tipo == TIPO_LOBO && tem_municao(e)) {
        /* ============================================================
         * CORRECAO: Arvore protege o lobo se estiver ADJACENTE a ele
         * (mesma logica da casa protegendo a ovelha)
         * ============================================================ */
        if (tem_adjacente_do_tipo(m->tabuleiro, alvo->x, alvo->y, m->N, CELULA_ARVORE)) {
            return;  /* Arvore protege o lobo */
        }
        
        int alvo_idx = destino->entidade_idx;
        remover_entidade(m, alvo_idx);
        decrementar_municao(e);
        mover_entidade_no_mundo(m, idx, nx, ny);
        return;
    }
}

static void agir_entidade(Mundo *m, int idx) {
    Entidade *e = &m->entidades[idx];
    if (!e->viva) return;

    switch (e->tipo) {
        case TIPO_OVELHA:  agir_ovelha(m, idx);  break;
        case TIPO_LOBO:    agir_lobo(m, idx);    break;
        case TIPO_CACADOR: agir_cacador(m, idx); break;
    }
}

/* ============================================================
 * Apenas UMA entidade se move por rodada
 * ============================================================ */

void executar_rodada(Mundo *m) {
    if (m == NULL) return;
    
    /* Contar quantas entidades vivas existem */
    int n_vivas = 0;
    for (int i = 0; i < m->n_entidades; i++) {
        if (m->entidades[i].viva) {
            n_vivas++;
        }
    }
    
    if (n_vivas == 0) return;
    
    /* Escolher UMA entidade aleatoria para agir */
    int alvo = rand() % n_vivas;
    int idx = -1;
    int contador = 0;
    
    for (int i = 0; i < m->n_entidades; i++) {
        if (m->entidades[i].viva) {
            if (contador == alvo) {
                idx = i;
                break;
            }
            contador++;
        }
    }
    
    if (idx != -1) {
        agir_entidade(m, idx);
    }
}

/* ============================================================
 * CORRECAO: Funcao executar_simulacao REMOVIDA (nao utilizada)
 * ============================================================ */

void imprimir_resultado(ResultadoSimulacao r) {
    printf("\n========================================\n");
    printf("           RESULTADO DA SIMULACAO\n");
    printf("========================================\n");
    printf("Rodadas executadas: %d\n", r.total_rodadas);
    printf("Motivo do fim:      ");

    switch (r.motivo_fim) {
        case FIM_LIMITE_RODADAS:      printf("Limite de rodadas atingido\n"); break;
        case FIM_OVELHAS_EXTINTAS:    printf("Ovelhas foram extintas\n"); break;
        case FIM_LOBOS_EXTINTOS:      printf("Lobos foram extintos\n"); break;
        case FIM_CACADORES_EXTINTOS:  printf("Cacadores foram extintos\n"); break;
    }

    printf("\nPopulacao final:\n");
    printf("  Ovelhas:   %d\n", r.ovelhas_final);
    printf("  Lobos:     %d\n", r.lobos_final);
    printf("  Cacadores: %d\n", r.cacadores_final);
    printf("========================================\n");
}

/* ============================================================
 * SECAO: MAIN - exibicao animada e ponto de entrada
 * ============================================================ */

static void limpar_tela(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

static void pausar_ms(int ms) {
    usleep((useconds_t)ms * 1000);
}

static void exibir_frame(Mundo *m, int rodada, int max_rodadas, int ov, int lo, int ca) {
    limpar_tela();
    printf("=== SIMULACAO ECOSSISTEMA ===\n");
    printf("Rodada: %d / %d\n", rodada, max_rodadas);
    printf("Ovelhas: %d | Lobos: %d | Cacadores: %d\n\n", ov, lo, ca);
    imprimir_mundo(m);
    fflush(stdout);
}

/* ============================================================
 * CORRECAO: Tratamento de erro no scanf com limpeza de buffer
 * ============================================================ */

int main(void) {
    int N, max_rodadas;

    printf("Tamanho do tabuleiro (minimo 5): ");
    if (scanf("%d", &N) != 1) {
        printf("Tamanho invalido. Usando 5.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        N = 5;
    } else if (N < 5) {
        printf("Tamanho invalido. Usando 5.\n");
        N = 5;
    }

    printf("Numero de rodadas: ");
    if (scanf("%d", &max_rodadas) != 1) {
        printf("Valor invalido. Usando 100.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        max_rodadas = 100;
    } else if (max_rodadas < 1) {
        printf("Valor invalido. Usando 100.\n");
        max_rodadas = 100;
    }

    srand((unsigned int)time(NULL));

    Mundo *m = criar_mundo(N);
    if (m == NULL) {
        printf("Erro: falha de alocacao de memoria.\n");
        return 1;
    }

    int ov, lo, ca;
    contar_entidades(m, &ov, &lo, &ca);
    exibir_frame(m, 0, max_rodadas, ov, lo, ca);
    pausar_ms(VELOCIDADE_MS);

    ResultadoSimulacao r;
    int finalizada = 0;

    for (int rodada = 1; rodada <= max_rodadas && !finalizada; rodada++) {
        executar_rodada(m);
        contar_entidades(m, &ov, &lo, &ca);

        exibir_frame(m, rodada, max_rodadas, ov, lo, ca);

        if (ov == 0 || lo == 0 || ca == 0) {
            r.total_rodadas = rodada;
            r.ovelhas_final = ov;
            r.lobos_final = lo;
            r.cacadores_final = ca;
            if (ov == 0)      r.motivo_fim = FIM_OVELHAS_EXTINTAS;
            else if (lo == 0) r.motivo_fim = FIM_LOBOS_EXTINTOS;
            else              r.motivo_fim = FIM_CACADORES_EXTINTOS;
            finalizada = 1;
        } else {
            pausar_ms(VELOCIDADE_MS);
        }
    }

    if (!finalizada) {
        r.total_rodadas = max_rodadas;
        r.motivo_fim = FIM_LIMITE_RODADAS;
        r.ovelhas_final = ov;
        r.lobos_final = lo;
        r.cacadores_final = ca;
    }

    pausar_ms(2000);
    imprimir_resultado(r);

    destruir_mundo(m);
    return 0;
}