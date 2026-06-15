
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


               /* CONSTANTES DOS TIMES */
#define MIN_TAMANHO      5
#define MAX_RODADAS      1000
#define PORC_TIME1      0.12f
#define PORC_TIME2      0.12f
#define PORC_TIME3      0.05f
#define PORC_OBSTACULO   0.10f
#define ENERGIA_INICIAL  100
#define ENERGIA_MAXIMA   150
#define ENERGIA_PREDACAO 20
#define CUSTO_MOVIMENTO  1
#define COOLDOWN_REPRO   3
#define IDADE_MAX_REPRO  10
#define IDADE_SENIL      15




                    /*   TAMANHO MASIMO DE LOG DE EVENTOS POR RODADA */
#define MAX_EVENTOS      256
#define TAM_EVENTO       128




/* ENUMERAÇÃO                    ENUMS  */
typedef enum { TIME_OVELHA=0, TIME_LOBO=1, TIME_CACADOR=2 } TipoTIME;
typedef enum { OBST_ARVORE=0,  OBST_PEDRA=1  }                  TipoObstaculo;
typedef enum { CELULA_VAZIA=0, CELULA_ENTIDADE=1, CELULA_OBSTACULO=2 } EstadoCelula;





/* ESTRUTURA                             STRUCTS            */
typedef struct Obstaculo {
    TipoObstaculo tipo;
    int x, y;
} Obstaculo;

typedef struct Entidade {
    TipoTIME tipo;
    int x, y;
    int energia;
    int idade;
    int cooldownReproducao;
    int morreuPorPredacao;
    int id;           /* IDENTIFICADOR UNICO PARA LOG */
    struct Entidade* proximo;
} Entidade;

                    /* LOG DE EVENTOS DE UMA RODADA */
typedef struct {
    char linhas[MAX_EVENTOS][TAM_EVENTO];
    int  total;
} LogRodada;



                            /*  MOÇÕES ACUMULADAS DA PARTIDA */
typedef struct {
    int mortesPorFome[3];       /*  MORREU POR ENERGIA ESGOTADA */

    int mortesPorPredacao[3];   /*  FOI ELIMINADO POR PREDAÇÃO */

    int nascimentos[3];         /*   FILHO GERADOP       */

    int predacoesLobo;          /*  TOTAL DE OVELHAS COMIDAS    */

    int eliminacoesCacador;     /*  TOTAL DE LOBOS ELIMINADOS  */

    int protecoes;              /*  TOTAL DE PROTEÇÕES  */

    int movimentos;             /*    TOTAL DE MOVIMENTOS    */

    int rodadasComReproducao;   /*   RODADAS EM QUE HOUVE FILHOTES */

    int pico[3];                /*  POPULAÇÃO MAXIMA DE CADA TIME */
} Estatisticas;

typedef struct {
    int largura, altura;
    EstadoCelula** estado;
    void***        conteudo;
    Entidade*      listaEntidades;
    int            contagemPorTipo[3];
    int            totalObstaculos;
    int            rodadaAtual;
    int            proximoId;
    LogRodada      log;
    Estatisticas   stats;
} Tabuleiro;

                            /*  IDEIAS   */
Tabuleiro* criarTabuleiro(int N);
void       posicionarAleatorio(Tabuleiro* t);
void       adicionarEntidade(Tabuleiro* t, TipoTIME tipo, int x, int y);
void       adicionarObstaculo(Tabuleiro* t, TipoObstaculo tipo, int x, int y);
int        sortearPosicaoLivre(int* ocupado, int total);
int        posicaoValida(Tabuleiro* t, int x, int y);

void movimentarTodos(Tabuleiro* t);
int  tentarMover(Tabuleiro* t, Entidade* e, int dx, int dy);

void processarInteracoes(Tabuleiro* t);
void predacaoLoboOvelha(Tabuleiro* t, Entidade* lobo, Entidade* ovelha);
void predacaoCacadorLobo(Tabuleiro* t, Entidade* cacador, Entidade* lobo);
void protecaoCacadorOvelha(Tabuleiro* t, Entidade* cacador, Entidade* ovelha);

