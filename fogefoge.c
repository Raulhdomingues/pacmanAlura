#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fogefoge.h"
#include "mapa.h"
#include "ui.h"

MAPA m;
POSICAO heroi;
int tempilula = 0;

void lemapa(MAPA* m) {
	FILE* f;
	f = fopen("mapa.txt", "r");
	if(f == 0) {
		printf("Erro na leitura do mapa");
		exit(1);
	}

	fscanf(f, "%d %d", &(m->linhas), &(m->colunas));
	alocamapa(m);
	
	for(int i = 0; i < m->linhas; i++) {
		fscanf(f, "%s", m->matriz[i]);
	}

	fclose(f);
}

char desenhoparede[4][7] = {
	{"......" },
	{"......" },
	{"......" },
	{"......" }
};

char desenhofantasma[4][7] = {
	{" .-.  " },
	{"| OO| " },
	{"|   | " },
	{"'^^^' " }
};

char desenhoheroi[4][7] = {
	{" .--. "  },
	{"/ _.-'"  },
	{"\\  '-." },
	{" '--' "  }
};

char desenhopilula[4][7] = {
	{"      "},
	{" .-.  "},
	{" '-'  "},
	{"      "}
};

char desenhovazio[4][7] = {
	{"      "},
	{"      "},
	{"      "},
	{"      "}
};


void imprimeparte(char desenho[4][7], int parte) {
	printf("%s", desenho[parte]);
}

void imprimemapa(MAPA* m) {
	for(int i = 0; i < m->linhas; i++) {
		
		for(int parte = 0; parte < 4; parte++) {
			
			for (int j = 0; j < m->colunas; j++) {

				switch(m->matriz[i][j]) {
				case FANTASMA:
					imprimeparte(desenhofantasma, parte);
					break;
				
				case HEROI:
					imprimeparte(desenhoheroi, parte);
					break;

				case PILULA:
					imprimeparte(desenhopilula, parte);
					break;

				case PAREDE_VERTICAL:
				case PAREDE_HORIZONTAL:
					imprimeparte(desenhoparede, parte);
					break;

				case VAZIO:
					imprimeparte(desenhovazio, parte);
					break;
				}
			}
		}
	}
}

int encontramapa(MAPA* m, POSICAO* p, char c) {

	for(int i = 0; i < m->linhas; i++) {
		for(int j = 0; j < m->colunas; j++) {
			if(m->matriz[i][j] == c) {
				p->x = i;
				p->y = j;
				return 1;
			}
		}
	}
	return 0;
}


int ehparede(MAPA* m, int x, int y) {
	return m->matriz[x][y] == PAREDE_VERTICAL ||
		m->matriz[x][y] == PAREDE_HORIZONTAL;
}

int ehpersonagem(MAPA* m, char personagem, int x, int y){
	return m->matriz[x][y] == personagem;
}

int podeandar (MAPA* m, char personagem, int x, int y) {
	return
		ehValida(m, x, y) &&
		!ehparede(m, x, y) &&
		!ehpersonagem(m, personagem, x, y);
}

void liberamapa(MAPA* m) {
	for(int i = 0; i < m->linhas; i++) {
		free(m->matriz[i]);
	}

	free(m->matriz);
}

void alocamapa(MAPA* m) {
	m->matriz = malloc(sizeof(char*) * m->linhas);

	for(int i = 0; i < m->linhas; i++) {
		m->matriz[i] = malloc(sizeof(char) * m->colunas + 1);
	}
}

int ehValida(MAPA* m, int x, int y) {
	if(x >= m->linhas) 
		return 0;
	if(y >= m->colunas) 
		return 0;

	return 1;	
}

int ehVazia(MAPA* m, int x, int y) {
	return m->matriz[x][y] == VAZIO;
}

void copiamapa(MAPA* destino, MAPA* origem) {
	destino->linhas = origem->linhas;
	destino->colunas = origem->colunas;

	alocamapa(destino);
	for(int i = 0; i < origem->linhas; i++) {
		strcpy(destino->matriz[i], origem->matriz[i]);
	}
}

