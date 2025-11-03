// ============================================================================
//         PROJETO DETECTIVE QUEST - DESAFIO DE CÓDIGO (NÍVEL AVENTUREIRO)
// ============================================================================
//
// OBJETIVOS (VERSÃO FINAL):
// - Integrar a árvore binária da mansão com uma BST de pistas.
// - Associar uma pista a cada cômodo e coletá-las durante a exploração.
// - Inserir automaticamente as pistas encontradas na BST em ordem alfabética.
// - Exibir todas as pistas coletadas ao final da jornada.
//
// ============================================================================
// AUTOR: Diego Bloise
// DATA: Novembro de 2025
// LINGUAGEM: C (ANSI C - padrão C99)
// COMPILAÇÃO (exemplo):
//     gcc detective_quest.c -o detective_quest
//
// EXECUÇÃO:
//     ./detective_quest
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// ============================================================================
//                            ESTRUTURAS DE DADOS
// ============================================================================

/**
 * @struct Sala
 * @brief Representa um cômodo da mansão (nó da árvore binária).
 *
 * Cada sala contém um nome, uma pista (opcional) e referências
 * para as salas à esquerda e à direita.
 */
typedef struct Sala {
    char nome[50];           /**< Nome do cômodo */
    char pista[100];         /**< Pista associada ao cômodo (opcional) */
    struct Sala* esquerda;   /**< Ponteiro para a sala à esquerda */
    struct Sala* direita;    /**< Ponteiro para a sala à direita */
} Sala;

/**
 * @struct PistaNode
 * @brief Representa um nó da árvore BST de pistas.
 *
 * Cada nó armazena uma pista e seus ponteiros esquerdo e direito,
 * permitindo a organização alfabética automática.
 */
typedef struct PistaNode {
    char pista[100];             /**< Texto da pista */
    struct PistaNode* esquerda;  /**< Ponteiro para a pista menor */
    struct PistaNode* direita;   /**< Ponteiro para a pista maior */
} PistaNode;

// ============================================================================
//                           PROTÓTIPOS DE FUNÇÕES
// ============================================================================

// Funções principais
Sala* criarSala(const char* nome, const char* pista);
PistaNode* inserirPista(PistaNode* raiz, const char* pista);
void explorarSalasComPistas(Sala* atual, PistaNode** raizPistas);
void exibirPistas(PistaNode* raiz);

// Funções utilitárias
void liberarMansao(Sala* raiz);
void liberarPistas(PistaNode* raiz);
void limparBufferEntrada();
void limparTela();

// ============================================================================
//                           FUNÇÃO PRINCIPAL
// ============================================================================

/**
 * @brief Ponto de entrada principal do programa.
 *
 * Constrói a estrutura fixa da mansão com pistas,
 * inicia a exploração interativa e exibe as pistas coletadas.
 *
 * @return int Código de status (0 = sucesso).
 */
int main() {
    setlocale(LC_ALL, "Portuguese");

    limparTela();
    printf("==============================================\n");
    printf("   DETECTIVE QUEST - COLETA DE PISTAS (NÍVEL AVENTUREIRO)\n");
    printf("==============================================\n\n");

    // ------------------------------------------------------------------------
    // Estrutura fixa da mansão com pistas
    //
    //                     [Hall de Entrada]
    //                       /          \
    //           [Biblioteca]            [Cozinha]
    //              /     \                   \
    //     [Estudo]     [Jardim]            [Sótão]
    // ------------------------------------------------------------------------

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

    // Raiz da árvore de pistas
    PistaNode* raizPistas = NULL;

    // Inicia a exploração da mansão
    explorarSalasComPistas(hall, &raizPistas);

    // Exibe todas as pistas coletadas em ordem alfabética
    limparTela();
    printf("==============================================\n");
    printf("          PISTAS COLETADAS (ORDENADAS)\n");
    printf("==============================================\n\n");
    exibirPistas(raizPistas);

    // Libera memória
    liberarMansao(hall);
    liberarPistas(raizPistas);

    printf("\nExploração encerrada. Mistério quase resolvido!\n");
    return 0;
}

// ============================================================================
//                      IMPLEMENTAÇÃO DAS FUNÇÕES PRINCIPAIS
// ============================================================================

/**
 * @brief Cria dinamicamente um cômodo com ou sem pista.
 *
 * @param nome Nome do cômodo.
 * @param pista Texto da pista associada (pode ser vazio).
 * @return Ponteiro para a sala criada.
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro: falha ao alocar memória para a sala '%s'!\n", nome);
        exit(EXIT_FAILURE);
    }

    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista ? pista : "");
    nova->esquerda = NULL;
    nova->direita  = NULL;
    return nova;
}

/**
 * @brief Insere uma nova pista na árvore de pistas (BST).
 *
 * As pistas são organizadas alfabeticamente.
 *
 * @param raiz Ponteiro para a raiz da árvore.
 * @param pista Texto da pista a ser inserida.
 * @return Ponteiro atualizado para a raiz da árvore.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            printf("Erro: falha ao alocar memória para a pista!\n");
            exit(EXIT_FAILURE);
        }
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}

/**
 * @brief Controla a exploração das salas e coleta de pistas.
 *
 * O jogador escolhe entre esquerda (e), direita (d) ou sair (s).
 * A cada sala visitada, sua pista é coletada automaticamente
 * e adicionada à árvore de pistas.
 *
 * @param atual Ponteiro para a sala inicial (raiz da mansão).
 * @param raizPistas Endereço do ponteiro raiz da BST de pistas.
 */
void explorarSalasComPistas(Sala* atual, PistaNode** raizPistas) {
    char opcao;

    while (atual != NULL) {
        limparTela();
        printf("==============================================\n");
        printf("Você está na sala: %s\n", atual->nome);
        printf("==============================================\n");

        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista encontrada aqui.\n");
        }

        printf("\nEscolha o caminho:\n");
        if (atual->esquerda) printf(" (e) Ir para %s\n", atual->esquerda->nome);
        if (atual->direita)  printf(" (d) Ir para %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("\n> ");
        scanf(" %c", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 'e':
            case 'E':
                if (atual->esquerda) atual = atual->esquerda;
                else printf("Não há sala à esquerda!\n");
                break;
            case 'd':
            case 'D':
                if (atual->direita) atual = atual->direita;
                else printf("Não há sala à direita!\n");
                break;
            case 's':
            case 'S':
                printf("\nSaindo da exploração...\n");
                return;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/**
 * @brief Exibe as pistas armazenadas na BST em ordem alfabética.
 *
 * Implementa percurso in-order recursivo.
 *
 * @param raiz Ponteiro para a raiz da árvore de pistas.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// ============================================================================
//                           FUNÇÕES AUXILIARES
// ============================================================================

/**
 * @brief Libera toda a memória da árvore de salas (mansão).
 * @param raiz Ponteiro para a raiz da árvore binária.
 */
void liberarMansao(Sala* raiz) {
    if (!raiz) return;
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}

/**
 * @brief Libera toda a memória da árvore BST de pistas.
 * @param raiz Ponteiro para a raiz da árvore BST.
 */
void liberarPistas(PistaNode* raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/**
 * @brief Limpa o buffer de entrada padrão (stdin).
 */
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Limpa o terminal de forma compatível com o sistema operacional.
 */
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
