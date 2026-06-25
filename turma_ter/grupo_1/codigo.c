#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

// Estrutura dos personagens
typedef struct {
    int id, x, y, vivo;
    char simbolo;
} Entidade;

// Variaveis globais dinâmicas
char **tabuleiro;
int tamanho_tabuleiro;

// Quantidades baseadas em porcentagem
int num_ovelhas, num_lobos, num_cacadores, num_obstaculos;

// Ponteiros para os arrays dinamicos
Entidade *ovelhas;
Entidade *lobos;
Entidade *cacadores;

char msg_evento[150] = "Tudo tranquilo na floresta...";

// Checa se a coordenada tem algum personagem
int esta_ocupado(int x, int y) {
    for (int i = 0; i < num_ovelhas; i++) 
        if (ovelhas[i].vivo && ovelhas[i].x == x && ovelhas[i].y == y) return 1;
    for (int i = 0; i < num_lobos; i++) 
        if (lobos[i].vivo && lobos[i].x == x && lobos[i].y == y) return 1;
    for (int i = 0; i < num_cacadores; i++) 
        if (cacadores[i].vivo && cacadores[i].x == x && cacadores[i].y == y) return 1;
    return 0;
}

// Coloca um personagem em um espaco vazio aleatorio
void posicionar_entidade(Entidade *e) {
    while (1) {
        int x = rand() % tamanho_tabuleiro;
        int y = rand() % tamanho_tabuleiro;
        if (tabuleiro[y][x] == '.' && !esta_ocupado(x, y)) {
            e->x = x;
            e->y = y;
            break;
        }
    }
}

// Prepara o cenario dinamicamente baseado no tamanho
void iniciar_jogo() {
    int total_celulas = tamanho_tabuleiro * tamanho_tabuleiro;
    
    // Calcula proporcoes automaticamente (12%, 12%, 5%, 10%)
    num_ovelhas = (total_celulas * 12) / 100;
    num_lobos = (total_celulas * 12) / 100;
    num_cacadores = (total_celulas * 5) / 100;
    num_obstaculos = (total_celulas * 10) / 100;

    // Garante no minimo 1 de cada para mapas muito pequenos (5x5)
    if(num_ovelhas == 0) num_ovelhas = 1;
    if(num_lobos == 0) num_lobos = 1;
    if(num_cacadores == 0) num_cacadores = 1;
    if(num_obstaculos == 0) num_obstaculos = 1;

    // Aloca matriz do tabuleiro dinamicamente
    tabuleiro = (char **)malloc(tamanho_tabuleiro * sizeof(char *));
    for (int i = 0; i < tamanho_tabuleiro; i++) {
        tabuleiro[i] = (char *)malloc(tamanho_tabuleiro * sizeof(char));
        for (int j = 0; j < tamanho_tabuleiro; j++) {
            tabuleiro[i][j] = '.';
        }
    }

    // Aloca memoria inicial para os arrays de entidades
    ovelhas = (Entidade *)malloc(num_ovelhas * sizeof(Entidade));
    lobos = (Entidade *)malloc(num_lobos * sizeof(Entidade));
    cacadores = (Entidade *)malloc(num_cacadores * sizeof(Entidade));

    // Espalha obstaculos fixos pelo mapa
    char tipos_obstaculos[] = {'A', 'K', 'R'}; // Arvore, Casa, Pedra
    for (int i = 0; i < num_obstaculos; i++) {
        int x, y; do { x = rand() % tamanho_tabuleiro; y = rand() % tamanho_tabuleiro; } while (tabuleiro[y][x] != '.');
        tabuleiro[y][x] = tipos_obstaculos[rand() % 3];
    }

    // Inicializa e posiciona os personagens
    for (int i = 0; i < num_ovelhas; i++) { ovelhas[i] = (Entidade){i, 0, 0, 1, 'O'}; posicionar_entidade(&ovelhas[i]); }
    for (int i = 0; i < num_lobos; i++) { lobos[i] = (Entidade){i, 0, 0, 1, 'L'}; posicionar_entidade(&lobos[i]); }
    for (int i = 0; i < num_cacadores; i++) { cacadores[i] = (Entidade){i, 0, 0, 1, 'C'}; posicionar_entidade(&cacadores[i]); }
}

