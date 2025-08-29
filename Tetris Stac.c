    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #define TAM_FILA 5
    #define TAM_PILHA 3

    typedef struct {
        char nome;
        int id;
    } Peca;

    typedef struct {
        Peca itens[TAM_FILA];
        int inicio, fim, qtd;
    } Fila;

    typedef struct {
        Peca itens[TAM_PILHA];
        int topo;
    } Pilha;

    char tipos[] = {'I','O','T','L'};
    int contadorId = 1;

    Peca gerarPeca() {
        Peca nova;
        nova.nome = tipos[rand() % 4];
        nova.id = contadorId++;
        return nova;
    }

    void inicializarFila(Fila *f) { f->inicio = 0; f->fim = -1; f->qtd = 0; }
    int filaCheia(Fila *f) { return f->qtd == TAM_FILA; }
    int filaVazia(Fila *f) { return f->qtd == 0; }

void enfileirar(Fila *f, Peca p) {
    if(filaCheia(f)) return;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->itens[f->fim] = p;
    f->qtd++;
}

Peca desenfileirar(Fila *f) {
    Peca removido = {'-', -1};
    if(filaVazia(f)) return removido;
    removido = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->qtd--;
    return removido;
}

void inicializarPilha(Pilha *p) { p->topo = -1; }
int pilhaCheia(Pilha *p) { return p->topo == TAM_PILHA-1; }
int pilhaVazia(Pilha *p) { return p->topo == -1; }

void empilhar(Pilha *p, Peca x) { if(!pilhaCheia(p)) p->itens[++p->topo] = x; }
Peca desempilhar(Pilha *p) { Peca r={'-',-1}; if(!pilhaVazia(p)) r=p->itens[p->topo--]; return r; }

void jogarPeca(Fila *f) {
    if(filaVazia(f)) return;
    Peca jogada = desenfileirar(f);
    printf("Jogou %c[%d]\n", jogada.nome, jogada.id);
    enfileirar(f, gerarPeca());
}

void reservarPeca(Fila *f, Pilha *p) {
    if(filaVazia(f) || pilhaCheia(p)) return;
    Peca reservada = desenfileirar(f);
    empilhar(p, reservada);
    printf("Reservou %c[%d]\n", reservada.nome, reservada.id);
    enfileirar(f, gerarPeca());
}

void usarReservada(Pilha *p) {
    if(pilhaVazia(p)) return;
    Peca usada = desempilhar(p);
    printf("Usou %c[%d]\n", usada.nome, usada.id);
}

void trocarAtual(Fila *f, Pilha *p) {
    if(filaVazia(f) || pilhaVazia(p)) return;
    Peca aux = f->itens[f->inicio];
    f->itens[f->inicio] = p->itens[p->topo];
    p->itens[p->topo] = aux;
    printf("Troca realizada\n");
}

void trocaMultipla(Fila *f, Pilha *p) {
    if(f->qtd < 3 || p->topo < 2) return;
    for(int i=0;i<3;i++) {
        int pos = (f->inicio+i) % TAM_FILA;
        Peca aux = f->itens[pos];
        f->itens[pos] = p->itens[p->topo - i];
        p->itens[p->topo - i] = aux;
    }
    printf("Troca múltipla realizada\n");
}

void exibirEstado(Fila *f, Pilha *p) {
    printf("\nFila: ");
    for(int i=0;i<f->qtd;i++) {
        int pos = (f->inicio+i) % TAM_FILA;
        printf("%c[%d] ", f->itens[pos].nome, f->itens[pos].id);
    }
    printf("\nPilha: ");
    for(int i=0;i<=p->topo;i++) {
        printf("%c[%d] ", p->itens[i].nome, p->itens[i].id);
    }
    printf("\n\n");
}

int main() {
    Fila fila; Pilha pilha;
    inicializarFila(&fila); inicializarPilha(&pilha);
    srand(time(NULL));
    for(int i=0;i<TAM_FILA;i++) enfileirar(&fila, gerarPeca());

    int opcao;
    do {
        exibirEstado(&fila,&pilha);
        printf("1-Jogar  2-Reservar  3-Usar reservada  4-Trocar atual  5-Troca múltipla  0-Sair\n");
        scanf("%d",&opcao);
        switch(opcao) {
            case 1: jogarPeca(&fila); break;
            case 2: reservarPeca(&fila,&pilha); break;
            case 3: usarReservada(&pilha); break;
            case 4: trocarAtual(&fila,&pilha); break;
            case 5: trocaMultipla(&fila,&pilha); break;
        }
    } while(opcao!=0);

    return 0;
}