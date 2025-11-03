// ============================================================================
//         PROJETO DETECTIVE QUEST - DESAFIO DE CÓDIGO (NÍVEL MESTRE)
// ============================================================================
//
// OBJETIVOS (VERSÃO FINAL):
// - Integrar árvore binária da mansão, BST de pistas e tabela hash de suspeitos.
// - Coletar pistas durante a exploração e relacioná-las automaticamente.
// - Permitir acusação final apoiada por evidências (mínimo 2 pistas).
//
// AUTOR: Diego Bloise
// DATA: Novembro de 2025
// LINGUAGEM: C (ANSI C - padrão C99)
// COMPILAÇÃO: gcc detective_quest.c -o detective_quest
// EXECUÇÃO:   ./detective_quest
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// ============================================================================
//                            CONFIGURAÇÕES E CONSTANTES
// ============================================================================

/** Tamanho do bucket da tabela hash (fixo para simplicidade). */
#define TAM_HASH 11

/** Tamanho máximo para nomes e textos (ajustável). */
#define MAX_NOME 64
#define MAX_PISTA 128

// ============================================================================
//                            ESTRUTURAS DE DADOS
// ============================================================================

/**
 * @struct Sala
 * @brief Nó da árvore binária que representa um cômodo da mansão.
 *
 * Cada Sala possui:
 *  - nome: identificador do cômodo
 *  - pista: string com a pista associada (pode ser vazia)
 *  - esquerda / direita: ponteiros para os cômodos adjacentes
 */
typedef struct Sala {
    char nome[MAX_NOME];           /**< Nome do cômodo (ex: "Cozinha") */
    char pista[MAX_PISTA];         /**< Pista associada (opcional) */
    struct Sala* esquerda;         /**< Sala à esquerda (NULL se não existir) */
    struct Sala* direita;          /**< Sala à direita (NULL se não existir) */
} Sala;

/**
 * @struct PistaNode
 * @brief Nó da BST (árvore de busca binária) que armazena pistas coletadas.
 *
 * A BST organiza as pistas em ordem alfabética (strcmp).
 */
typedef struct PistaNode {
    char pista[MAX_PISTA];         /**< Texto da pista */
    struct PistaNode* esquerda;    /**< Ponteiro para subárvore esquerda (menores) */
    struct PistaNode* direita;     /**< Ponteiro para subárvore direita (maiores) */
} PistaNode;

/**
 * @struct SuspeitoNode
 * @brief Nó de lista encadeada usado em cada bucket da tabela hash.
 *
 * Cada nó armazena uma associação: pista -> suspeito.
 * A tabela hash é um vetor de ponteiros para listas encadeadas.
 */
typedef struct SuspeitoNode {
    char pista[MAX_PISTA];         /**< Texto da pista (chave) */
    char suspeito[MAX_NOME];       /**< Nome do suspeito (valor) */
    struct SuspeitoNode* prox;     /**< Próximo nó na lista (colisão) */
} SuspeitoNode;

// ============================================================================
//                            PROTÓTIPOS (Doxygen)
// ============================================================================

/* ----------------- Criação e exploração da mansão ----------------- */

/**
 * @brief Cria dinamicamente uma sala com nome e pista associada.
 *
 * Aloca memória, copia o nome e a pista, e inicializa ponteiros.
 *
 * @param nome Nome do cômodo (string).
 * @param pista Texto da pista associada ao cômodo (pode ser "").
 * @return Ponteiro para a Sala criada (não-NULL). Em falha, finaliza o programa.
 */
Sala* criarSala(const char* nome, const char* pista);

/**
 * @brief Explora a mansão interativamente a partir de uma sala inicial.
 *
 * A cada sala visitada, se houver pista não-vazia, ela é automaticamente
 * inserida na BST de pistas e relacionada na tabela hash (pista→suspeito).
 *
 * Comandos de navegação:
 *  - 'e' / 'E' : esquerda
 *  - 'd' / 'D' : direita
 *  - 's' / 'S' : encerrar exploração
 *
 * @param atual Ponteiro para a sala inicial (raiz da exploração).
 * @param raizPistas Endereço do ponteiro para a raiz da BST de pistas coletadas.
 * @param tabela Tabela hash (vetor de ponteiros para SuspeitoNode) já inicializada.
 */
