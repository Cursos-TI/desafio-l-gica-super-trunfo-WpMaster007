/*
  torre.c
  Módulo de priorização e montagem de componentes da torre de fuga.

  Compilar:
    gcc -std=c11 -Wall -Wextra -o torre torre.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COMPONENTES 20
#define MAX_NOME 30
#define MAX_TIPO 20

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade;   // 1..10
} Componente;

/* ---------- Prototypes ---------- */

/* Leitura segura de string (fgets + trim newline) */
void lerString(char *buf, int tam);

/* Exibe componentes em formato de tabela */
void mostrarComponentes(const Componente arr[], int n);

/* Algoritmos de ordenação (cada um conta comparações via ponteiro comparacoes) */
void bubbleSortNome(Componente arr[], int n, long *comparacoes);
void insertionSortTipo(Componente arr[], int n, long *comparacoes);
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes);

/* Busca binária por nome (requer vetor ordenado por nome). Retorna índice ou -1. */
int buscaBinariaPorNome(const Componente arr[], int n, const char chave[], long *comparacoes);

/* Função utilitária para medir tempo de execução de um algoritmo de ordenação */
typedef void (*SortFunc)(Componente[], int, long*);
double medirTempoSort(SortFunc f, Componente arr[], int n, long *comparacoes);

/* Cópia de vetor */
void copiarVet(Componente dst[], const Componente src[], int n);

/* Função para limpar buffer (quando necessário) */
void limparBufferStdin(void);

/* ---------- Implementations ---------- */

