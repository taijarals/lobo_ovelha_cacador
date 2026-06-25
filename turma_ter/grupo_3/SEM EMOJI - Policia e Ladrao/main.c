#include "jogo.h"

int altura, largura; // Variáveis de altura e largura do mapa, declaradas de forma global para que sejam lidas pelas funções

int main() {
	char *mapa;
	int i, j, menu;
	int escolha = 1;
	char direcao;
	FILE *nivel; // Ponteiro de arquivo

	//JOGADOR;
	Player p;
	p.posicaox = 4; // Posição vertical do jogador
	p.posicaoy = 3; // Posição horizontal do jogador
	p.pontuacao = 0; // Inicializa a pontuação para evitar uma variável com lixo de memória

	Policial pm1, pm2;
	pm1.posicaox = 3; // Posição vertical do policial
	pm1.posicaoy = 12; // Posição horizontal do policial
	pm1.ultimoMovimento = 0; // 0 = Neutro, 1 = Cima, 2 = Baixo, 3 = Esquerda, 4 = Direita

    Saida s1;
    s1.posicaox = -1;
    s1.posicaoy = -1;
    
	nivel = fopen("nivel1.txt", "r"); // Abre o arquivo com o mapa

	if(nivel == NULL) {
		printf("Erro ao abrir arquivo\n");
		return 1; // Retorna um erro caso não seja possível abrir o arquivo
	}

	fscanf(nivel, " %d %d", &altura, &largura); // Lê os primeiros dois números do arquivo e os considera como tamanho da altura e da largura, alocando cada valor nas variáveis  altura e largura, respectivamente


	mapa = (char*)malloc(largura * altura * sizeof(char)); // Aloca memória o suficiente no ponteiro mapa, de acordo com os valores de altura e largura


	for(i = 0; i < altura; i++) {
		for(j = 0; j < largura; j++) {
			if(fscanf(nivel, " %c", &mapa[(i * largura + j)]) != 1) { // Caso a leitura do arquivo não retorne um caractere válido (True) imprima um texto de erro
				printf("Erro na leitura do mapa!");
				free(mapa);
				return 1;
			}
		}
	}
	fclose(nivel); // Fecha o arquivo após a leitura
	/*👮█💰🏃*/


	printf("---- Policia e Ladrao ----\nDigite 1 para jogar ou 0 para sair\n");
	scanf("%d", &menu);
	while(getchar() != '\n');

	switch(menu) {

	case 0:
		printf("Saindo...\n");
		free(mapa); // Libera a memoria alocada, evitando um vazamento de memoria
		return 0; // Encerra o programa

	case 1:
		while (1) {
			limparTela();
			pontuacao(&p); // Informa a pontuação do jogador na parte de cima da tela, acessando o endereço de memória do struct
			renderizarMapa(mapa, &p, &pm1, &s1); // Renderiza o mapa, jogador e policial
			if(p.pontuacao == 10){
			    printf("O seu parceiro chegou, FUJA!\n");
			}

			printf("Para onde ir? Digite 'w a s d' e digite ENTER - Digite 0 para SAIR\n");
			scanf(" %c", &direcao);
			while(getchar() != '\n'); // Limpa o buffer e faz o programa considerar somente o primeiro caractere digitado, evitando que o player ande várias casas de uma vez com um "wwww", por exemplo


			if(direcao == 'w' || direcao == 'W') { // "Se a tecla W for acionada, vá para cima"
				MoverParaCima(&p, mapa);

			}
			
			else if(direcao == 'a' || direcao == 'A') { // "Se a tecla A for acionada, vá para a esquerda"
				MoverParaEsquerda(&p, mapa);
			}
			

			else if(direcao == 's' || direcao == 'S') { //"Se a tecla S for acionada, vá para a direita"
				MoverParaBaixo(&p, mapa);
			}

			

			else if(direcao == 'd'|| direcao == 'D') { // "Se a tecla D for acionada, vá para baixo"
				MoverParaDireita(&p, mapa);
			}

			else if(direcao == '0') { // "Se a tecla 0 for acionada, finalize o jogo"
				printf("Saindo...\n");
				free(mapa); // Libera a memória alocada
				return 0; // Finaliza o programa
			}

			moverPolicial(&p, &pm1, mapa); // Ativa a movimentação do policial toda vez que o jogador der um comando


			if (p.posicaox == pm1.posicaox && p.posicaoy == pm1.posicaoy) { // Caso a posição do jogador e a do policial seja a mesma...
				pontuacao(&p); // Informe a pontuação
				renderizarMapa(mapa, &p, &pm1, &s1); // Renderize o mapa novamente
				gameOver(&p, &pm1, &mapa); // Finalize o jogo
			}
            
            if(p.pontuacao == 10){ // Caso o player colete 10 moedas...
                s1.posicaox = 13; // Faça a saída (carro) aparecer nas seguintes posições
                s1.posicaoy = 17;
                
            }
            
            if (p.posicaox == s1.posicaox && p.posicaoy == s1.posicaoy){ // Se o player alcançar o carro...
                fuga(&p,&pm1,&s1,mapa); // Chama a função que spawna o carro de fuga, possibilitando a finalização do jogo
            }

		}
	default: // Caso seja digitado algo além de 1 ou 0 no menu, retorne erro
		printf("Erro: Digite um valor valido!\n");
		free(mapa); // Libera a memória alocada
		return 1; // Encerra o programa com um erro

	}// Fim do case aqui

	return 0; // Finaliza o programa com êxito
}
