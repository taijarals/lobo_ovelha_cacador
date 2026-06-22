#include "jogo.h"

void pontuacao(Player *p) { // Funcao para exibir a pontuacao atual do jogador durante a partida. Adicionado acima das outras funções para que possa ser lido por elas
	printf("Pontuacao: %d\n", p->pontuacao); // Acessa o valor de pontuação da struct por referência
}

void limparTela() { // Comando para limpar a tela, adaptado a diferentes sistemas
	#ifdef _WIN32
	system("cls"); // Usa cls se o OS for windows
	#else
	system("clear"); // Usa clear se o OR for linux
	#endif
}

void gameOver(Player *p, Policial *pm, char **m) { // Padroniza o comportamento de um game over em uma função só. Se precisa usar um ponteiro de ponteiro porque a alocação de memória já é um ponteiro, então precisa ser referenciado
	int tentarNovamente;
	FILE *nivel; // Abre novamente o arquivo do mapa
	free(*m); // Libera a memória alocada anteriormente
	do { // Repita a tela de gameover até ser digitada uma opção válida

		printf("\n---- G A M E   O V E R ----\nVoce foi pego! Digite 1 para tentar novamente ou 0 para sair\n");
		scanf("%d", &tentarNovamente);
		while(getchar() != '\n');

		switch(tentarNovamente) {
		case 0:
			printf("Saindo...\n");
			exit(0); // Sai do programa


		case 1:

			p->posicaox = 4; // Reinicia a posição vertical do jogador
			p->posicaoy = 3; // Reinicia a posição horizontal do jogador
			p->pontuacao = 0; // Reinicia a pontuação para zero

			pm->posicaox = 3; // Reinicia a posição vertical do policial
			pm->posicaoy = 12; // Reinicia a posição horizontal do policial
			pm->ultimoMovimento = 0; // Reinicia o contador de último movimento do policial

			nivel = fopen("nivel1.txt", "r"); // Abre o arquivo com o mapa


			if(nivel == NULL) { // Caso não consiga ler o arquivo...
			
				printf("Erro ao abrir arquivo\n");
				exit(1);
			}


			fscanf(nivel, " %d %d", &altura, &largura); // Relê o mapa e considera os dois primeiros números como altura e largura

			*m = (char*)malloc(largura * altura * sizeof(char)); // Aloca memória o suficiente no ponteiro mapa, de acordo com os valores de altura e largura

			for(int i = 0; i < altura; i++) {
				for(int j = 0; j < largura; j++) {
					if(fscanf(nivel, " %c", &(*m)[(i * largura + j)]) != 1) { // Escrever "&(*m)" força o compilador a ler primeiro a matriz, depois o seu índice. Isso evita um segmentation fault
						printf("Erro na leitura do mapa!"); // Caso não consiga ler o arquivo, imprima erro
						free(*m); // Libera memória alocada
						exit(1);
					}
				}
			}
			fclose(nivel); // Fecha o arquivo
			break;
		}
	}
	while(tentarNovamente != 1);
}

void verificarLocal(Player *c, char *m) {
	if (m[c->posicaox * largura + c->posicaoy] == MOEDA) { // Caso a posição do jogador seja a mesma que a de uma moeda, aumente a pontuação do jogador
		c->pontuacao++;
		m[c->posicaox * largura + c->posicaoy] = VAZIO; // Substitui o local que a moeda estava por um espaço em branco
	}
}

void moverPolicial(Player *c, Policial *pm, char *m) { // Mecânica de perseguição inteligente do policial
	if(pm->posicaox > c->posicaox && m[(pm->posicaox - 1) * largura + pm->posicaoy] != PAREDE && pm->ultimoMovimento != 2) { // Se o policial estiver mais baixo que o player, FAÇA ELE IR PARA CIMA. Não execute se ele tiver acabado de ir para baixo
		pm->posicaox--;
		pm->ultimoMovimento = 1; // Informa que a última movimentação foi para cima
	}

	else if(pm->posicaox < c->posicaox && m[(pm->posicaox + 1) * largura + pm->posicaoy] != PAREDE && pm->ultimoMovimento != 1) { // Se o policial estiver mais alto que o player, FAÇA ELE IR PARA BAIXO. Não execute se ele tiver acabado de ir para cima
		pm->posicaox++;
		pm->ultimoMovimento = 2; // Informa que a última movimentação foi para baixo
	}

	else if(pm->posicaoy > c->posicaoy && m[pm->posicaox * largura + (pm->posicaoy - 1)] != PAREDE && pm->ultimoMovimento != 4) { // Se o policial estiver mais para a esquerda que o jogador FAÇA ELE IR PARA A ESQUERDA. Não execute se ele tiver acabado de ir para a esquerda
		pm->posicaoy--;
		pm->ultimoMovimento = 3; // Informa que a última movimentação foi para esquerda
	}

	else if(pm->posicaoy < c->posicaoy && m[pm->posicaox * largura + (pm->posicaoy + 1)] != PAREDE && pm->ultimoMovimento != 3) { // Se o policial estiver mais para a direita que o jogador FAÇA ELE IR PARA A DIREITA. Não execute se ele tiver acabado de ir para a direita
		pm->posicaoy++;
		pm->ultimoMovimento = 4; // Informa que a última movimentação foi para direita
	}

	//Caso encontre paredes...

	// "Se houver alguma parede dos lados, contorne verticalmente"
	else if(pm->posicaoy != c->posicaoy && m[(pm->posicaox - 1) * largura + pm->posicaoy] != PAREDE && pm->ultimoMovimento != 2) { // Se o policial não estiver na mesma linha horizontal, o bloco acima não for uma parede e não tenha acabado de se movimentar para baixo, vá para CIMA
		pm->posicaox--; // Cima
		pm->ultimoMovimento = 1;
	}

	else if(pm->posicaoy != c->posicaoy && m[(pm->posicaox + 1) * largura + pm->posicaoy] != PAREDE && pm->ultimoMovimento != 1) { // Se o policial não estiver na mesma linha horizontal, o bloco abaixo não for uma parede e não tenha acabado de se movimentar para cima, vá para BAIXO
		pm->posicaox++;
		pm->ultimoMovimento = 2;
	}

	// "Se houver alguma parece acima, contorne horizontalmente"
	else if(pm->posicaox != c->posicaox && m[pm->posicaox * largura + (pm->posicaoy - 1)] != PAREDE && pm->ultimoMovimento != 4) { // Se o policial não estiver na mesma linha vertical, o bloco à esquerda não for uma parede e não tenha acabado de se movimentar para direita, vá para ESQUERDA
		pm->posicaoy--;
		pm->ultimoMovimento = 3;
	}

	else if(pm->posicaox != c->posicaox && m[pm->posicaox * largura + (pm->posicaoy + 1)] != PAREDE && pm->ultimoMovimento != 3) { // Se o policial não estiver na mesma linha vertical, o bloco à direita não for uma parede e não tenha acabado de se movimentar para esquerda, vá para DIREITA
		pm->posicaoy++;
		pm->ultimoMovimento = 4;
	}
}