void andanomapa(MAPA* m, int xorigem, int yorigem, 
	int xdestino, int ydestino) {

	char personagem = m->matriz[xorigem][yorigem];
	m->matriz[xdestino][ydestino] = personagem;
	m->matriz[xorigem][yorigem] = VAZIO;

}

int praondefantasmavai(int xatual, int yatual, int* xdestino, int* ydestino) {
	int opcoes [4][2] = {
		{xatual, yatual +1},
		{xatual +1, yatual},
		{xatual, yatual -1},
		{xatual -1, yatual}
	};

	srand(time(0));
	for(int i = 0; i < 10; i++) {
		int posicao = rand() % 4;

		if(podeandar(&m, FANTASMA, opcoes[posicao][0], opcoes[posicao][1])) {
			*xdestino = opcoes[posicao][0];
			*ydestino = opcoes[posicao][1];
			return 1;
		}
	}
	return 0;
}

void fantasmas (){
	MAPA copia;

	copiamapa(&copia, &m);
	for(int i = 0; i < m.linhas; i++){
		for(int j = 0; j < m.colunas; j++) {
			if(copia.matriz[i][j] == FANTASMA) {
				int xdestino;
				int ydestino;

				int encontrou = praondefantasmavai(i, j, &xdestino, &ydestino);

				if(encontrou) {
					andanomapa (&m, i, j, xdestino, ydestino);
				}

				if(ehValida(&m, i, j+1) && ehVazia(&m, i, j+1)) {
					andanomapa(&m, i, j, i, j+1);
				}
			}
		}
	}
 	liberamapa(&copia);
}

int acabou() {
	POSICAO pos;
	int fogefogenomapa = encontramapa(&m, &pos, HEROI);
	return !fogefogenomapa;
}

int ehDirecao(char direcao) {
	return
		direcao == ESQUERDA || 
		direcao == CIMA ||
		direcao == BAIXO ||
		direcao == DIREITA;
}

void move(char direcao) {

	if(!ehDirecao(direcao))	
		return;

	int proximox = heroi.x;
	int proximoy = heroi.y;

	switch(direcao) {
		case ESQUERDA:
			proximoy--;
			break;
		case CIMA:
			proximox--;
			break;
		case BAIXO:
			proximox++;
			break;
		case DIREITA:
			proximoy++;
			break;
	}

	if(!podeandar(&m, HEROI, proximox, proximoy))
		return;

	if(ehpersonagem(&m, PILULA, proximox, proximoy)){
		tempilula = 1;
	}

	andanomapa(&m, heroi.x, heroi.y, proximox, proximoy);
	heroi.x = proximox;
	heroi.y = proximoy;
}

void explodepilula() {

	if(!tempilula) return;

	explodepilula2(heroi.x, heroi.y, 0, 1, 3);
	explodepilula2(heroi.x, heroi.y, 0, -1, 3);
	explodepilula2(heroi.x, heroi.y, 1, 0, 3);
	explodepilula2(heroi.x, heroi.y, -1, 0, 3);

	tempilula = 0;
}

void explodepilula2(int x, int y, int somax, int somay, int qtd) {

	if(qtd == 0) return;

	int novox = x + somax;
	int novoy = y + somay;

	if(!ehValida(&m, novox, novoy)) return;
	if(ehparede(&m, novox, novoy)) return;

	m.matriz[novox][novoy] = VAZIO;
	explodepilula2(novox, novoy, somax, somay, qtd - 1);	
}


int main() {
	
	lemapa(&m);
	encontramapa(&m, &heroi, HEROI);

	do {

		printf("Tem pilula: %s\n", (tempilula ? "SIM" : "NAO"));
		imprimemapa(&m);

		char comando;
		scanf(" %c", &comando);

		move(comando);
		if(comando == BOMBA) explodepilula();

		fantasmas();


	} while (!acabou());

	liberamapa(&m);
}