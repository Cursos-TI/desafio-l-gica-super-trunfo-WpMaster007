#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_MAPA 6
#define MAX_MISSAO_LEN 100

/* Estrutura do território */
typedef struct {
    char nome[30];
    char cor[10];   // e.g., "Red", "Blue"
    int tropas;
} Territorio;

/* Prototypes das funções (modularização) */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes, const char* corJogador);
int verificarMissao(const char* missao, const char* corJogador, Territorio* mapa, int tamanho);
void exibirMissao(const char* missao);
void atacar(Territorio* atacante, Territorio* defensor);
void exibirMapa(Territorio* mapa, int tamanho);
void liberarMemoria(Territorio** mapa, char** missao1, char** missao2);

/* Função que sorteia e copia a missão (usa strcpy conforme requisito) */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes, const char* corJogador) {
    if (destino == NULL || missoes == NULL || totalMissoes <= 0) return;
    int idx;
    do {
        idx = rand() % totalMissoes;
        /* Evita atribuir M3 ao jogador da cor Red */
        if (strncmp(missoes[idx], "M3:", 3) == 0 && strcmp(corJogador, "Red") == 0) {
            continue;
        }
        break;
    } while (1);
    strcpy(destino, missoes[idx]);
}

/* Verifica se a missão foi cumprida.
   Interpreta missões baseando-se em prefixos "M1:", "M2:", etc. */
int verificarMissao(const char* missao, const char* corJogador, Territorio* mapa, int tamanho) {
    if (!missao || !corJogador || !mapa) return 0;

    /* M1: Conquistar 3 territórios seguidos (3 vizinhos no vetor) */
    /* NOTA: Assume-se que territórios consecutivos no vetor são vizinhos */
    if (strncmp(missao, "M1:", 3) == 0) {
        int consec = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                consec++;
                if (consec >= 3) return 1;
            } else {
                consec = 0;
            }
        }
        return 0;
    }

    /* M2: Controlar 4 territórios no total */
    if (strncmp(missao, "M2:", 3) == 0) {
        int cnt = 0;
        for (int i = 0; i < tamanho; ++i)
            if (strcmp(mapa[i].cor, corJogador) == 0) cnt++;
        return (cnt >= 4) ? 1 : 0;
    }

    /* M3: Eliminar todas as tropas da cor Red */
    if (strncmp(missao, "M3:", 3) == 0) {
        int totalRed = 0;
        for (int i = 0; i < tamanho; ++i)
            if (strcmp(mapa[i].cor, "Red") == 0) totalRed += mapa[i].tropas;
        return (totalRed == 0) ? 1 : 0;
    }

    /* M4: Ter pelo menos 1 território com 10 ou mais tropas */
    if (strncmp(missao, "M4:", 3) == 0) {
        for (int i = 0; i < tamanho; ++i)
            if (strcmp(mapa[i].cor, corJogador) == 0 && mapa[i].tropas >= 10) return 1;
        return 0;
    }

    /* M5: Controlar os territórios A e B */
    if (strncmp(missao, "M5:", 3) == 0) {
        int controlaA = 0, controlaB = 0;
        for (int i = 0; i < tamanho; ++i) {
            if (strcmp(mapa[i].nome, "A") == 0 && strcmp(mapa[i].cor, corJogador) == 0) controlaA = 1;
            if (strcmp(mapa[i].nome, "B") == 0 && strcmp(mapa[i].cor, corJogador) == 0) controlaB = 1;
        }
        return (controlaA && controlaB) ? 1 : 0;
    }

    return 0;
}

/* Exibe missão (passagem por valor - só leitura) */
void exibirMissao(const char* missao) {
    if (!missao) return;
    printf("Missão sorteada: %s\n", missao);
}

/* Simula um ataque entre dois territórios */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (!atacante || !defensor) return;

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;
    printf("Rolagem: Atacante %d x Defensor %d\n", dadoA, dadoD);

    if (dadoA > dadoD) {
        /* Atacante vence: transfere cor e metade das tropas */
        int metade = atacante->tropas / 2;
        if (metade < 1) metade = 1; /* Garante ao menos 1 tropa transferida */
        if (atacante->tropas - metade < 1) metade = atacante->tropas - 1; /* Garante 1 tropa no atacante */
        printf("Atacante venceu! Transferindo cor e %d tropas para defensor.\n", metade);
        atacante->tropas -= metade;
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = metade; /* Defensor recebe apenas as tropas transferidas */
    } else {
        /* Atacante perde uma tropa */
        if (atacante->tropas > 0) atacante->tropas -= 1;
        printf("Atacante perdeu 1 tropa.\n");
    }

    if (atacante->tropas < 0) atacante->tropas = 0;
    if (defensor->tropas < 0) defensor->tropas = 0;
}

