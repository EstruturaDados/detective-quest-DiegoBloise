// ============================================================================
//         PROJETO DETECTIVE QUEST - DESAFIO DE CÓDIGO (NÍVEL NOVATO)
// ============================================================================
//
// OBJETIVOS (VERSÃO FINAL):
// - Implementar um mapa interativo de uma mansão utilizando árvore binária.
// - Permitir navegação a partir do Hall de Entrada, escolhendo esquerda/direita.
// - Exibir o nome de cada sala visitada até chegar a um nó-folha.
// - Garantir legibilidade, clareza e usabilidade na exploração.
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
//                            ESTRUTURA DE DADOS
// ============================================================================

/**
 * @struct Sala
 * @brief Representa um cômodo da mansão (nó da árvore binária).
 *
 * Cada sala possui um nome e dois caminhos possíveis: esquerda e direita,
 * representando os cômodos adjacentes. O Hall de entrada é a raiz da árvore.
 */
typedef struct Sala {
    char nome[50];           /**< Nome da sala */
    struct Sala* esquerda;   /**< Ponteiro para a sala à esquerda */
    struct Sala* direita;    /**< Ponteiro para a sala à direita */
} Sala;

// ============================================================================
//                          PROTÓTIPOS DAS FUNÇÕES
// ============================================================================

/** @brief Cria dinamicamente uma nova sala com o nome especificado. */
Sala* criarSala(const char* nome);

/** @brief Explora as salas da mansão interativamente a partir de uma sala. */
void explorarSalas(Sala* atual);

/** @brief Libera toda a memória alocada pela árvore de salas. */
void liberarMansao(Sala* raiz);

/** @brief Limpa o buffer de entrada para evitar erros no scanf. */
void limparBufferEntrada();

/** @brief Limpa o terminal (compatível com Windows e Linux). */
void limparTela();

// ============================================================================
//                           FUNÇÃO PRINCIPAL
// ============================================================================

/**
 * @brief Função principal do programa.
 *
 * Monta automaticamente a estrutura fixa da mansão (árvore binária)
 * e inicia a exploração a partir do Hall de Entrada.
 *
 * @return int Código de status do programa (0 = sucesso, 1 = erro).
 */
int main() {
    setlocale(LC_ALL, "Portuguese");

    limparTela();
    printf("==============================================\n");
    printf("       DETECTIVE QUEST - MANSÃO INTERATIVA     \n");
    printf("==============================================\n\n");

    // ------------------------------------------------------------------------
    // Estrutura fixa da mansão (criada manualmente)
    //
    //                  [Hall de Entrada]
    //                     /         \
    //          [Biblioteca]         [Cozinha]
    //            /     \               \
    //   [Estudo]     [Jardim]       [Sótão]
    // ------------------------------------------------------------------------

    Sala* hall       = criarSala("Hall de Entrada");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* cozinha    = criarSala("Cozinha");
    Sala* estudo     = criarSala("Sala de Estudo");
    Sala* jardim     = criarSala("Jardim");
    Sala* sotao      = criarSala("Sótão");

    hall->esquerda       = biblioteca;
    hall->direita        = cozinha;
    biblioteca->esquerda = estudo;
    biblioteca->direita  = jardim;
    cozinha->direita     = sotao;

    // Inicia a exploração
    explorarSalas(hall);

    // Libera memória
    liberarMansao(hall);

    printf("\nExploração encerrada. Obrigado por jogar Detective Quest!\n");
    return 0;
}

// ============================================================================
//                         IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================================================

/**
 * @brief Cria uma sala de forma dinâmica e inicializa seus ponteiros.
 * @param nome Nome do cômodo a ser criado.
 * @return Ponteiro para a sala recém-criada.
 */
Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro: falha ao alocar memória para a sala '%s'!\n", nome);
        exit(EXIT_FAILURE);
    }

    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita  = NULL;

    return nova;
}

/**
 * @brief Permite ao jogador explorar as salas da mansão.
 *
 * A exploração começa no Hall de Entrada e segue conforme
 * as escolhas do jogador: esquerda (e), direita (d) ou sair (s).
 *
 * Quando o jogador chega a uma sala sem saídas, o programa finaliza
 * automaticamente a exploração.
 *
 * @param atual Ponteiro para a sala inicial (raiz da árvore).
 */
void explorarSalas(Sala* atual) {
    char opcao;

    while (atual != NULL) {
        limparTela();
        printf("==============================================\n");
        printf("Você está na sala: %s\n", atual->nome);
        printf("==============================================\n");

        if (!atual->esquerda && !atual->direita) {
            printf("\nVocê chegou ao fim da exploração!\n");
            return;
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
 * @brief Libera recursivamente toda a memória alocada para a mansão.
 * @param raiz Ponteiro para a sala raiz da árvore (Hall de Entrada).
 */
void liberarMansao(Sala* raiz) {
    if (raiz == NULL) return;

    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
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