void lerString(char *buf, int tam) {
    if (!buf) return;
    if (fgets(buf, tam, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0'; // remove newline
}

void mostrarComponentes(const Componente arr[], int n) {
    if (n <= 0) {
        printf("(Nenhum componente cadastrado)\n");
        return;
    }
    printf("\nLista de componentes (total %d):\n", n);
    printf("Idx  Nome                          Tipo               Prioridade\n");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printf("%2d   %-28s %-18s %2d\n", i, arr[i].nome, arr[i].tipo, arr[i].prioridade);
    }
    printf("\n");
}

/* --- Bubble sort por nome (strings). Conta comparações de strcmp. --- */
void bubbleSortNome(Componente arr[], int n, long *comparacoes) {
    if (comparacoes) *comparacoes = 0;
    for (int i = 0; i < n - 1; ++i) {
        int trocou = 0;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (comparacoes) (*comparacoes)++;
            if (strcmp(arr[j].nome, arr[j+1].nome) > 0) {
                Componente tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

/* --- Insertion sort por tipo (strings). Conta comparações de strcmp. --- */
void insertionSortTipo(Componente arr[], int n, long *comparacoes) {
    if (comparacoes) *comparacoes = 0;
    for (int i = 1; i < n; ++i) {
        Componente chave = arr[i];
        int j = i - 1;
        /* Comparar tipos: cada strcmp conta 1 comparação */
        while (j >= 0) {
            if (comparacoes) (*comparacoes)++;
            if (strcmp(arr[j].tipo, chave.tipo) > 0) {
                arr[j+1] = arr[j];
                j--;
            } else break;
        }
        arr[j+1] = chave;
    }
}

/* --- Selection sort por prioridade (int). Conta comparações de inteiro. --- */
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes) {
    if (comparacoes) *comparacoes = 0;
    for (int i = 0; i < n - 1; ++i) {
        int idxMin = i;
        for (int j = i + 1; j < n; ++j) {
            if (comparacoes) (*comparacoes)++;
            if (arr[j].prioridade < arr[idxMin].prioridade) idxMin = j;
        }
        if (idxMin != i) {
            Componente tmp = arr[i];
            arr[i] = arr[idxMin];
            arr[idxMin] = tmp;
        }
    }
}

/* --- Busca binária por nome.
       Conta comparações: cada strcmp com o elemento do meio conta 1. --- */
int buscaBinariaPorNome(const Componente arr[], int n, const char chave[], long *comparacoes) {
    int low = 0, high = n - 1;
    if (comparacoes) *comparacoes = 0;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (comparacoes) (*comparacoes)++;
        int cmp = strcmp(chave, arr[mid].nome);
        if (cmp == 0) return mid;
        else if (cmp < 0) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

/* Mede tempo de execução do algoritmo de ordenação (em segundos).
   Também escreve em *comparacoes o número de comparações que o algoritmo registrou. */
double medirTempoSort(SortFunc f, Componente arr[], int n, long *comparacoes) {
    clock_t inicio = clock();
    f(arr, n, comparacoes);
    clock_t fim = clock();
    return ((double)(fim - inicio)) / CLOCKS_PER_SEC;
}

void copiarVet(Componente dst[], const Componente src[], int n) {
    for (int i = 0; i < n; ++i) dst[i] = src[i];
}

void limparBufferStdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ---------- Função main: interface e fluxo ---------- */
int main(void) {
    Componente orig[MAX_COMPONENTES];   // vetor original (como o jogador cadastrou)
    Componente trabalho[MAX_COMPONENTES]; // vetor de trabalho onde se aplicam ordenações
    int n = 0; // quantidade cadastrada

    int op;
    int sortedByName = 0; // flags sobre o estado do vetor de trabalho
    int sortedByType = 0;
    int sortedByPriority = 0;
    long lastComparacoes = 0;
    double lastTempo = 0.0;

    srand((unsigned)time(NULL));

    printf("=== Módulo de Priorizacao e Montagem da Torre de Fuga ===\n\n");

    do {
        printf("Menu:\n");
        printf("1 - Cadastrar componente (max %d)\n", MAX_COMPONENTES);
        printf("2 - Mostrar componentes cadastrados\n");
        printf("3 - Ordenar componentes (escolha algoritmo)\n");
        printf("4 - Buscar componente-chave por nome (apenas se ordenado por nome)\n");
        printf("5 - Montagem final / confirmar componente-chave\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        if (scanf("%d", &op) != 1) {
            printf("Entrada invalida. Encerrando.\n");
            break;
        }
        limparBufferStdin();

        if (op == 1) {
            if (n >= MAX_COMPONENTES) {
                printf("Limite atingido (%d componentes). Remova ou continue.\n", MAX_COMPONENTES);
                continue;
            }
            Componente c;
            printf("Nome do componente: ");
            lerString(c.nome, MAX_NOME);
            if (strlen(c.nome) == 0) {
                printf("Nome vazio. Cadastro cancelado.\n");
                continue;
            }
            printf("Tipo do componente (ex: controle, suporte, propulsao): ");
            lerString(c.tipo, MAX_TIPO);
            if (strlen(c.tipo) == 0) {
                printf("Tipo vazio. Cadastro cancelado.\n");
                continue;
            }
            int prio = 0;
            do {
                printf("Prioridade (1..10): ");
                if (scanf("%d", &prio) != 1) {
                    limparBufferStdin();
                    prio = 0;
                    printf("Entrada invalida. Tente novamente.\n");
                } else {
                    if (prio < 1 || prio > 10) {
                        printf("Prioridade deve ser entre 1 e 10.\n");
                    }
                }
            } while (prio < 1 || prio > 10);
            limparBufferStdin();
            c.prioridade = prio;
            orig[n++] = c;
            printf("Componente cadastrado com sucesso.\n\n");
        }
        else if (op == 2) {
            mostrarComponentes(orig, n);
        }
        else if (op == 3) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            printf("\nEscolha o algoritmo de ordenacao:\n");
            printf("1 - Bubble sort por NOME (string)\n");
            printf("2 - Insertion sort por TIPO (string)\n");
            printf("3 - Selection sort por PRIORIDADE (int)\n");
            printf("Opcao: ");
            int alg = 0;
            if (scanf("%d", &alg) != 1) { limparBufferStdin(); printf("Entrada invalida.\n"); continue; }
            limparBufferStdin();

            /* Copia original para vetor de trabalho para preservar originais (permite comparar vários algoritmos) */
            copiarVet(trabalho, orig, n);

            lastComparacoes = 0;
            lastTempo = 0.0;
            sortedByName = sortedByType = sortedByPriority = 0;

            if (alg == 1) {
                lastTempo = medirTempoSort(bubbleSortNome, trabalho, n, &lastComparacoes);
                sortedByName = 1;
                printf("\nResultado: bubble sort por NOME concluido.\n");
            } else if (alg == 2) {
                lastTempo = medirTempoSort(insertionSortTipo, trabalho, n, &lastComparacoes);
                sortedByType = 1;
                printf("\nResultado: insertion sort por TIPO concluido.\n");
            } else if (alg == 3) {
                lastTempo = medirTempoSort(selectionSortPrioridade, trabalho, n, &lastComparacoes);
                sortedByPriority = 1;
                printf("\nResultado: selection sort por PRIORIDADE concluido.\n");
            } else {
                printf("Algoritmo invalido.\n");
                continue;
            }

            /* Exibe estatísticas e vetor ordenado (no trabalho) */
            printf("Comparacoes realizadas: %ld\n", lastComparacoes);
            printf("Tempo de execucao: %.6f s\n", lastTempo);
            mostrarComponentes(trabalho, n);

            printf("Observacao: o vetor original (cadastrado) foi preservado. O vetor de trabalho contem a ordenacao para analise.\n\n");
        }
        else if (op == 4) {
            if (!sortedByName) {
                printf("Busca binaria exige ordenacao por NOME (bubble sort). Ordenar por nome primeiro.\n");
                continue;
            }
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            char chave[MAX_NOME];
            printf("Digite o NOME exato do componente a buscar: ");
            lerString(chave, MAX_NOME);
            if (strlen(chave) == 0) { printf("Nome vazio. Abortando busca.\n"); continue; }

            long comps = 0;
            int idx = buscaBinariaPorNome(trabalho, n, chave, &comps);
            printf("Comparacoes na busca binaria: %ld\n", comps);
            if (idx >= 0) {
                printf("Componente encontrado no indice %d:\n", idx);
                printf("  Nome: %s\n  Tipo: %s\n  Prioridade: %d\n\n", trabalho[idx].nome, trabalho[idx].tipo, trabalho[idx].prioridade);
            } else {
                printf("Componente NAO encontrado.\n\n");
            }
        }
        else if (op == 5) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            /* Exibe o vetor de trabalho se já existe ordenacao; caso contrario exibe original */
            if (sortedByName || sortedByType || sortedByPriority) {
                printf("Vetor de trabalho (ordenado conforme ultima operacao):\n");
                mostrarComponentes(trabalho, n);
            } else {
                printf("Nenhuma ordenacao feita ainda. Mostrando vetor original:\n");
                mostrarComponentes(orig, n);
            }

            /* Pergunta pelo componente-chave que destrava a torre */
            char chave[MAX_NOME];
            printf("Digite o NOME do componente-chave para confirmar (ex: chip central): ");
            lerString(chave, MAX_NOME);
            if (strlen(chave) == 0) { printf("Nome vazio. Abortando montagem.\n"); continue; }

            /* Se estiver ordenado por nome, faça busca binária e mostre comparações.
               Caso contrário, faça busca linear (contando comparações) */
            if (sortedByName) {
                long comps = 0;
                int idx = buscaBinariaPorNome(trabalho, n, chave, &comps);
                printf("Busca binaria (vetor ordenado por nome): comparacoes = %ld\n", comps);
                if (idx >= 0) {
                    printf("Componente-chave PRESENTE. Montagem da torre pode proceder!\n");
                } else {
                    printf("Componente-chave NAO PRESENTE. Verifique lista e tente novamente.\n");
                }
            } else {
                /* busca linear no vetor apresentado (se ordenado, trabalho; senão, orig) */
                const Componente *arr = (sortedByType || sortedByPriority) ? trabalho : orig;
                long comps = 0;
                int found = 0;
                for (int i = 0; i < n; ++i) {
                    comps++;
                    if (strcmp(arr[i].nome, chave) == 0) { found = 1; break; }
                }
                printf("Busca linear: comparacoes = %ld\n", comps);
                if (found) printf("Componente-chave PRESENTE. Montagem da torre pode proceder!\n");
                else printf("Componente-chave NAO PRESENTE. Verifique lista e tente novamente.\n");
            }
            printf("\n");
        }
        else if (op == 0) {
            printf("Saindo...\n");
        }
        else {
            printf("Opcao desconhecida.\n");
        }

    } while (op != 0);

    return 0;
}
