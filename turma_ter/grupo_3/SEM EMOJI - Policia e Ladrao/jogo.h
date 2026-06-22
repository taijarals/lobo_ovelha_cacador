#ifndef JOGO_H
#define JOGO_H


#include <stdio.h>
#include <stdlib.h>

#define PAREDE '1' // Definição de constantes para fácil utilização e leitura do código
#define POLICIAL '2'
#define MOEDA '$'
#define VAZIO '.'
#define SAIDA '@'

extern int altura, largura; // Variáveis de altura e largura do mapa, declaradas de forma global para que sejam lidas pelas funções

typedef struct Player { // Struct com nome Player (para ser lido corretamente pela função RenderizarMapa)
	int vida;
	int pontuacao; // Pontuação: É aumentada com a coleta de moedas
	int posicaox; // Posição vertical do jogador
	int posicaoy; // Posição horizontal do jogador
} Player; // Alias da struct

typedef struct Policial { // Struct com a posição do policial
	int posicaox; // Posição vertical do policial
	int posicaoy; // Posição horizontal do policial
	int ultimoMovimento; // Guarda a última movimentação do policial para evitar repetição de movimentos e garantir uma perseguição eficiente
} Policial; // Alias da struct

typedef struct Saida{ // Define a posição do carro que possibilita a fuga do player
	int posicaox;
	int posicaoy;
} Saida;

void pontuacao(Player *p);

void limparTela();

void gameOver(Player *p, Policial *pm, char **m);

void verificarLocal(Player *c, char *m);

void moverPolicial(Player *c, Policial *pm, char *m);

void renderizarMapa(char *m, Player *p, Policial *pm, Saida *s1);

void fuga(Player *p, Policial *pm, Saida *s1, char *m);

void MoverParaCima(Player *c, char *m);

void MoverParaBaixo(Player *c,char *m);

void MoverParaEsquerda(Player *c, char *m);

void MoverParaDireita(Player *c, char *m);


#endif