/* Mostra o mapa (lista de territórios) */
void exibirMapa(Territorio* mapa, int tamanho) {
    printf("\nMAPA ATUAL:\n");
    printf("Idx  Nome   Cor    Tropas\n");
    printf("--------------------------------\n");
    for (int i = 0; i < tamanho; ++i) {
        printf("%2d   %-5s  %-5s   %2d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("\n");
}

/* Libera memória alocada dinamicamente */
void liberarMemoria(Territorio** mapa, char** missao1, char** missao2) {
    if (*mapa) {
        free(*mapa);
        *mapa = NULL;
    }
    if (*missao1) {
        free(*missao1);
        *missao1 = NULL;
    }
    if (*missao2) {
        free(*missao2);
        *missao2 = NULL;
    }
}

/* Função principal: fluxo do jogo */
int main(void) {
    srand((unsigned)time(NULL));

    /* Vetor de missões */
    char* missoes[] = {
        "M1: Conquistar 3 territórios seguidos",
        "M2: Controlar 4 territórios",
        "M3: Eliminar todas as tropas da cor Red",
        "M4: Ter pelo menos 1 território com 10 tropas",
        "M5: Controlar os territórios A e B"
    };
    int totalMissoes = sizeof(missoes) / sizeof(missoes[0]);

    /* Aloca mapa dinamicamente */
    Territorio* mapa = (Territorio*)calloc(TAM_MAPA, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro de alocação do mapa\n");
        return 1;
    }

    /* Inicializa nomes, cores alternadas e tropas aleatórias */
    const char* nomesInit[TAM_MAPA] = {"A", "B", "C", "D", "E", "F"};
    for (int i = 0; i < TAM_MAPA; ++i) {
        strncpy(mapa[i].nome, nomesInit[i], sizeof(mapa[i].nome) - 1);
        mapa[i].nome[sizeof(mapa[i].nome) - 1] = '\0';
        if (i % 2 == 0) strcpy(mapa[i].cor, "Red");
        else strcpy(mapa[i].cor, "Blue");
        mapa[i].tropas = (rand() % 5) + 1; /* 1 a 5 tropas iniciais */
    }

    /* Aloca dinamicamente as strings de missão */
    char* missaoJogador1 = (char*)malloc(MAX_MISSAO_LEN);
    char* missaoJogador2 = (char*)malloc(MAX_MISSAO_LEN);
    if (!missaoJogador1 || !missaoJogador2) {
        fprintf(stderr, "Erro de alocação das missões\n");
        liberarMemoria(&mapa, &missaoJogador1, &missaoJogador2);
        return 1;
    }

    /* Sorteia missão para cada jogador */
    char corJogador1[] = "Red";
    char corJogador2[] = "Blue";
    atribuirMissao(missaoJogador1, missoes, totalMissoes, corJogador1);
    atribuirMissao(missaoJogador2, missoes, totalMissoes, corJogador2);

    /* Exibir missão apenas uma vez */
    printf("Jogador 1 (%s):\n", corJogador1);
    exibirMissao(missaoJogador1);
    printf("\nJogador 2 (%s):\n", corJogador2);
    exibirMissao(missaoJogador2);

    /* Loop principal do jogo */
    int vez = 1; /* 1 -> jogador1, 2 -> jogador2 */
    int encerrado = 0;
    while (!encerrado) {
        exibirMapa(mapa, TAM_MAPA);

        /* Verificar missões no início do turno */
        if (verificarMissao(missaoJogador1, corJogador1, mapa, TAM_MAPA)) {
            printf("Jogador 1 (cor %s) cumpriu a missão e vence!\n", corJogador1);
            break;
        }
        if (verificarMissao(missaoJogador2, corJogador2, mapa, TAM_MAPA)) {
            printf("Jogador 2 (cor %s) cumpriu a missão e vence!\n", corJogador2);
            break;
        }

        printf("Vez do jogador %d (%s)\n", vez, (vez == 1) ? corJogador1 : corJogador2);
        printf("Escolha ação: 1-Ataque  0-Sair\n");
        int acao = 0;
        if (scanf("%d", &acao) != 1) {
            while (getchar() != '\n'); /* Limpa buffer */
            printf("Entrada inválida. Tente novamente.\n");
            continue; /* Continua o loop em vez de encerrar */
        }
        if (acao == 0) {
            printf("Jogo encerrado pelo usuário.\n");
            break;
        } else if (acao == 1) {
            int idxAt, idxDef;
            printf("Escolha índice do território atacante: ");
            if (scanf("%d", &idxAt) != 1) {
                while (getchar() != '\n');
                printf("Índice inválido. Tente novamente.\n");
                continue;
            }
            printf("Escolha índice do território defensor: ");
            if (scanf("%d", &idxDef) != 1) {
                while (getchar() != '\n');
                printf("Índice inválido. Tente novamente.\n");
                continue;
            }

            /* Valida índices */
            if (idxAt < 0 || idxAt >= TAM_MAPA || idxDef < 0 || idxDef >= TAM_MAPA) {
                printf("Índices inválidos.\n");
            } else if (idxAt == idxDef) {
                printf("Atacante e defensor são o mesmo.\n");
            } else {
                Territorio* at = &mapa[idxAt];
                Territorio* def = &mapa[idxDef];
                const char* corAtualJogador = (vez == 1) ? corJogador1 : corJogador2;
                if (strcmp(at->cor, corAtualJogador) != 0) {
                    printf("Você só pode atacar com territórios da sua cor (%s).\n", corAtualJogador);
                } else if (strcmp(def->cor, corAtualJogador) == 0) {
                    printf("Não é permitido atacar seu próprio território.\n");
                } else if (at->tropas <= 1) { /* Garante pelo menos 1 tropa para permanecer */
                    printf("Território atacante precisa de pelo menos 2 tropas para atacar.\n");
                } else {
                    atacar(at, def);
                }
            }
        } else {
            printf("Ação desconhecida.\n");
        }

        /* Verificar missões ao final do turno */
        if (verificarMissao(missaoJogador1, corJogador1, mapa, TAM_MAPA)) {
            printf("Jogador 1 (cor %s) cumpriu a missão e vence!\n", corJogador1);
            break;
        }
        if (verificarMissao(missaoJogador2, corJogador2, mapa, TAM_MAPA)) {
            printf("Jogador 2 (cor %s) cumpriu a missão e vence!\n", corJogador2);
            break;
        }

        vez = (vez == 1) ? 2 : 1; /* Alterna vez */
    }

    liberarMemoria(&mapa, &missaoJogador1, &missaoJogador2);
    return 0;
}
