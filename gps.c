/*
Projeto Rotas por:

Arthur Veloso Kamienski
RA: 11002315

Bruno Aristimunha Pinto
RA: 21062916

Erick Akio Oti Aoyagui
RA: 11021315
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define INF 999999999

typedef struct Coordenada* coord;
typedef struct Vertice* vert;
typedef struct Aresta* aresta;
typedef struct ElementoHeap* eh;

struct Coordenada
{	int c1, c2;
	int v;
	coord prox;
};

//vulgo rua
struct Vertice
{
	int n;
	char* nome;
	char* municipio;
	int numero[2];
	int length, lado;
	vert ant;
	coord pMedio;
	coord c;
	aresta a;
};

struct Aresta
{
	vert v;
	aresta prox;
	coord cd;
};

//elemento da Heap para fila de prioridade
struct ElementoHeap
{
	int vertice;
	int dist;
};

//para debugar
void imprimeVertice(vert v)
{
	printf("Numero:\t\t %d\n", v->n);
	printf("Rua:\t\t %s\n", v->nome);
	printf("Numero Comeco:\t %d\n", v->numero[0]);
	printf("Numero Final:\t %d\n", v->numero[1]);
	printf("Municipio:\t %s\n", v->municipio);
	printf("Comprimento:\t %d\n", v->length);
	printf("Arestas:\t ");
	for(aresta a=v->a; a != NULL; a = a->prox)
		printf("%d ", a->v->n);
	printf("\n");
	printf("Coordenadas:\t ");
	for(coord c=v->c; c != NULL; c = c->prox)
		printf("[%lf, %lf] ", ((double)c->c1/-1000000), ((double)c->c2/-1000000));
	printf("\n");
	printf("Ponto Médio:\t [%lf, %lf]\n", ((double)v->pMedio->c1/-1000000), ((double)v->pMedio->c2/-1000000));
	printf("\n");
}

void imprimeGrafo(vert* grafo, int n)
{
	int i;
	for(i=0; i<n; i++)
		if(grafo[i] != NULL)
			imprimeVertice(grafo[i]);
}

coord criaCoord(int c1, int c2, int v)
{
	coord aux;

	aux = (coord) malloc(1*sizeof(struct Coordenada));
	aux->c1 = c1;
	aux->c2 = c2;
	aux->v = v;
	aux->prox = NULL;

	return aux;
}

//adiciona coord numa lista ligada de coords
void adicionaCoord(coord c1, coord c2)
{
	if(c1->prox == NULL)
		c1->prox = c2;
	else
		adicionaCoord(c1->prox, c2);
}

aresta criaAresta(vert v, coord cd)
{
	aresta aux = (aresta) malloc(sizeof(struct Aresta));

	aux->v = v;
	aux->prox = NULL;

	for(coord c=v->c; c != NULL; c = c->prox)
		if(c->c1 == cd->c1 && c->c2 == cd->c2)
			aux->cd = c;

	return aux;
}

//adiciona aresta numa lista ligada de arestas
void adicionaAresta(aresta a1, aresta a2)
{
	if(a1->prox == NULL)
		a1->prox = a2;
	else
		adicionaAresta(a1->prox, a2);
}

//conta o numero de linhas e coords para estimar espaco alocado
void contaLinhas(FILE *arq, int* a, int* b, int* c)
{
	long int t = time(NULL);
	int j = 0, i = 0;
	double a1, a2, maior=INF, menor=-INF;
	char n;

	printf("\n\nLendo Arquivo...\n");

	fseek(arq, 42, SEEK_SET);

	while(!feof(arq))
	{
		n = fgetc(arq);

		if(n == '[')
			while(n != ']')
			{
				fscanf(arq, "[%lf,%lf]", &a1, &a2);

				if(a1 < maior)
					maior = a1;

				if(a1 > menor)
					menor = a1;

				n = fgetc(arq);
			}
		else if(n == '\n')
			i++;
	}

	printf("Número estimado de vértices: %d\n", i-1);

	*a = i-1;
	*b = (int)(maior*(-1000000));
	*c = (int)(menor*(-1000000));

	printf("%ld segundos\n\n", time(NULL)-t);
}

void leCoords(coord *c, int numero, vert v, FILE *arq, int menor)
{
	coord aux;
	double a1, a2;
	int c1, c2, tamanho=0;
	int c1Medio=0, c2Medio=0;
	char n;

	while(n!=']')
	{
		fscanf(arq, "[%lf,%lf]", &a1, &a2);

		c1 = (int)(a1*(-1000000));
		c2 = (int)(a2*(-1000000));

		tamanho++;
		c1Medio += c1;
		c2Medio += c2;

		aux = criaCoord(c1, c2, numero);

		// printf("%d %d\n", c1, c2);

		if(c[c1-menor] == NULL)
			c[c1-menor] = aux;
		else
			adicionaCoord(c[c1-menor], aux);

		aux = criaCoord(c1, c2, numero);

		if(v->c == NULL)
			v->c = aux;
		else
			adicionaCoord(v->c, aux);

		n = fgetc(arq);
	}

	c1Medio = ((double)c1Medio/tamanho+0.5);
	c2Medio = ((double)c2Medio/tamanho+0.5);

	v->pMedio = criaCoord(c1Medio, c2Medio, numero);
}

int leNumero(FILE *arq)
{
	char n;
	int numero = -INF;

	while(n!=':')
		n = fgetc(arq);

	n = fgetc(arq);

	if(n != 'n')
	{
		fseek(arq, -1, SEEK_CUR);
		fscanf(arq, "%d", &numero);
	}

	return numero;
}

char* readString(FILE *arq, char delimiter)
{
	char* buf = (char*) malloc(200*sizeof(char));
	char* string;
	char n;
	int i, j;

	n = fgetc(arq);

	for(i=0; i<200 & n != delimiter; i++)
	{
		buf[i] = n;
		n = fgetc(arq);
	}

	string = (char*) malloc((i+1)*sizeof(char));

	for(j=0; j<i; j++)
		string[j] = buf[j];
	string[i] = '\0';

	free(buf);

	return string;
}

//liga vertices do grafo
void liga(coord* c, vert* grafo, int nCoords)
{
	coord cd1, cd2;
	aresta aux;
	int i;

	for(i=0; i<nCoords; i++)
	{
		cd1 = c[i];

		while(cd1 != NULL)
		{
			cd2 = cd1->prox;

			while(cd2 != NULL)
			{
				if(cd2->c1 == cd1->c1)
					if(cd2->c2 == cd1->c2)
					{
						aux = criaAresta(grafo[cd2->v], cd2);

						if(grafo[cd1->v]->a == NULL)
							grafo[cd1->v]->a = aux;
						else
							adicionaAresta(grafo[cd1->v]->a, aux);

						aux = criaAresta(grafo[cd1->v], cd2);

						if(grafo[cd2->v]->a == NULL)
							grafo[cd2->v]->a = aux;
						else
							adicionaAresta(grafo[cd2->v]->a, aux);
					}

				cd2 = cd2->prox;
			}

			cd1 = cd1->prox;
		}
	}
}

//free recursivamente de lista ligada
void freeRecurs(coord c)
{
	if(c != NULL)
		freeRecurs(c->prox);

	free(c);
}

void lerArq(FILE *arq, vert* grafo, int nVertices, int nCoords, int menor)
{
	long int t = time(NULL);
	int numero=-1, porcent, ultimo=0;
	int startR, startL, endR, endL, i, j;
	double d;
	char n;
	char* buf;
	coord aux;

	coord *c = (coord*) malloc(nCoords*sizeof(coord));
	for(i=0; i<nCoords; i++)
		c[i] = NULL;

	fseek(arq, 42, SEEK_SET);

	printf("Gerando grafo...\n");

	while(!feof(arq))
	{
		n = fgetc(arq);

		if(n=='[')
		{
			numero++;

			porcent = ((numero*100)/nVertices);
			
			for(i=0; i<porcent-ultimo; i++)
				printf("|");

			ultimo = porcent;

			fflush(stdout);

			// printf("%d\n", numero);
			startR = startL = endL = endR = 0;
			grafo[numero] = (vert) malloc(sizeof(struct Vertice));
			grafo[numero]->n = numero;
			grafo[numero]->c = NULL;
			grafo[numero]->a = NULL;

			leCoords(c, numero, grafo[numero], arq, menor);

			fseek(arq, 2, SEEK_CUR);

			while(n != '}')
			{
				n = fgetc(arq);

				if(n == '"')
				{
					buf = readString(arq, '"');
					
					if(!strcmp(buf, "LENGTH"))
					{
						fscanf(arq, ":%lf", &d);
						grafo[numero]->length = (1000*d)+0.5;

						// printf("%lf\n", d);
						// printf("%d\n", grafo[numero]->length);
					}
					else if(!strcmp(buf, "MUNICIPIO"))
					{
						fseek(arq, 2, SEEK_CUR);
						grafo[numero]->municipio = readString(arq, '"');
						n = fgetc(arq);
					}
					else if(!strcmp(buf, "START_LEFT"))
					{
						startL = leNumero(arq);
						endL = leNumero(arq);
						startR = leNumero(arq);
						endR = leNumero(arq);

						// printf("%d\n", startL);
						// printf("%d\n", endL);
						// printf("%d\n", startR);
						// printf("%d\n", endR);

						if(startL == -INF)
							startL = INF;
						if(startR == -INF)
							startR = INF;

						// if((startL == -INF && endL == INF) || (startR == -INF && endR == INF))
						// 	if(startL != -INF || startR != -INF || endL != INF || endR != INF)
						// 		grafo[i]->lado = 1;

						grafo[numero]->numero[0] = (startR>startL)? startL : startR;
						grafo[numero]->numero[1] = (endR>endL)? endR : endL;
					}
					else if(!strcmp(buf, "NOME_CAPS"))
					{
						fseek(arq, 2, SEEK_CUR);

						grafo[numero]->nome = readString(arq, '"');

						// printf("%s\n", grafo[numero]->nome);
					}
				}
			}
		}
	}

	printf("| 100%%\n");

	liga(c, grafo, nCoords);

	for(i=0; i<nCoords; i++)
		freeRecurs(c[i]);
	free(c);

	printf("%ld segundos\n\n", time(NULL)-t);
}

void troca(eh* A, int i, int j)
{
	eh aux = A[i];
	A[i] = A[j];
	A[j] = aux;
}

void MinHeapify(eh *A, int m, int i) 
{
	int e = 2*i;
	int d = 2*i + 1;
	int menor;

	if(e<=m && A[e]->dist<A[i]->dist)
		menor = e;
	else
		menor = i;

	if(d<=m && A[d]->dist<A[menor]->dist)
		menor = d;

	if(menor != i)
	{
		troca(A, i, menor);
		MinHeapify(A, m, menor);
	}
}

void HeapIncreaseKey(eh *A, int i, int key)
{
	if(key <= A[i]->dist)
	{
		A[i]->dist = key;

		while(i>1 && A[i/2]->dist > A[i]->dist)
		{
			troca(A, i, i/2);

			i /= 2;
		}
	}
}

void MinHeapInsert(eh *A, int *n, int key, int v)
{
	*n = *n + 1;

	eh novo = (eh) malloc(sizeof(struct ElementoHeap));

	novo->vertice = v;
	novo->dist = INF;

	A[*n] = novo;

	HeapIncreaseKey(A, *n, key);
}

int HeapExtractMin(eh *A, int *n)
{
	if(*n>0)
	{
		int min = A[1]->vertice;

		A[1] = A[*n];

		*n = *n-1;

		MinHeapify(A, *n, 1);

		return min;
	}
}

//atualiza valor de um elemento
void atualiza(int size, eh* A, int key, int v)
{
	int i, f=1;

	for(i=1; i<size && f; i++)
		if(A[i]->vertice == v)
		{
			f = 0;
			HeapIncreaseKey(A, i, key);
		}
}

void dijkstra(int s, int t, int n, vert* grafo)
{
	int i, size = 0;

	eh* heap = (eh*) malloc((n+1)*sizeof(eh));
	int* dist = (int*) malloc(n*sizeof(int));

	for(i=0; i<n; i++)
		dist[i] = INF;

	dist[s] = 0;

	vert u = grafo[s];

	MinHeapInsert(heap, &size, 0, s);

	while(u->n != t && size != 0)
	{
		u = grafo[HeapExtractMin(heap, &size)];

		for(aresta a=u->a; a != NULL; a = a->prox)
			if(dist[a->v->n] > (dist[u->n] + u->length))
			{
				if(dist[a->v->n] == INF)
					MinHeapInsert(heap, &size, (dist[u->n] + u->length), a->v->n);
				else
					atualiza(size, heap, (dist[u->n] + u->length), a->v->n);
					
				dist[a->v->n] = (dist[u->n] + u->length);
				a->v->ant = u;
			}
	}
}

//calcula pra que lado faz a curva
int calculaCurva(vert rua1, vert rua2)
{
	coord cd;
	double cordRua[2][2];
	double v, u;
	double c1, c2;
	double a1, a2;
	int curva;

	for(aresta a=rua1->a; a != NULL; a = a->prox)
		if(a->v == rua2)
			cd = a->cd;

	cordRua[0][0] = cd->c1 - rua1->pMedio->c1;
	cordRua[0][1] = cd->c2 - rua1->pMedio->c2;
	cordRua[1][0] = rua2->pMedio->c1 - rua1->pMedio->c1;
	cordRua[1][1] = rua2->pMedio->c2 - rua1->pMedio->c2;

	v = cordRua[0][0]*cordRua[1][0] + cordRua[0][1]*cordRua[1][1];
	v /= (sqrt(pow(cordRua[0][0],2) + pow(cordRua[0][1],2)));
	v /= (sqrt(pow(cordRua[1][0],2) + pow(cordRua[1][1],2)));
	v = acos(v);
	v *= (180.0/M_PI);

	if(v > 20)
	{
		v = cordRua[1][1];
		v /= (sqrt(pow(cordRua[1][0],2) + pow(cordRua[1][1],2)));
		v = acos(v);

		c1 = cos(v)*cordRua[1][0] - sin(v)*cordRua[1][1];

		if((int)c1 != 0)
			v = -v;

		a1 = cos(v)*cordRua[0][0] - sin(v)*cordRua[0][1];

		if(a1 < 0)
			curva = 0;
		else
			curva = 1;
	}
	else
		curva = -1;

	return curva;
}

//encontra a rua com o nome, municipio e numero informado
int findRua(char* nome, char* municipio, int numero, int n, vert* grafo)
{
	int i, num, provisorio = 0, find = 0;

	for(i=0; i<n && !find; i++)
		if(grafo[i] != NULL)
			if(!strcmp(grafo[i]->nome, nome) && !strcmp(grafo[i]->municipio, municipio))
			{
				if(!provisorio)
				{
					provisorio = 1;
					num = i;
				}

				if(numero >= grafo[i]->numero[0] || grafo[i]->numero[0] == -INF)
					if(numero <= grafo[i]->numero[1] || grafo[i]->numero[1] == INF)
					{
						if(grafo[i]->numero[0] == -INF || grafo[i]->numero[1] == INF)
							provisorio = 1;
						else if(grafo[i]->lado)
							if(grafo[i]->numero[0]%2==0 && numero%2!=0)
								provisorio = 1;
							else
								find = 1;
						else
							find = 1;

						num = i;
					}
			}

	if(!find)
		if(provisorio)
			printf("\nNumero nao encontrado, calculando rota para a rua sem numero\n");
		else
		{
			printf("\nEndereco nao encontrado\n");
			num = -1;
		}
	
	// if(num != -1) imprimeVertice(grafo[num]);
	return num;
}

//transforma tudo em maiusculas
void maiusc(char* str)
{
	int i;

	for(i=0; i<strlen(str); i++)
		str[i] = toupper(str[i]);
}

//le endereco separado por virgula
int leEndereco(int n, vert* grafo, int* nRua)
{
	char *num, *municipio, *rua, c;
	int v;

	rua = readString(stdin, ',');
	maiusc(rua);

	c = fgetc(stdin);

	num = readString(stdin, ',');

	c = fgetc(stdin);

	*nRua = atoi(num);

	municipio = readString(stdin, '\n');
	maiusc(municipio);

	// printf("%s, %s, %s\n", rua, num, municipio);

	v = findRua(rua, municipio, atoi(num), n, grafo);

	return v;
}

//calcula a distancia entre a coordenada da interseccao e o numero do endereco
int distNumero(int numero, vert rua1, vert rua2)
{
	aresta a;
	double dist;

	for(a=rua1->a; a != NULL; a = a->prox);

	if(a == NULL)
		dist = abs(rua1->numero[1]-numero);
	else
		dist = abs(rua1->numero[0]-numero);

	return (int)dist;
}

void tracaRota(int n, vert* grafo)
{
	int nOrigem = -1, curva, nDestino = -1, distTotal=0, distRua=0;
	int nRuaO, nRuaD;

	while(nOrigem == -1)
	{
		printf("Digite o endereco de origem:\n");
		nOrigem = leEndereco(n, grafo, &nRuaO);
		// nOrigem = findRua("AV DOS ESTADOS", "SANTO ANDRE", 5001, n, grafo);
	}

	while(nDestino == -1)
	{
		printf("\nDigite o endereco de destino:\n");
		nDestino = leEndereco(n, grafo, &nRuaD);
		// nDestino = findRua("AV KENNEDY", "SAO BERNARDO DO CAMPO", 1125, n, grafo);
	}
		
	dijkstra(nDestino, nOrigem, n, grafo);

	distRua += distNumero(nRuaO, grafo[nOrigem], grafo[nOrigem]->ant);

	vert ultimo;

	printf("\n");

	printf("Rota:\n\n");

	for(vert u = grafo[nOrigem]; u->n != nDestino; u=u->ant)
	{
		if(strcmp(u->nome, u->ant->nome))
		{
	 		printf("Em %d metros ", distRua);

			curva = calculaCurva(u, u->ant);
			if(curva != -1)
				if(curva)
					printf("< Vire a esquerda em ");
				else
					printf("> Vire a direita em ");
			else
				printf("^ Continue para ");

			printf("%s\n", u->ant->nome);

			distTotal += distRua;
			distRua = 0;
	 	}

		distRua += u->ant->length;
		ultimo = u;
	}

	distRua = distNumero(nRuaD, grafo[nDestino], ultimo);

	printf("Em %d metros voce chegara ao seu destino\n", distRua);

	distTotal += distRua;

	printf("Distancia total: %d\n\n", distTotal);
}

int main(int argc, char** argv)
{
	FILE *arq = fopen("Log2016_CEM_RMSP.json", "r");
	// FILE *arq = fopen("ruas.json", "r");
	int nVertices, nCoords, maior, menor;
	char resp;

	contaLinhas(arq, &nVertices, &maior, &menor);

	vert* grafo = (vert*) malloc(nVertices*sizeof(vert));

	nCoords = maior - menor;

	lerArq(arq, grafo, nVertices, nCoords, menor);
	fclose(arq);

	// imprimeGrafo(grafo, nVertices);

	printf("****Endereco no formado RUA, NUMERO, CIDADE****\n");
	printf("****Nomes de ruas e cidades devem ser identicos ao do arquivo****\n\n");
	printf("Exemplo:\n\n");
	printf("av dos estados, 5001, santo andre\n");
	printf("av kennedy, 1125, sao bernardo do campo\n\n");

	do
	{
		tracaRota(nVertices, grafo);
		printf("Deseja realizar nova busca? Y\\N\n");
		scanf("%c", &resp);
	}
	while(resp == 'Y' || resp == 'y');

	return 0;
}