void explorarMansao(Sala* atual, PistaNode** raizPistas, SuspeitoNode* tabela[]);

/* ----------------- BST de pistas ----------------- */

/**
 * @brief Insere uma pista na BST (mantendo ordem alfabética).
 *
 * Não insere duplicatas idênticas (comparação por strcmp).
 *
 * @param raiz Ponteiro para a raiz atual da BST.
 * @param pista Texto da pista a inserir.
 * @return Ponteiro atualizado para a raiz da BST.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista);

/**
 * @brief Exibe todas as pistas armazenadas na BST, em ordem alfabética.
 *
 * Percurso in-order recursivo.
 *
 * @param raiz Ponteiro para a raiz da BST de pistas.
 */
void exibirPistas(PistaNode* raiz);

/**
 * @brief Libera recursivamente toda a memória alocada pela BST de pistas.
 *
 * @param raiz Ponteiro para a raiz da BST.
 */
void liberarPistas(PistaNode* raiz);

/* ----------------- Tabela hash e suspeitos ----------------- */

/**
 * @brief Inicializa a tabela hash, setando todos os buckets para NULL.
 *
 * @param tabela Vetor (size TAM_HASH) de ponteiros para SuspeitoNode.
 */
void inicializarHash(SuspeitoNode* tabela[]);

/**
 * @brief Função hash simples — transforma a string (pista) em índice.
 *
 * Implementação: soma dos bytes (unsigned char) mod TAM_HASH.
 *
 * @param pista Texto da pista.
 * @return Índice no intervalo [0, TAM_HASH-1].
 */
int hash(const char* pista);

/**
 * @brief Insere uma associação (pista -> suspeito) na tabela hash.
 *
 * Trata colisões por inserção no início da lista encadeada do bucket.
 *
 * @param tabela Tabela hash previamente inicializada.
 * @param pista Texto da pista (chave).
 * @param suspeito Nome do suspeito (valor).
 */
void inserirNaHash(SuspeitoNode* tabela[], const char* pista, const char* suspeito);

/**
 * @brief Consulta a tabela hash buscando o suspeito ligado a uma pista.
 *
 * Se a pista não estiver presente, retorna a string literal "Desconhecido".
 *
 * @param tabela Tabela hash.
 * @param pista Texto da pista buscada.
 * @return Ponteiro para o nome do suspeito (string estática ou do nó).
 */
const char* encontrarSuspeito(SuspeitoNode* tabela[], const char* pista);

/**
 * @brief Libera toda a memória alocada na tabela hash (listas encadeadas).
 *
 * @param tabela Tabela hash.
 */
void liberarHash(SuspeitoNode* tabela[]);

/* ----------------- Verificação final / utilitários ----------------- */

/**
 * @brief Conta, entre as pistas coletadas (BST), quantas apontam para um suspeito.
 *
 * Percorre a BST e, para cada pista, consulta a tabela hash para recuperar
 * o suspeito associado — se coincidir com o nome passado, incrementa contador.
 *
 * @param tabela Tabela hash (pista→suspeito).
 * @param raizPistas Raiz da BST com as pistas coletadas.
 * @param suspeito Nome do suspeito a ser verificado.
 * @return Quantidade de pistas coletadas que apontam para o suspeito.
 */
int contarPistasPorSuspeitoNaBST(SuspeitoNode* tabela[], PistaNode* raizPistas, const char* suspeito);

/**
 * @brief Fase de julgamento: solicita acusação e verifica evidências.
 *
 * Regras: acusação é considerada consistente se houver pelo menos 2 pistas
 * coletadas que apontem para o suspeito acusado.
 *
 * @param tabela Tabela hash de associações.
 * @param raizPistas Raiz da BST com as pistas coletadas.
 */
