#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definições dos tipos no mapa
#define VAZIO 0
#define REBELDE 1       // 12% - Presa
#define STORMTROOPER 2  // 12% - Predador
#define JEDI 3          // 5% - Protetor
#define OBS_ESCUDO 4    // Obstáculo fixo
#define OBS_SUCATA 5    // Obstáculo fixo

// Estrutura para representar cada personagem
typedef struct {
    int id;
    int tipo;
    int x;
    int y;
    int vivo; // 1 = vivo, 0 = morto
} Entidade;

// Estrutura principal da simulação
typedef struct {
    int tamanho;
    int rodada_atual;
    int max_rodadas;
    int **mapa;
    Entidade *entidades;
    int qtd_entidades;
    int max_entidades;
} Simulacao;

// ==========================================
// FUNÇÕES DE GERENCIAMENTO DE MEMÓRIA
// ==========================================

// Aloca a matriz do tabuleiro dinamicamente
int** alocarMapa(int tamanho) {
    int **mapa = (int**) malloc(tamanho * sizeof(int*));
    for (int i = 0; i < tamanho; i++) {
        mapa[i] = (int*) calloc(tamanho, sizeof(int)); // calloc já zera (VAZIO)
    }
    return mapa;
}

void liberarMemoria(Simulacao *sim) {
    for (int i = 0; i < sim->tamanho; i++) {
        free(sim->mapa[i]);
    }
    free(sim->mapa);
    free(sim->entidades);
    free(sim);
}

// ==========================================
// INICIALIZAÇÃO DO JOGO
// ==========================================

// Retorna uma posição aleatória vazia
void pegarPosicaoVazia(Simulacao *sim, int *x, int *y) {
    do {
        *x = rand() % sim->tamanho;
        *y = rand() % sim->tamanho;
    } while (sim->mapa[*y][*x] != VAZIO);
}

void adicionarEntidade(Simulacao *sim, int tipo) {
    if (sim->qtd_entidades >= sim->max_entidades) return;
    
    int x, y;
    pegarPosicaoVazia(sim, &x, &y);
    
    sim->mapa[y][x] = tipo;
    
    if (tipo != OBS_ESCUDO && tipo != OBS_SUCATA) {
        sim->entidades[sim->qtd_entidades].id = sim->qtd_entidades;
        sim->entidades[sim->qtd_entidades].tipo = tipo;
        sim->entidades[sim->qtd_entidades].x = x;
        sim->entidades[sim->qtd_entidades].y = y;
        sim->entidades[sim->qtd_entidades].vivo = 1;
        sim->qtd_entidades++;
    }
}

Simulacao* inicializarSimulacao(int tamanho, int max_rodadas) {
    Simulacao *sim = (Simulacao*) malloc(sizeof(Simulacao));
    sim->tamanho = tamanho;
    sim->rodada_atual = 0;
    sim->max_rodadas = max_rodadas;
    sim->mapa = alocarMapa(tamanho);
    
    // Capacidade máxima é o tabuleiro inteiro cheio
    sim->max_entidades = tamanho * tamanho;
    sim->entidades = (Entidade*) malloc(sim->max_entidades * sizeof(Entidade));
    sim->qtd_entidades = 0;

    // Cálculo das porcentagens exigidas
    int area = tamanho * tamanho;
    int qtd_reb = (area * 12) / 100;
    int qtd_storm = (area * 12) / 100;
    int qtd_jedi = (area * 5) / 100;
    int qtd_obs = (area * 10) / 100;
    
    // Garante pelo menos 1 de cada se o mapa for muito pequeno
    if (qtd_reb == 0) qtd_reb = 1;
    if (qtd_storm == 0) qtd_storm = 1;
    if (qtd_jedi == 0) qtd_jedi = 1;
    if (qtd_obs < 2) qtd_obs = 2; // Pelo menos dois tipos de obstáculos

    // Posicionando Obstáculos Fixos (Metade Escudo, Metade Sucata)
    for (int i = 0; i < qtd_obs; i++) {
        adicionarEntidade(sim, (i % 2 == 0) ? OBS_ESCUDO : OBS_SUCATA);
    }
    
    // Posicionando Personagens
    for (int i = 0; i < qtd_reb; i++) adicionarEntidade(sim, REBELDE);
    for (int i = 0; i < qtd_storm; i++) adicionarEntidade(sim, STORMTROOPER);
    for (int i = 0; i < qtd_jedi; i++) adicionarEntidade(sim, JEDI);

    return sim;
}

// ==========================================
// LÓGICA E REGRAS DE INTERAÇÃO
// ==========================================

