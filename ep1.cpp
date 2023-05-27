// Ep1 realizado por D1m1tri

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <limits.h>

// elemento das listas de adjacência e de resposta - NAO ALTERE ESTA DEFINICAO
typedef struct estr 
{
  int adj; // elemento
  int peso; // custo (não precisa ser usado na resposta)
  struct estr *prox;
} NO;

// vertices do grafo (salas) - use este tipo ao criar o grafo  - NAO ALTERE ESTA DEFINICAO
typedef struct 
{
  int flag; // para uso na busca em largura e profundidade, se necessario
  bool aberto; // vale true se a sala em questao esta aberta
  int via; // use este campo se precisar computar algum caminho etc.
  int dist; // use este campo se precisar computar alguma distância etc.
  NO* inicio;
} VERTICE;


// funcao principal
NO *caminho(int N, int A, int *ijpeso, int *aberto, int inicio, int fim, int chave);
//------------------------------------------
// O EP consiste em implementar esta funcao
// e outras funcoes auxiliares que esta
// necessitar
//------------------------------------------

// função para criar arestas
void criarAresta(VERTICE* grafo, int anterior, int proximo, int peso){
  NO* temp = grafo[anterior].inicio;
  grafo[anterior].inicio = (NO*) malloc(sizeof(NO));
  grafo[anterior].inicio->prox = temp;
  grafo[anterior].inicio->adj = proximo;
  grafo[anterior].inicio->peso = peso;


  temp = grafo[proximo].inicio;
  grafo[proximo].inicio = (NO*) malloc(sizeof(NO));
  grafo[proximo].inicio->prox = temp;
  grafo[proximo].inicio->adj = anterior;
  grafo[proximo].inicio->peso = peso;
}

// funcao para criar o grafo
VERTICE* criarGrafo(int N, int A, int * ijpeso, int * aberto){
  VERTICE * grafo = (VERTICE *) malloc(N * sizeof(VERTICE));
  for (int i=0; i<N;i++) {
    grafo[i].aberto = aberto[i];
  }
  int i=0;
  while (i<A*3) {
    criarAresta(grafo, ijpeso[i]-1, ijpeso[i+1]-1, ijpeso[i+2]);
    i = i+3;
  }
  return grafo;
}

// função para inicializar o grafo
void inicializarGrafo(VERTICE * grafo, int * distancia, int * anterior, int inicio, int tamanho){
  for (int v=0; v<tamanho; v++){
    distancia[v] = INT_MAX/2;
    anterior[v] = -1;
  }
  distancia[inicio] = 0;
}

// função para relaxar a aresta
void relaxa(VERTICE * grafo, int * distancia, int * anterior, int verticeA, int verticeB){
  NO* no = grafo[verticeA].inicio;
  while (no && no->adj != verticeB) {
    no = no->prox;
  }
  if (no) {
    if (distancia[verticeB] > distancia[verticeA] + no->peso) {
      distancia[verticeB] = distancia[verticeA] + no->peso;
      anterior[verticeB] = verticeA;
    }
  }
}

// função para verificar a existência de vertices em aberto
bool existeAberto(bool * aberto, int N){
  for (int i=0; i<N; i++) {
    if (aberto[i]) return true;
  }
  return false;
}

// busca o primeiro nó aberto
int menorDist(int N, bool * aberto, int * distancia){
  int i;
  for (i=0; i<N; i++) {
    if (aberto[i]) {
      break;
    }
  }
  if (i==N) {
    return(-1);
  }
  int menor=i;
  for (i = menor+1; i<N; i++) {
    if (aberto[i] && (distancia[menor]>distancia[i])) {
      menor = i;
    }
  }
  return menor;
}