void processarReproducao(Tabuleiro* t);
int  reproduzir(Tabuleiro* t, Entidade* a, Entidade* b);
int  encontrarPosicaoLivreVizinha(Tabuleiro* t, int x, int y, int* nx, int* ny);
void removerEntidadesMortas(Tabuleiro* t);
void avancarIdade(Tabuleiro* t);

void registrarEvento(Tabuleiro* t, const char* msg);
void imprimirTabuleiro(Tabuleiro* t);
void imprimirLogRodada(Tabuleiro* t);
void imprimirEstatisticasFinais(Tabuleiro* t);
int  verificarVitoria(Tabuleiro* t);
void liberarTabuleiro(Tabuleiro* t);
void aguardarEnter(void);
void limparLog(Tabuleiro* t);
void pausar(int ms);
void limparTela(void);

                /*  HELPERS  */


const char* nomeTIME(TipoTIME f) {
    switch(f) {
        case TIME_OVELHA:  return "Ovelha";
        case TIME_LOBO:    return "Lobo";
        case TIME_CACADOR: return "Cacador";
    }
    return "?";
}

void registrarEvento(Tabuleiro* t, const char* msg) {
    if (t->log.total < MAX_EVENTOS) {
        strncpy(t->log.linhas[t->log.total], msg, TAM_EVENTO - 1);
        t->log.linhas[t->log.total][TAM_EVENTO - 1] = '\0';
        t->log.total++;
    }
}

void limparLog(Tabuleiro* t) {
    t->log.total = 0;
}

void aguardarEnter(void) {
    printf("  [ENTER para continuar]");
    while (getchar() != '\n');
}

