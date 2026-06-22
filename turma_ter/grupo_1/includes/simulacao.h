#ifndef SIMULACAO_H
#define SIMULACAO_H

#include "estruturas.h"

// Protótipos de funções de Tabuleiro (alocação e desalocação)
Tabuleiro* criar_tabuleiro(int l, int c);
void destruir_tabuleiro(Tabuleiro* tab);
void inicializar_elementos(Tabuleiro* tab);
void exibir_tabuleiro(Tabuleiro* tab);
void processar_rodada(Tabuleiro* tab);

#endif // SIMULACAO_H