void renderizarMapa(char *m, Player *p, Policial *pm, Saida *s1) { // Struct sendo passado como parâmetro para ser lido pelo laço for
	for(int i = 0; i < altura; i++) {
		for(int j = 0; j < largura; j++) {
		    
		    if(i == s1->posicaox && j == s1->posicaoy) {
			    printf("#   ");
			}
			else if(i == pm->posicaox && j == pm->posicaoy) { // Se i e j forem iguais a posição do policial, imprima 👮
				printf("&   "); // É printado antes do jogador para que que o sobreponha após o gameover
			}

			else if(i == p->posicaox && j == p->posicaoy) { // Se i e j forem iguais a posição do jogador, imprima 🏃
				printf("@   "); // Com três espaçamentos para evitar erro de exibição
			}

			else if(m[i * largura + j] == MOEDA) { // Caso encontre um "$" no código, substitui pelo emoji
				printf("$   ");
			}

			else if(m[i * largura + j] == PAREDE) { // Caso encontre um "1" no código, substitui pelo emoji
				printf("|   ");
			}
			
			
			/*👮█💰🏃*/
			else {
				printf(".   "); // Se o conteúdo da matriz não for vazio E NÃO FOR IGUAL A POSIÇÃO DO JOGADOR, imprima seu conteúdo
			}
		}


		printf("\n"); // Quebra de linha para organização
	}
}

void fuga(Player *p, Policial *pm, Saida *s1, char *m){ // Faz aparecer uma mensagem de vitória caso o player colete 10 moedas e fuja entrando no carro
	if(p->posicaox == s1->posicaox && p->posicaoy == s1->posicaoy){
	int escolha;

	do{ // Repita a mensagem...
	limparTela();
	renderizarMapa(m,p,pm,s1);
	printf("\nParabéns, você venceu! Pressione 0 para sair\n");
	scanf("%d", &escolha);
	while(getchar() != '\n');

	switch(escolha){
	case 0:
	printf("Saindo...\n");
	free(m);
	exit(0);
	break;
        
	default:
	printf("Digite um valor valido!\n");
	}
	}
	while (escolha != 0); ///... enquanto não for digitado 0
	}
}

void MoverParaCima(Player *c, char *m) {
	if((c->posicaox - 1) >= 0 && m[(c->posicaox - 1) * largura + c->posicaoy] != PAREDE && m[(c->posicaox - 1) * largura + c->posicaoy] != POLICIAL) { // Caso o novo movimento não seja no mesmo local de uma parede ou policial e nem seja menos que zero (acesso indevido de memoria) mova o jogador
		c->posicaox--;
		verificarLocal(c, m); // Verifica se o novo local tem uma moeda
	}
}

void MoverParaBaixo(Player *c,char *m) {
	if((c->posicaox + 1) < altura && m[(c->posicaox + 1) * largura + c->posicaoy] != PAREDE && m[(c->posicaox + 1) * largura + c->posicaoy] != POLICIAL) { // Caso o novo movimento não seja no mesmo local de uma parede ou policial e nem tente acessar áreas além do valor de altura do mapa (acesso indevido de memoria) mova o jogador
		c->posicaox++;
		verificarLocal(c, m);
	}

}

void MoverParaEsquerda(Player *c, char *m) {
	if((c->posicaoy - 1) >= 0 && m[c->posicaox * largura + (c->posicaoy - 1)] != PAREDE && m[c->posicaox * largura + (c->posicaoy - 1)] != POLICIAL) {
		c->posicaoy--;
		verificarLocal(c, m);
	}
}

void MoverParaDireita(Player *c, char *m) {
	if((c->posicaoy + 1) < largura && m[c->posicaox * largura + c->posicaoy + 1] != PAREDE && m[(c->posicaox) * largura + c->posicaoy + 1] != POLICIAL) {
		c->posicaoy++;
		verificarLocal(c, m);
	}
}