void verificarSuspeitoFinal(SuspeitoNode* tabela[], PistaNode* raizPistas);

/**
 * @brief Limpa o buffer de entrada (stdin) para evitar lixo em leituras.
 */
void limparBufferEntrada(void);

/**
 * @brief Limpa o terminal (compatível Windows / Unix).
 */
void limparTela(void);

/**
 * @brief Libera toda a memória da árvore de salas (mansão).
 *
 * Percorre recursivamente e libera cada nó (Sala).
 *
 * @param raiz Ponteiro para a raiz da árvore de salas.
 */
void liberarMansao(Sala* raiz);

// ============================================================================
//                                MAIN
// ============================================================================

/**
 * @brief Ponto de entrada do programa.
 *
 * Constrói o mapa fixo da mansão, inicializa a tabela hash com associações
 * pista→suspeito, conduz a exploração interativa, exibe pistas e realiza
 * a fase final de acusação.
 */
int main(void) {
    setlocale(LC_ALL, "Portuguese");

    limparTela();
    printf("========================================================\n");
    printf("  DETECTIVE QUEST - NÍVEL MESTRE (Investigaçã o Final)\n");
    printf("========================================================\n\n");

    /* -----------------------------
     * Montagem fixa da mansão (árvore)
     * -----------------------------
     *
     *                 [Hall de Entrada]
     *                   /           \
     *            [Biblioteca]       [Cozinha]
     *             /      \             \
     *       [Sala de Estudo] [Jardim]  [Sótão]
     *
     * Cada cômodo tem uma pista estática definida abaixo.
     */

    Sala* hall       = criarSala("Hall de Entrada", "Pegadas de lama recentes");
    Sala* biblioteca = criarSala("Biblioteca", "Página arrancada de um diário");
    Sala* cozinha    = criarSala("Cozinha", "Copo quebrado com marca de batom");
    Sala* estudo     = criarSala("Sala de Estudo", "Envelope selado com cera vermelha");
    Sala* jardim     = criarSala("Jardim", "Chave antiga caída entre as flores");
    Sala* sotao      = criarSala("Sótão", "Retrato rasgado de uma mulher desconhecida");

    hall->esquerda       = biblioteca;
    hall->direita        = cozinha;
    biblioteca->esquerda = estudo;
    biblioteca->direita  = jardim;
    cozinha->direita     = sotao;

    /* -----------------------------
     * Inicialização da BST de pistas e tabela hash
     * ----------------------------- */
    PistaNode* raizPistas = NULL;
    SuspeitoNode* tabela[TAM_HASH];
    inicializarHash(tabela);

    /* -----------------------------
     * Associação pista -> suspeito (pré-definida)
     * -----------------------------
     *
     * Observação: não há inserção dinâmica de suspeitos neste nível.
     */
    inserirNaHash(tabela, "Pegadas de lama recentes", "Jardineiro");
    inserirNaHash(tabela, "Página arrancada de um diário", "Governanta");
    inserirNaHash(tabela, "Copo quebrado com marca de batom", "Madame Sinclair");
    inserirNaHash(tabela, "Envelope selado com cera vermelha", "Governanta");
    inserirNaHash(tabela, "Chave antiga caída entre as flores", "Jardineiro");
    inserirNaHash(tabela, "Retrato rasgado de uma mulher desconhecida", "Madame Sinclair");

    /* -----------------------------
     * Exploração interativa (coleta automática de pistas)
     * ----------------------------- */
    explorarMansao(hall, &raizPistas, tabela);

    /* -----------------------------
     * Exibe pistas coletadas (ordenadas)
     * ----------------------------- */
    limparTela();
    printf("========================================================\n");
    printf("               PISTAS COLETADAS (ORDENADAS)\n");
    printf("========================================================\n\n");
    exibirPistas(raizPistas);

    /* -----------------------------
     * Fase final: acusação e veredito
     * ----------------------------- */
    verificarSuspeitoFinal(tabela, raizPistas);

    /* -----------------------------
     * Limpeza de memória
     * ----------------------------- */
    liberarPistas(raizPistas);
    liberarHash(tabela);
    liberarMansao(hall);

    printf("\nInvestigação finalizada. Obrigado por jogar Detective Quest!\n");
    return 0;
}