void processarTurno(Simulacao *sim) {
    int dx[] = {0, 0, 1, -1}; // Movimentos em X (Baixo, Cima, Direita, Esquerda)
    int dy[] = {1, -1, 0, 0}; // Movimentos em Y
    
    for (int i = 0; i < sim->qtd_entidades; i++) {
        Entidade *e = &sim->entidades[i];
        if (!e->vivo) continue;

        // 1. Interações (Predação, Proteção e Reprodução)
        for (int dir = 0; dir < 4; dir++) {
            int nx = e->x + dx[dir];
            int ny = e->y + dy[dir];
            
            // Verifica limites do mapa
            if (nx >= 0 && nx < sim->tamanho && ny >= 0 && ny < sim->tamanho) {
                int alvo = sim->mapa[ny][nx];
                
                // Stormtrooper ataca Rebelde
                if (e->tipo == STORMTROOPER && alvo == REBELDE) {
                    for (int j = 0; j < sim->qtd_entidades; j++) {
                        if (sim->entidades[j].x == nx && sim->entidades[j].y == ny && sim->entidades[j].vivo) {
                            sim->entidades[j].vivo = 0; // Mata o rebelde
                            sim->mapa[ny][nx] = VAZIO;
                            break;
                        }
                    }
                }
                
                // Jedi ataca Stormtrooper
                if (e->tipo == JEDI && alvo == STORMTROOPER) {
                    for (int j = 0; j < sim->qtd_entidades; j++) {
                        if (sim->entidades[j].x == nx && sim->entidades[j].y == ny && sim->entidades[j].vivo) {
                            sim->entidades[j].vivo = 0; // Mata o stormtrooper
                            sim->mapa[ny][nx] = VAZIO;
                            break;
                        }
                    }
                }
                
                // Rebelde reproduz (Evolução) - 20% de chance ao encontrar outro rebelde
                if (e->tipo == REBELDE && alvo == REBELDE && (rand() % 100 < 20)) {
                    adicionarEntidade(sim, REBELDE);
                }
            }
        }

        // 2. Movimentação Aleatória
        int dir = rand() % 4;
        int nx = e->x + dx[dir];
        int ny = e->y + dy[dir];
        
        // Verifica se pode andar (dentro do mapa e casa vazia)
        if (nx >= 0 && nx < sim->tamanho && ny >= 0 && ny < sim->tamanho) {
            if (sim->mapa[ny][nx] == VAZIO) {
                sim->mapa[e->y][e->x] = VAZIO; // Libera casa antiga
                e->x = nx;
                e->y = ny;
                sim->mapa[ny][nx] = e->tipo;   // Ocupa casa nova
            }
        }
    }
}

int checarVitoria(Simulacao *sim) {
    int vivos_reb = 0;
    int vivos_storm = 0;
    
    for (int i = 0; i < sim->qtd_entidades; i++) {
        if (sim->entidades[i].vivo) {
            if (sim->entidades[i].tipo == REBELDE) vivos_reb++;
            if (sim->entidades[i].tipo == STORMTROOPER) vivos_storm++;
        }
    }
    
    if (vivos_reb == 0) {
        printf("\n=> O IMPERIO VENCEU! Todos os rebeldes foram eliminados.\n");
        return 1;
    }
    if (vivos_storm == 0) {
        printf("\n=> A REBELIAO VENCEU! A galaxia esta a salvo.\n");
        return 1;
    }
    return 0; // Jogo continua
}

// ==========================================
// INTERFACE E IMPRESSÃO
// ==========================================

void imprimirTabuleiro(Simulacao *sim) {
    printf("\n");
    for (int y = 0; y < sim->tamanho; y++) {
        for (int x = 0; x < sim->tamanho; x++) {
            switch(sim->mapa[y][x]) {
                case VAZIO: printf(". "); break;
                case REBELDE: printf("R "); break;       // R = Rebelde
                case STORMTROOPER: printf("S "); break;  // S = Stormtrooper
                case JEDI: printf("J "); break;          // J = Jedi
                case OBS_ESCUDO: printf("O "); break;    // O = Gerador de Escudo
                case OBS_SUCATA: printf("X "); break;    // X = Sucata
            }
        }
        printf("\n");
    }
    printf("\n");
}

// ==========================================
// FUNÇÃO PRINCIPAL
// ==========================================

int main() {
    srand(time(NULL));
    int tamanho, max_rodadas;

    printf("=== SIMULADOR DE ECOSSISTEMA: STAR WARS ===\n");
    printf("Digite o tamanho do tabuleiro (minimo 5): ");
    scanf("%d", &tamanho);
    if (tamanho < 5) tamanho = 5;

    printf("Digite o limite de rodadas: ");
    scanf("%d", &max_rodadas);

    Simulacao *sim = inicializarSimulacao(tamanho, max_rodadas);

    printf("\n[ LEGENDA: R=Rebelde | S=Stormtrooper | J=Jedi | O=Escudo | X=Sucata ]\n");
    printf("Posicao Inicial:");
    imprimirTabuleiro(sim);

    while (sim->rodada_atual < sim->max_rodadas) {
        sim->rodada_atual++;
        processarTurno(sim);
        
        printf("--- RODADA %d ---", sim->rodada_atual);
        imprimirTabuleiro(sim);
        
        if (checarVitoria(sim)) {
            break;
        }
    }

    if (sim->rodada_atual >= sim->max_rodadas) {
        printf("\n=> EMPATE! O limite de rodadas foi atingido.\n");
    }

    // Liberação obrigatória para análise de memória
    liberarMemoria(sim);

    return 0;
}