// algoritmo de Dijkstra
int * dijkstra(VERTICE * grafo, int N, int inicio, int * fechado, bool bloquearPassagem){
  int * distancia = (int*) malloc(N*sizeof(int));
  int * anterior = (int *) malloc(N*sizeof(int));
  inicializarGrafo(grafo, distancia, anterior, inicio, N);
  bool aberto[N];
  for (int i=0; i<N; i++) {
    aberto[i] = true;
  }
  bool tenhoChave = false;
  while (existeAberto(aberto, N)) {
    int u = menorDist(N, aberto, distancia);
    aberto[u] = false;
    NO * no = grafo[u].inicio;
    while (no) {
      if (bloquearPassagem){
        if (fechado[u] == 1) {
          relaxa(grafo, distancia, anterior, u, no->adj);
        }
      }
      else {
        relaxa(grafo, distancia, anterior, u, no->adj);
      }
      no = no->prox;
    }
  }
  int * resultado = (int*) malloc(N*3*sizeof(int));
  int j=0;
  for (int i=0; i<N; i++) {
    resultado[j] = i;
    j++;
    resultado[j] = distancia[i];
    j++;
    resultado[j] = anterior[i];
    j++;
  }
  free(distancia);
  return resultado;
}

//printar todo o resultado do dijkstra
void printAll(int* parcial, int N){
  for (int i=0;i<N*3; i++) {
    if (i%3==0) {
      printf("%i: ",parcial[i]+1);
    }
    else {
      printf("%i, ", parcial[i]);
      i++;
      if (parcial[i] == -1) {
        printf("%i", parcial[i]);
      }
      else {
        printf("%i", parcial[i]+1);
      }
    }
    if ((i+1)%3 == 0) {
      printf("\n");
    }
  }
}

// função para comparar dois resultados, escolher o melhor e retornar o array de NO
NO * comparar(int * parcial, int * parcialComChave, int fim, int chave){
  NO * temp;
  temp = NULL;
  int atual = fim-1;
  if (parcial[(fim-1)*3+1] <= parcialComChave[(fim-1)*3+1]+parcial[(chave-1)*3+1]) {
    while (atual != -1) {
      NO * novo = (NO*) malloc(sizeof(NO));
      novo->adj = atual+1;
      novo->peso = parcial[atual*3+1];
      novo->prox = temp;
      temp = novo;
      atual = parcial[atual*3+2];
    }
  }
  else {
    while (atual != chave-1) {
      NO * novo = (NO*) malloc(sizeof(NO));
      novo->adj = atual + 1;
      novo->peso = parcialComChave[atual*3-1]+parcial[(chave-1)*3+1];
      novo->prox = temp;
      temp = novo;
      atual = parcialComChave[atual*3+2];
    }
    atual = chave - 1;
    while (atual != -1) {
      NO * novo = (NO*) malloc(sizeof(NO));
      novo->adj = atual+1;
      novo->peso = parcial[atual*3+1];
      novo->prox = temp;
      temp = novo;
      atual = parcial[atual*3+2];
    }
  }
  if (temp->prox == NULL) {
    return NULL;
  }
  return temp;
}

NO * caminho(int N, int A, int *ijpeso, int *aberto, int inicio, int fim, int chave)
{
  VERTICE* grafo = criarGrafo(N, A, ijpeso, aberto);
  NO* resp = comparar(dijkstra(grafo, N, inicio-1, aberto, true), dijkstra(grafo, N, chave-1, aberto, false), fim, chave);

  return resp;
}



int main() {


  // aqui vc pode incluir codigo de teste

  // exemplo de teste trivial

  int N=9; // grafo de 9 vértices numerados de 1..9
  int A=10; // grafo de 10 arestas
  int aberto[] = {0,1,1,1,1,1,1,1,1}; // vértice 1 bloqueado
  int inicio=7;
  int fim=4;
  int chave=6;
  int ijpeso[]={1,2,30, 1,3,20, 2,6,20, 2,7,30, 3,7,80, 5,8,10, 6,7,10, 7,9,80, 4,3,20, 4,9,80};

  // o EP sera testado com uma serie de chamadas como esta
  NO * teste = NULL;
  teste = caminho(N, A, ijpeso, aberto, inicio, fim, chave);
  printf("Caminho = [");
  if (teste) {
    while (teste) {
      printf(" %i,", teste->adj);
      teste=teste->prox;
    }
  }
  else {
    printf(" NULL");
  }
  printf(" ]\n");
  return 0;
}

// por favor nao inclua nenhum código abaixo da função main()