void pausar(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

            /*  IMPLEMENTACAO  */

Tabuleiro* criarTabuleiro(int N) {
    if (N < MIN_TAMANHO) N = MIN_TAMANHO;
    Tabuleiro* t = (Tabuleiro*)malloc(sizeof(Tabuleiro));
    if (!t) { fprintf(stderr, "Sem memoria.\n"); exit(1); }



    t->largura = t->altura = N;
    t->listaEntidades = NULL;
    t->totalObstaculos = t->rodadaAtual = 0;
    t->proximoId = 1;
    memset(&t->stats, 0, sizeof(Estatisticas));
    limparLog(t);


    for (int i = 0; i < 3; i++) t->contagemPorTipo[i] = 0;


    t->estado   = (EstadoCelula**)malloc(N * sizeof(EstadoCelula*));
    t->conteudo = (void***)malloc(N * sizeof(void**));
    for (int i = 0; i < N; i++) {
        t->estado[i]   = (EstadoCelula*)calloc(N, sizeof(EstadoCelula));
        t->conteudo[i] = (void**)calloc(N, sizeof(void*));
    }
    return t;
}


int sortearPosicaoLivre(int* ocupado, int total) {
    int livres = 0;
    for (int i = 0; i < total; i++) if (!ocupado[i]) livres++;
    if (livres == 0) return -1;
    int alvo = rand() % livres, contador = 0;
    for (int i = 0; i < total; i++) {
        if (!ocupado[i]) {
            if (contador == alvo) { ocupado[i] = 1; return i; }
            contador++;
        }
    }
    return -1;
}


void posicionarAleatorio(Tabuleiro* t) {
    int total  = t->largura * t->altura;
    int qtdO   = (int)(total * PORC_TIME1);    if (qtdO   < 1) qtdO   = 1;
    int qtdL   = (int)(total * PORC_TIME2);    if (qtdL   < 1) qtdL   = 1;
    int qtdC   = (int)(total * PORC_TIME3);    if (qtdC   < 1) qtdC   = 1;
    int qtdObs = (int)(total * PORC_OBSTACULO); if (qtdObs < 1) qtdObs = 1;

    
    int* ocupado = (int*)calloc(total, sizeof(int));
    if (!ocupado) { fprintf(stderr, "Sem memoria.\n"); exit(1); }


    for (int i=0; i<qtdO;   i++) { int p=sortearPosicaoLivre(ocupado,total); if(p<0)break; adicionarEntidade(t, TIME_OVELHA,  p/t->largura, p%t->largura); }
    for (int i=0; i<qtdL;   i++) { int p=sortearPosicaoLivre(ocupado,total); if(p<0)break; adicionarEntidade(t, TIME_LOBO,    p/t->largura, p%t->largura); }
    for (int i=0; i<qtdC;   i++) { int p=sortearPosicaoLivre(ocupado,total); if(p<0)break; adicionarEntidade(t, TIME_CACADOR, p/t->largura, p%t->largura); }
    for (int i=0; i<qtdObs; i++) { int p=sortearPosicaoLivre(ocupado,total); if(p<0)break; adicionarObstaculo(t, (i%2==0)?OBST_ARVORE:OBST_PEDRA, p/t->largura, p%t->largura); }


                    /* REGISTRA PICOS INICIAIS POPULAÇÃO)*/
    for (int i=0;i<3;i++) t->stats.pico[i] = t->contagemPorTipo[i];

    free(ocupado);
}


void adicionarEntidade(Tabuleiro* t, TipoTIME tipo, int x, int y) {
    if (!posicaoValida(t,x,y) || t->estado[x][y] != CELULA_VAZIA) return;
    Entidade* e = (Entidade*)malloc(sizeof(Entidade));
    if (!e) return;
    e->tipo=tipo; e->x=x; e->y=y;
    e->energia=ENERGIA_INICIAL; e->idade=0; e->cooldownReproducao=0; e->morreuPorPredacao=0;
    e->id = t->proximoId++;
    e->proximo = t->listaEntidades;
    t->listaEntidades = e;
    t->estado[x][y]   = CELULA_ENTIDADE;
    t->conteudo[x][y] = e;
    t->contagemPorTipo[tipo]++;


                    /* ATUALIZAR POPULACÃO (PICO) */
    if (t->contagemPorTipo[tipo] > t->stats.pico[tipo])
        t->stats.pico[tipo] = t->contagemPorTipo[tipo];
}

void adicionarObstaculo(Tabuleiro* t, TipoObstaculo tipo, int x, int y) {
    if (!posicaoValida(t,x,y) || t->estado[x][y] != CELULA_VAZIA) return;
    Obstaculo* o = (Obstaculo*)malloc(sizeof(Obstaculo));
    if (!o) return;
    o->tipo=tipo; o->x=x; o->y=y;
    t->estado[x][y]   = CELULA_OBSTACULO;
    t->conteudo[x][y] = o;
    t->totalObstaculos++;
}
int posicaoValida(Tabuleiro* t, int x, int y) {
    return (x>=0 && x<t->largura && y>=0 && y<t->altura);
}



void movimentarTodos(Tabuleiro* t) {
    int dx[] = {-1,1,0,0}, dy[] = {0,0,-1,1};
    for (Entidade* e = t->listaEntidades; e != NULL; e = e->proximo) {
        if (e->energia <= 0) continue;
        e->energia -= CUSTO_MOVIMENTO;
        if (e->energia <= 0) continue;
        int ord[4]={0,1,2,3};
        for (int i=3;i>0;i--){int j=rand()%(i+1),tmp=ord[i];ord[i]=ord[j];ord[j]=tmp;}
        for (int i=0;i<4;i++) if (tentarMover(t, e, dx[ord[i]], dy[ord[i]])) { t->stats.movimentos++; break; }
    }
}



int tentarMover(Tabuleiro* t, Entidade* e, int dx, int dy) {
    int nx=e->x+dx, ny=e->y+dy;
    if (!posicaoValida(t,nx,ny) || t->estado[nx][ny] != CELULA_VAZIA) return 0;
    t->estado[e->x][e->y]   = CELULA_VAZIA;
    t->conteudo[e->x][e->y] = NULL;
    e->x=nx; e->y=ny;
    t->estado[nx][ny]   = CELULA_ENTIDADE;
    t->conteudo[nx][ny] = e;
    return 1;
}



void processarInteracoes(Tabuleiro* t) {
    int dx[]={-1,1,0,0}, dy[]={0,0,-1,1};
    for (Entidade* a = t->listaEntidades; a != NULL; a = a->proximo) {
        if (a->energia <= 0) continue;
        for (int i=0;i<4;i++) {
            int vx=a->x+dx[i], vy=a->y+dy[i];
            if (!posicaoValida(t,vx,vy) || t->estado[vx][vy] != CELULA_ENTIDADE) continue;
            Entidade* b = (Entidade*)t->conteudo[vx][vy];
            if (!b || b->energia<=0) continue;
            if (a->tipo==TIME_LOBO    && b->tipo==TIME_OVELHA)  predacaoLoboOvelha(t, a, b);
            if (a->tipo==TIME_CACADOR && b->tipo==TIME_LOBO)    predacaoCacadorLobo(t, a, b);
            if (a->tipo==TIME_CACADOR && b->tipo==TIME_OVELHA)  protecaoCacadorOvelha(t, a, b);
        }
    }
}


void predacaoLoboOvelha(Tabuleiro* t, Entidade* lobo, Entidade* ovelha) {
    char buf[TAM_EVENTO];

    snprintf(buf, TAM_EVENTO,
        "  [PREDACAO]  Lobo #%d comeu Ovelha #%d em (%d,%d). Lobo: %d -> %d energia.",
        lobo->id, ovelha->id, ovelha->x, ovelha->y,
        lobo->energia, lobo->energia + ENERGIA_PREDACAO);
    registrarEvento(t, buf);



    lobo->energia += ENERGIA_PREDACAO;
    if (lobo->energia > ENERGIA_MAXIMA) lobo->energia = ENERGIA_MAXIMA;
    ovelha->energia = 0;
    ovelha->morreuPorPredacao = 1;
    t->stats.predacoesLobo++;
    t->stats.mortesPorPredacao[TIME_OVELHA]++;
}


void predacaoCacadorLobo(Tabuleiro* t, Entidade* cacador, Entidade* lobo) {
    char buf[TAM_EVENTO];
    snprintf(buf, TAM_EVENTO,
        "  [CACADA]    Cacador #%d eliminou Lobo #%d em (%d,%d). Cacador: %d -> %d energia.",
        cacador->id, lobo->id, lobo->x, lobo->y,
        cacador->energia, cacador->energia + ENERGIA_PREDACAO/2);
    registrarEvento(t, buf);



    cacador->energia += ENERGIA_PREDACAO / 2;
    if (cacador->energia > ENERGIA_MAXIMA) cacador->energia = ENERGIA_MAXIMA;
    lobo->energia = 0;
    lobo->morreuPorPredacao = 1;
    t->stats.eliminacoesCacador++;
    t->stats.mortesPorPredacao[TIME_LOBO]++;
}


void protecaoCacadorOvelha(Tabuleiro* t, Entidade* cacador, Entidade* ovelha) {
    if (cacador->energia <= 5) return;
    char buf[TAM_EVENTO];
    snprintf(buf, TAM_EVENTO,

        "  [PROTECAO]  Cacador #%d protegeu Ovelha #%d em (%d,%d). Ovelha: %d -> %d energia.",
        cacador->id, ovelha->id, ovelha->x, ovelha->y,
        ovelha->energia, ovelha->energia + 10);
    registrarEvento(t, buf);


    cacador->energia -= 5;
    ovelha->energia  += 10;
    if (ovelha->energia > ENERGIA_MAXIMA) ovelha->energia = ENERGIA_MAXIMA;
    t->stats.protecoes++;
}



void processarReproducao(Tabuleiro* t) {
    int dx[]={-1,1,0,0}, dy[]={0,0,-1,1};
    int houveNascimento = 0;
    for (Entidade* a = t->listaEntidades; a != NULL; a = a->proximo) {
        if (a->energia<=0 || a->cooldownReproducao>0 || a->idade>IDADE_MAX_REPRO) continue;
        for (int i=0;i<4;i++) {
            int vx=a->x+dx[i], vy=a->y+dy[i];
            if (!posicaoValida(t,vx,vy) || t->estado[vx][vy] != CELULA_ENTIDADE) continue;
            Entidade* b = (Entidade*)t->conteudo[vx][vy];
            if (!b || b->energia<=0 || b->tipo!=a->tipo ||
                b->cooldownReproducao>0 || b->idade>IDADE_MAX_REPRO) continue;
            if (reproduzir(t, a, b)) {
                a->cooldownReproducao = COOLDOWN_REPRO;
                b->cooldownReproducao = COOLDOWN_REPRO;
                t->stats.nascimentos[a->tipo]++;
                houveNascimento = 1;

                char buf[TAM_EVENTO];



                        /*  (id == proximoId - 1)  FILHO É O ULTIMA A SER INSERIDO NA LISTA */
                snprintf(buf, TAM_EVENTO,
                    "  [NASCIMENTO] %s #%d e #%d geraram filho #%d perto de (%d,%d).",
                    nomeTIME(a->tipo), a->id, b->id, t->proximoId - 1, a->x, a->y);
                registrarEvento(t, buf);
                break;
            }
        }
    }
    if (houveNascimento) t->stats.rodadasComReproducao++;
}



int reproduzir(Tabuleiro* t, Entidade* a, Entidade* b) {
    int nx, ny;
    if (encontrarPosicaoLivreVizinha(t, a->x, a->y, &nx, &ny)) { adicionarEntidade(t, a->tipo, nx, ny); return 1; }
    if (encontrarPosicaoLivreVizinha(t, b->x, b->y, &nx, &ny)) { adicionarEntidade(t, a->tipo, nx, ny); return 1; }
    return 0;
}


int encontrarPosicaoLivreVizinha(Tabuleiro* t, int x, int y, int* nx, int* ny) {
    int dx[]={-1,1,0,0}, dy[]={0,0,-1,1}, ord[4]={0,1,2,3};
    for (int i=3;i>0;i--){int j=rand()%(i+1),tmp=ord[i];ord[i]=ord[j];ord[j]=tmp;}
    for (int i=0;i<4;i++){
        int d=ord[i], cx=x+dx[d], cy=y+dy[d];
        if (posicaoValida(t,cx,cy) && t->estado[cx][cy]==CELULA_VAZIA) { *nx=cx; *ny=cy; return 1; }
    }
    return 0;
}



void removerEntidadesMortas(Tabuleiro* t) {
    Entidade* atual=t->listaEntidades, *anterior=NULL;
    while (atual != NULL) {
        if (atual->energia <= 0) {


                                    /*  MORTE POR FOME SEM PREDAÇÃO */
            int porPredacao = (t->stats.mortesPorPredacao[atual->tipo] > 0);
            (void)porPredacao;


                            /* REGISTRO DAS MORTES POR FOME/ENVELHECIMENTO*/
            if (atual->energia == 0 &&
                !(atual->tipo==TIME_OVELHA  && t->stats.mortesPorPredacao[TIME_OVELHA]>0) &&
                !(atual->tipo==TIME_LOBO    && t->stats.mortesPorPredacao[TIME_LOBO]>0))
            {
                


                            /* CONTAGEM SIMPLES (FOME) */
            }
            if (!atual->morreuPorPredacao) t->stats.mortesPorFome[atual->tipo]++;

            t->estado[atual->x][atual->y]   = CELULA_VAZIA;
            t->conteudo[atual->x][atual->y] = NULL;
            t->contagemPorTipo[atual->tipo]--;

            char buf[TAM_EVENTO];
            snprintf(buf, TAM_EVENTO,
                "  [MORTE]      %s #%d morreu em (%d,%d). Idade: %d.",
                nomeTIME(atual->tipo), atual->id, atual->x, atual->y, atual->idade);
            registrarEvento(t, buf);

            Entidade* morreu = atual;
            if (!anterior) t->listaEntidades  = atual->proximo;
            else           anterior->proximo  = atual->proximo;
            atual = atual->proximo;
            free(morreu);
        } else {
            anterior = atual;
            atual    = atual->proximo;
        }
    }
}

void avancarIdade(Tabuleiro* t) {
    for (Entidade* e = t->listaEntidades; e != NULL; e = e->proximo) {
        e->idade++;
        if (e->cooldownReproducao > 0) e->cooldownReproducao--;
        if (e->idade > IDADE_SENIL)    e->energia -= 2;
    }
}



                      /*  EXIBICAO DA PARTIDA */

void imprimirTabuleiro(Tabuleiro* t) {
    int sep = t->largura * 3 + 6;
    for (int k=0; k<sep; k++) printf("=");
    printf("\n  RODADA %d\n", t->rodadaAtual);
    for (int k=0; k<sep; k++) printf("-");
    printf("\n");

    printf("   ");
    for (int y=0; y<t->largura; y++) printf("%3d", y);
    printf("\n");

    for (int x=0; x<t->largura; x++) {
        printf("%3d", x);
        for (int y=0; y<t->altura; y++) {
            if (t->estado[x][y] == CELULA_VAZIA) {
                printf("  .");
            } else if (t->estado[x][y] == CELULA_OBSTACULO) {
                Obstaculo* o = (Obstaculo*)t->conteudo[x][y];
                printf("  %c", o->tipo == OBST_ARVORE ? 'A' : 'P');
            } else {
                Entidade* e = (Entidade*)t->conteudo[x][y];
                printf(" %c", "OLC"[e->tipo]);
            }
        }
        printf("\n");
    }

    for (int k=0; k<sep; k++) printf("-");
    printf("\n");
    printf("  Ovelhas: %-3d  Lobos: %-3d  Cacadores: %-3d\n",
        t->contagemPorTipo[TIME_OVELHA],
        t->contagemPorTipo[TIME_LOBO],
        t->contagemPorTipo[TIME_CACADOR]);
    for (int k=0; k<sep; k++) printf("=");
    printf("\n");
}

void imprimirLogRodada(Tabuleiro* t) {
    if (t->log.total == 0) {
        printf("  (nenhum evento nesta rodada)\n");
    } else {
        for (int i = 0; i < t->log.total; i++) {
            printf("%s\n", t->log.linhas[i]);
        }
    }
}

void imprimirEstatisticasFinais(Tabuleiro* t) {
    const char* vencedor =
        t->contagemPorTipo[TIME_OVELHA]  > 0 ? "OVELHAS"   :
        t->contagemPorTipo[TIME_LOBO]    > 0 ? "LOBOS"     :
        t->contagemPorTipo[TIME_CACADOR] > 0 ? "CACADORES" : "EXTINCAO TOTAL";

    int sep = t->largura * 3 + 6;
    if (sep < 48) sep = 48;

    printf("\n");
    for (int k=0;k<sep;k++) printf("#");
    printf("\n  RESULTADO FINAL\n");
    for (int k=0;k<sep;k++) printf("#");

    printf("\n\n  Vencedor   : %s\n", vencedor);
    printf("  Rodadas    : %d\n\n", t->rodadaAtual);




                     /* CONTAGEM DA POPULAÇÃO (PICO) */
    printf("  POPULACAO FINAL\n");
    for (int k=0;k<sep;k++) printf("-");
    printf("\n");
    printf("  %-12s  Vivos: %-4d  Pico: %-4d  Nascimentos: %-4d\n",
        "Ovelhas",
        t->contagemPorTipo[TIME_OVELHA],
        t->stats.pico[TIME_OVELHA],
        t->stats.nascimentos[TIME_OVELHA]);
    printf("  %-12s  Vivos: %-4d  Pico: %-4d  Nascimentos: %-4d\n",
        "Lobos",
        t->contagemPorTipo[TIME_LOBO],
        t->stats.pico[TIME_LOBO],
        t->stats.nascimentos[TIME_LOBO]);
    printf("  %-12s  Vivos: %-4d  Pico: %-4d  Nascimentos: %-4d\n",
        "Cacadores",
        t->contagemPorTipo[TIME_CACADOR],
        t->stats.pico[TIME_CACADOR],
        t->stats.nascimentos[TIME_CACADOR]);





                /* COMBATE DOS JOGADORES */
    printf("\n  COMBATE\n");
    for (int k=0;k<sep;k++) printf("-");
    printf("\n");
    printf("  Ovelhas comidas pelos lobos       : %d\n", t->stats.predacoesLobo);
    printf("  Lobos eliminados pelos cacadores  : %d\n", t->stats.eliminacoesCacador);
    printf("  Protecoes cacador -> ovelha        : %d\n", t->stats.protecoes);

              /* MORTES  */
    printf("\n  MORTES\n");
    for (int k=0;k<sep;k++) printf("-");
    printf("\n");
    printf("  Ovelhas    : %d mortes no total\n",
        t->stats.mortesPorPredacao[TIME_OVELHA] + t->stats.mortesPorFome[TIME_OVELHA]);
    printf("  Lobos      : %d mortes no total\n",
        t->stats.mortesPorPredacao[TIME_LOBO] + t->stats.mortesPorFome[TIME_LOBO]);
    printf("  Cacadores  : %d mortes no total\n",
        t->stats.mortesPorPredacao[TIME_CACADOR] + t->stats.mortesPorFome[TIME_CACADOR]);


                        /* VISÃO GERAL */
    printf("\n  GERAL\n");
    for (int k=0;k<sep;k++) printf("-");
    printf("\n");
    printf("  Total de movimentos               : %d\n", t->stats.movimentos);
    printf("  Rodadas com nascimentos           : %d\n", t->stats.rodadasComReproducao);

    printf("\n");
    for (int k=0;k<sep;k++) printf("#");
    printf("\n");
}

int verificarVitoria(Tabuleiro* t) {
    int vivas = 0;
    for (int i=0;i<3;i++) if(t->contagemPorTipo[i]>0) vivas++;
    return (vivas<=1 || t->rodadaAtual>=MAX_RODADAS);
}

void liberarTabuleiro(Tabuleiro* t) {
    Entidade* e=t->listaEntidades;
    while(e){Entidade* p=e->proximo;free(e);e=p;}
    for(int i=0;i<t->largura;i++){
        for(int j=0;j<t->altura;j++) if(t->estado[i][j]==CELULA_OBSTACULO) free(t->conteudo[i][j]);
        free(t->estado[i]); free(t->conteudo[i]);
    }
    free(t->estado); free(t->conteudo); free(t);
}

                         /*  MAIN  */
int main(void) {
    srand((unsigned int)time(NULL));

    int tamanho;
    printf("____________________________________________\n");
    printf("   SIMULADOR DE ECOSSISTEMA — MODO VISUAL\n");
    printf("____________________________________________\n");
    printf("Tamanho do tabuleiro (minimo %d): ", MIN_TAMANHO);
    if (scanf("%d", &tamanho) != 1) tamanho = MIN_TAMANHO;
    while (getchar() != '\n');
    if (tamanho < MIN_TAMANHO) tamanho = MIN_TAMANHO;

    Tabuleiro* jogo = criarTabuleiro(tamanho);
    posicionarAleatorio(jogo);

    printf("\n  Legenda: O=Ovelha  L=Lobo  C=Cacador  A=Arvore  P=Pedra\n");
    printf("\nPressione ENTER para iniciar...");
    getchar();




                    /*  TABULEIRO INICIA ANTES DA 1° PARTIDA  */
    printf("\n");
    imprimirTabuleiro(jogo);
    printf("\nIniciando simulacao automatica...");
    pausar(2000);




                    /*  DEFINIÇÃO DO LOOP PRINCIPAL  */
    while (!verificarVitoria(jogo)) {
        jogo->rodadaAtual++;
        limparLog(jogo);

        movimentarTodos(jogo);
        processarInteracoes(jogo);
        removerEntidadesMortas(jogo);
        processarReproducao(jogo);
        avancarIdade(jogo);

        limparTela();

        printf("____________________________________________\n");
        printf("   SIMULADOR DE ECOSSISTEMA\n");
        printf("____________________________________________\n");

        imprimirTabuleiro(jogo);

        printf("\nEVENTOS DA RODADA %d:\n", jogo->rodadaAtual);
        imprimirLogRodada(jogo);

        printf("\nRESUMO:\n");
        printf("Ovelhas   : %d\n", jogo->contagemPorTipo[TIME_OVELHA]);
        printf("Lobos     : %d\n", jogo->contagemPorTipo[TIME_LOBO]);
        printf("Cacadores : %d\n", jogo->contagemPorTipo[TIME_CACADOR]);

        fflush(stdout);
        pausar(1000);
    }

                                    /*  RESULTADO DA PARTIDA */
    printf("\n");
    imprimirTabuleiro(jogo);
    imprimirEstatisticasFinais(jogo);

    liberarTabuleiro(jogo);
    return 0;
}