// Desenha o mapa na tela com emojis alinhados e espaçados
void imprimir_tabuleiro() {
    for (int i = 0; i < tamanho_tabuleiro; i++) {
        for (int j = 0; j < tamanho_tabuleiro; j++) {
            int impresso = 0;
            
            for (int k = 0; k < num_lobos; k++) if (lobos[k].vivo && lobos[k].x == j && lobos[k].y == i) { printf("🐺 "); impresso = 1; break; }
            if (!impresso) for (int k = 0; k < num_cacadores; k++) if (cacadores[k].vivo && cacadores[k].x == j && cacadores[k].y == i) { printf("🤠 "); impresso = 1; break; }
            if (!impresso) for (int k = 0; k < num_ovelhas; k++) if (ovelhas[k].vivo && ovelhas[k].x == j && ovelhas[k].y == i) { printf("🐑 "); impresso = 1; break; }
            
            // Desenha o cenario baseado nos caracteres da matriz
            if (!impresso) {
                if (tabuleiro[i][j] == '.') printf("🟩 "); // Fundo do mapa
                else if (tabuleiro[i][j] == 'A') printf("🌲 ");
                else if (tabuleiro[i][j] == 'K') printf("🏠 ");
                else if (tabuleiro[i][j] == 'R') printf("🪨 ");
            }
        }
        printf("\n");
    }
}

// Move personagem (Apenas Cima, Baixo, Esquerda, Direita)
void mover_entidade(Entidade *e) {
    if (!e->vivo) return;
    
    int dx = 0; 
    int dy = 0;
    int direcao = rand() % 4; 
    
    if (direcao == 0) dy = -1; // Cima
    else if (direcao == 1) dy = 1;  // Baixo
    else if (direcao == 2) dx = -1; // Esquerda
    else if (direcao == 3) dx = 1;  // Direita

    int nx = e->x + dx;
    int ny = e->y + dy;

    // Checa limites do mapa e se o destino nao eh um obstaculo e nao tem personagem
    if (nx >= 0 && nx < tamanho_tabuleiro && ny >= 0 && ny < tamanho_tabuleiro) {
        if (tabuleiro[ny][nx] == '.' && !esta_ocupado(nx, ny)) {
            e->x = nx;
            e->y = ny;
        }
    }
}

// Mecanica de reproducao dinâmica (Usa realloc)
void reproduzir_entidades() {
    int qtd_novas = 2; // Nascem 2 ovelhas
    ovelhas = (Entidade *)realloc(ovelhas, (num_ovelhas + qtd_novas) * sizeof(Entidade));
    for(int i = 0; i < qtd_novas; i++) {
        ovelhas[num_ovelhas] = (Entidade){num_ovelhas, 0, 0, 1, 'O'};
        posicionar_entidade(&ovelhas[num_ovelhas]);
        num_ovelhas++;
    }
    
    int qtd_novos_lobos = 1; // Nasce 1 lobo
    lobos = (Entidade *)realloc(lobos, (num_lobos + qtd_novos_lobos) * sizeof(Entidade));
    for(int i = 0; i < qtd_novos_lobos; i++) {
        lobos[num_lobos] = (Entidade){num_lobos, 0, 0, 1, 'L'};
        posicionar_entidade(&lobos[num_lobos]);
        num_lobos++;
    }
    sprintf(msg_evento, "Evolucao! Nascimentos: +%d Ovelhas, +%d Lobos.", qtd_novas, qtd_novos_lobos);
}

int obter_distancia(Entidade a, Entidade b) {
    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    return (dx > dy) ? dx : dy; // Retorna a maior distancia (Chebyshev)
}