// ============================================================================
//                       IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================================================

Sala* criarSala(const char* nome, const char* pista) {
    /* Alocação e verificação */
    Sala* s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: falha na alocação de memória para Sala '%s'\n", nome);
        exit(EXIT_FAILURE);
    }

    /* Copia segura dos textos (assegurando terminação) */
    strncpy(s->nome, nome, MAX_NOME - 1);
    s->nome[MAX_NOME - 1] = '\0';

    if (pista && pista[0] != '\0') {
        strncpy(s->pista, pista, MAX_PISTA - 1);
        s->pista[MAX_PISTA - 1] = '\0';
    } else {
        s->pista[0] = '\0';
    }

    s->esquerda = s->direita = NULL;
    return s;
}

void explorarMansao(Sala* atual, PistaNode** raizPistas, SuspeitoNode* tabela[]) {
    char opcao;

    while (atual != NULL) {
        limparTela();
        printf("--------------------------------------------------------\n");
        printf("Local: %s\n", atual->nome);
        printf("--------------------------------------------------------\n");

        /* Coleta automática: insere pista se existir e não for vazia */
        if (atual->pista[0] != '\0') {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista encontrada aqui.\n");
        }

        /* Opções de navegação apresentadas ao jogador */
        printf("\nEscolha o caminho:\n");
        if (atual->esquerda) printf(" (e) Ir para %s\n", atual->esquerda->nome);
        if (atual->direita)  printf(" (d) Ir para %s\n", atual->direita->nome);
        printf(" (s) Encerrar investigação\n");
        printf("\n> ");
        scanf(" %c", &opcao);
        limparBufferEntrada();

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esquerda) atual = atual->esquerda;
            else {
                printf("Caminho inexistente à esquerda! Pressione ENTER para continuar...");
                limparBufferEntrada();
            }
        } else if (opcao == 'd' || opcao == 'D') {
            if (atual->direita) atual = atual->direita;
            else {
                printf("Caminho inexistente à direita! Pressione ENTER para continuar...");
                limparBufferEntrada();
            }
        } else if (opcao == 's' || opcao == 'S') {
            printf("\nEncerrando exploração...\n");
            break;
        } else {
            printf("Opção inválida! Pressione ENTER para tentar novamente...");
            limparBufferEntrada();
        }
    }
}

PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (pista == NULL || pista[0] == '\0') return raiz; /* nada a inserir */

    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            fprintf(stderr, "Erro: falha na alocação de memória para PistaNode\n");
            exit(EXIT_FAILURE);
        }
        strncpy(novo->pista, pista, MAX_PISTA - 1);
        novo->pista[MAX_PISTA - 1] = '\0';
        novo->esquerda = novo->direita = NULL;
        return novo;
    }

    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (cmp > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    } else {
        /* duplicata: não insere novamente */
    }

    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