// Regras de predacao e combate
void verificar_regras() {
    for (int w = 0; w < num_lobos; w++) {
        if (!lobos[w].vivo) continue;
        
        // Cacador mata lobo
        for (int h = 0; h < num_cacadores; h++) {
            if (cacadores[h].vivo && obter_distancia(lobos[w], cacadores[h]) <= 1) {
                lobos[w].vivo = 0;
                sprintf(msg_evento, "O Lobo [%d] foi abatido pelo Cacador!", w+1);
                break;
            }
        }
        if (!lobos[w].vivo) continue;
        
        // Lobo ataca Ovelha
        for (int s = 0; s < num_ovelhas; s++) {
            if (ovelhas[s].vivo && obter_distancia(lobos[w], ovelhas[s]) <= 1) {
                int protegido = 0;
                for (int h = 0; h < num_cacadores; h++) {
                    if (cacadores[h].vivo && obter_distancia(cacadores[h], ovelhas[s]) <= 2) {
                        protegido = 1; break;
                    }
                }
                if (protegido) {
                    lobos[w].vivo = 0;
                    sprintf(msg_evento, "Cacador protegeu a Ovelha e matou o Lobo [%d]!", w+1);
                } else {
                    ovelhas[s].vivo = 0;
                    sprintf(msg_evento, "A Ovelha [%d] foi devorada!", s+1);
                }
                break; 
            }
        }
    }
}

// Libera toda a memoria alocada
void liberar_memoria() {
    free(ovelhas);
    free(lobos);
    free(cacadores);
    for (int i = 0; i < tamanho_tabuleiro; i++) {
        free(tabuleiro[i]);
    }
    free(tabuleiro);
}

int main() {
    srand(time(NULL));

    printf("=== CONFIGURACAO DA FLORESTA ===\n");
    do {
        printf("Digite o tamanho do tabuleiro (minimo 5): ");
        scanf("%d", &tamanho_tabuleiro);
    } while (tamanho_tabuleiro < 5);
    
    getchar(); // Consome o Enter

    iniciar_jogo();

    int turno = 1;
    int fim_jogo = 0;

    while (!fim_jogo) {
        printf("\n\n=== 🌲 SIMULADOR DE FLORESTA (%dx%d) 🌲 ===\n", tamanho_tabuleiro, tamanho_tabuleiro);
        printf("Turno: %d\n\n", turno);

        imprimir_tabuleiro();

        int ovelhas_vivas = 0, lobos_vivos = 0, cacadores_vivos = 0;
        for(int i=0; i<num_ovelhas; i++) ovelhas_vivas += ovelhas[i].vivo;
        for(int i=0; i<num_lobos; i++) lobos_vivos += lobos[i].vivo;
        for(int i=0; i<num_cacadores; i++) cacadores_vivos += cacadores[i].vivo;

        printf("\nVivos -> 🐑 Ovelhas: %d | 🐺 Lobos: %d | 🤠 Cacadores: %d\n", ovelhas_vivas, lobos_vivos, cacadores_vivos);
        printf(">>> %s\n", msg_evento);
        sprintf(msg_evento, "O ciclo da vida continua..."); 

        if (ovelhas_vivas == 0) {
            printf("\nFIM DE JOGO: Todas as ovelhas foram devoradas. 🪦\n");
            fim_jogo = 1;
        } else if (lobos_vivos == 0) {
            printf("\nFIM DE JOGO: Todos os lobos foram eliminados. A paz reina! ✨\n");
            fim_jogo = 1;
        }

        if (!fim_jogo) {
            for(int i=0; i<num_ovelhas; i++) mover_entidade(&ovelhas[i]);
            for(int i=0; i<num_lobos; i++) mover_entidade(&lobos[i]);
            for(int i=0; i<num_cacadores; i++) mover_entidade(&cacadores[i]);
            
            verificar_regras();

            if(turno % 15 == 0) reproduzir_entidades();
            
            turno++;
            
            printf("\n[Pressione ENTER para o proximo turno...]");
            getchar(); 
            
            if(turno > 300) {
                printf("\nFIM DE JOGO: Limite de turnos atingido (Empate).\n");
                fim_jogo = 1;
            }
        }
    }

    liberar_memoria();
    return 0;
}