void liberarPistas(PistaNode* raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/* ----------------- Tabela hash ----------------- */

void inicializarHash(SuspeitoNode* tabela[]) {
    for (int i = 0; i < TAM_HASH; ++i) tabela[i] = NULL;
}

int hash(const char* pista) {
    unsigned int soma = 0;
    for (const unsigned char* p = (const unsigned char*)pista; *p != '\0'; ++p)
        soma += *p;
    return (int)(soma % (unsigned int)TAM_HASH);
}

void inserirNaHash(SuspeitoNode* tabela[], const char* pista, const char* suspeito) {
    if (!pista || !suspeito) return;

    int idx = hash(pista);
    SuspeitoNode* novo = (SuspeitoNode*) malloc(sizeof(SuspeitoNode));
    if (!novo) {
        fprintf(stderr, "Erro: falha na alocação de memória para SuspeitoNode\n");
        exit(EXIT_FAILURE);
    }
    strncpy(novo->pista, pista, MAX_PISTA - 1);
    novo->pista[MAX_PISTA - 1] = '\0';
    strncpy(novo->suspeito, suspeito, MAX_NOME - 1);
    novo->suspeito[MAX_NOME - 1] = '\0';

    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

const char* encontrarSuspeito(SuspeitoNode* tabela[], const char* pista) {
    if (!pista) return "Desconhecido";
    int idx = hash(pista);
    SuspeitoNode* cur = tabela[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return "Desconhecido";
}

void liberarHash(SuspeitoNode* tabela[]) {
    for (int i = 0; i < TAM_HASH; ++i) {
        SuspeitoNode* cur = tabela[i];
        while (cur) {
            SuspeitoNode* tmp = cur;
            cur = cur->prox;
            free(tmp);
        }
        tabela[i] = NULL;
    }
}

/* ----------------- Verificação final ----------------- */

int contarPistasPorSuspeitoNaBST(SuspeitoNode* tabela[], PistaNode* raizPistas, const char* suspeito) {
    if (!raizPistas) return 0;

    int contador = 0;

    /* Função interna recursiva */
    struct Aux {
        SuspeitoNode** tabela;
        const char* suspeito;
    } aux_data;
    aux_data.tabela = tabela;
    aux_data.suspeito = suspeito;

    /* Usamos uma função recursiva local via ponteiro para função */
    int contadorRec(PistaNode* node) {
        if (!node) return 0;
        int c = 0;
        c += contadorRec(node->esquerda);
        const char* s = encontrarSuspeito(tabela, node->pista);
        if (strcmp(s, suspeito) == 0) c++;
        c += contadorRec(node->direita);
        return c;
    }

    /* chamar recursiva */
    contador = contadorRec(raizPistas);
    return contador;
}

void verificarSuspeitoFinal(SuspeitoNode* tabela[], PistaNode* raizPistas) {
    char nome[MAX_NOME];

    printf("\n========================================================\n");
    printf("                      FASE FINAL - ACUSAÇÃO\n");
    printf("========================================================\n\n");

    printf("Suspeitos conhecidos: Jardineiro, Governanta, Madame Sinclair\n");
    printf("Digite o nome do suspeito a ser acusado: ");
    if (fgets(nome, sizeof(nome), stdin) == NULL) {
        printf("Entrada inválida.\n");
        return;
    }
    nome[strcspn(nome, "\n")] = '\0'; /* remove newline */

    if (nome[0] == '\0') {
        printf("Nenhum nome informado. Acusação abortada.\n");
        return;
    }

    /* Conta quantas pistas coletadas apontam para este suspeito */
    int total = contarPistasPorSuspeitoNaBST(tabela, raizPistas, nome);

    if (total >= 2) {
        printf("\n✅ Acusação confirmada! '%s' é considerado culpado (evidências: %d pistas).\n", nome, total);
    } else if (total > 0) {
        printf("\n⚠️ Acusação frágil: '%s' tem somente %d pista(s) coletada(s) relacionada(s).\n", nome, total);
        printf("São necessárias ao menos 2 pistas para confirmação.\n");
    } else {
        printf("\n❌ Acusação sem fundamento: nenhuma pista coletada aponta para '%s'.\n", nome);
    }

    /* Exibir quais pistas coletadas apontam para cada suspeito (opcional, informativo) */
    printf("\nResumo das pistas coletadas e seus suspeitos (baseado em tabela):\n");
    exibirPistas(raizPistas);
    printf("\nAssociações (pista -> suspeito):\n");
    for (int i = 0; i < TAM_HASH; ++i) {
        SuspeitoNode* cur = tabela[i];
        while (cur) {
            printf(" - \"%s\" -> %s\n", cur->pista, cur->suspeito);
            cur = cur->prox;
        }
    }
}

/* ======================================================================== */
/*                           FUNÇÕES UTILITÁRIAS                              */
/* ======================================================================== */

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void liberarMansao(Sala* raiz) {
    if (!raiz) return;
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}
