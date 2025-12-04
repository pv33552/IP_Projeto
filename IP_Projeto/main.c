#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_num_Id 7            // Numero ID de atleta com numero unico e positivo de 7 digitos
#define MAX_players_per_team 15 // Numero maximo de atletas por equipa

/* ---- Estruturas do programa ---- */
typedef enum position
{
    PONTA,
    LATERAL,
    CENTRAL,
    PIVO,
    GR
} POSITION;

typedef struct player
{
    char name[100];
    int num_id;
    int year;
    POSITION position;                                  // posicao a que joga --- Como juntar o enum????
    float mPontos, mRemates, mPerdas, mAssist, mFintas; // para as estatisticas
    int tMinutos;
} PLAYER;

typedef struct team
{
    char name[50];
    int nPlayers;
    PLAYER *players[MAX_players_per_team]; // Controlar um maximo de 15
} TEAM;

TEAM *listaEquipas[100]; // até 100 equipas
int nTeams = 0;          // Controlar numero de equipas
int nPlayers = 0;        // Controlar numero de atl

/* ---- Funções do programa ---- */
/* ---- 1. Listar equipa ---- */
void listar_equipas()
{
    printf("\nMostrar equipas registadas: \n");
    for (int i = 0; i < nTeams; i++)
    {
        printf("\tNome da equipa: %s\n", listaEquipas[i]->name);
    }
}

/* ---- 2. Funcoes de registar equipa ---- */
TEAM *registar_equipa()
{
    TEAM *t = malloc(sizeof(TEAM));

    if (t == NULL)
    {
        printf("Memoria insuficiente!\n");
        return NULL; // termina a funcao em caso de erro
    }

    printf("\nIntroduza o nome da equipa a registar: ");
    scanf(" %[^\n]", t->name); // para conseguir ler espacos no nome da equipa

    if (t->name[0] == '\0')
    {
        printf("\nNome invalido, por favor introduza um nome valido.\n");
        free(t);
        return NULL;
    }
    else
    {
        t->nPlayers = 0;          // inicia com jogadores a 0
        listaEquipas[nTeams] = t; // guardar a equipa na lista ARRAY
        nTeams++;

        printf("\nA equipa %s foi registada com sucesso.\n", t->name);

        return t;
    }
}


/* ---- *************. Pesquisar atletas OU equipas por valia  ----  */
// Calcular a valia primeiro

float calcular_valia(PLAYER *p)
{
    switch (p->position)
    {
    case PONTA:
        return 5 * p->mPontos + 4 * p->mRemates + 2 * p->mAssist - 3 * p->mPerdas;
    case LATERAL:
        return 3 * p->mPontos + 2 * p->mRemates + 4 * p->mAssist - 3 * p->mPerdas;
    case CENTRAL:
        return 2 * p->mPontos + 5 * p->mAssist + 4 * p->mFintas - 2 * p->mPerdas;
    case PIVO:
        return 2 * p->mPontos + 1 * p->mRemates + 5 * p->mFintas - 2 * p->mPerdas;
    case GR: // Caso especial para GR
        return 0;
    default:
        return 0;
    }
}



/* ---- 3. Calcular valida da eequipa ----- */
// Calcula a valia total de uma equipa usando a função calcular_valia para cada jogador
float calcular_valia_equipa(TEAM *t)
{
    if (!t || t->nPlayers == 0)
    {
        printf("\nA equipa não tem jogadores.\n");
        return 0.0;
    }

    float valia_total = 0.0;
    for (int i = 0; i < t->nPlayers; i++)
    {
        PLAYER *p = t->players[i];
        valia_total += calcular_valia(p); // usa a função já criada
    }

    return valia_total;
}

/* ---- 2.1 Adicionar atletas a uma equipa ---- */
void adicionar_jogador(TEAM *t)
{
    if (t->nPlayers >= MAX_players_per_team)
    {
        printf("\nA equipa %s ja tem %d atletas (limite).\n", t->name, MAX_players_per_team);
        return;
    }

    PLAYER *p = malloc(sizeof(PLAYER));
    if (p == NULL)
    {
        printf("Memoria insuficiente!\n");
        return; // termina a funcao em caso de erro
    }

    printf("\nIntroduza o nome do atleta: ");
    scanf(" %[^\n]", p->name);

    if (p->name[0] == '\0')
    {
        printf("\nNome invalido, por favor introduza um nome valido.\n");
        free(p);
        return;
    }

    printf("\nIntroduza o numero de identificacao do atleta (7 digitos): ");
    scanf("%d", &p->num_id);

    printf("\nIntroduza o ano de nascimento do atleta: ");
    scanf("%d", &p->year);

    printf("\nIntroduza a posicao do atleta (0 - PONTA, 1 - LATERAL, 2 - CENTRAL, 3 - PIVO, 4 - GR): ");
    int pos;
    scanf("%d", &pos);

    if (pos < 0 || pos > 4)
    {
        printf("\nPosicao invalida, por favor introduza uma posicao valida.\n");
        free(p);
        return;
    }

    printf("\nO atleta %s foi adicionado a equipa %s com sucesso.\n", p->name, t->name);

    p->position = (POSITION)pos;

    // Para iniciar as estatisticas a zero de cada jogador inserido
    p->mPontos = 0.0;
    p->mRemates = 0.0;
    p->mPerdas = 0.0;
    p->mAssist = 0.0;
    p->mFintas = 0.0;
    p->tMinutos = 0;

    // guardar na equipa
    t->players[t->nPlayers] = p;
    t->nPlayers++;
}

///////////////////////////////////////////////////
char menu_principal()
{
    char op;

    printf("\nMenu de opcoes:\n");
    printf("#-------------------------#\n");
    printf("| 1 - Gestao de equipas   |\n");
    printf("| 2 - Gestao de jogadores |\n");
    printf("| 3 - Sair                |\n");
    printf("#-------------------------#\n");
    printf("\nEscolha uma opcao: ");
    scanf(" %c", &op);

    return op;
}

char menu_gestao_equipas()
{
    char op;

    printf("\nFazer gestao da equipa: \n");
    printf("#------------------------#\n");
    printf("| 1 - Listar equipas     |\n");
    printf("| 2 - Adicionar equipa   |\n");
    printf("| 3 - Ranking de equipas |\n");
    printf("| 4 - Sair               |\n");
    printf("#------------------------#\n");
    printf("\nEscolha uma opcao: ");
    scanf(" %c", &op);

    return op;
}

char menu_gestao_jogadores()
{
    char op;

    printf("\nGestao de jogadores: \n");
    printf("#--------------------------#\n");
    printf("| 1 - Adicionar jogadores  |\n");
    printf("| 2 - Listar jogadores     |\n");
    printf("| 3 - Ranking de jogadores |\n");
    printf("| 4 - Sair                 |\n");
    printf("#--------------------------#\n");
    printf("\nEscolha uma opcao: ");
    scanf(" %c", &op);

    return op;
}

///////////////////////////////////////////////////

int main()
{

    char op;

    printf("\nBem-vindo ao programa de gestao de estatisticas de ANDEBOL!\n");

    do
    {

        op = menu_principal();

        switch (op)
        {

        case '1':
        {
            char opc;

            do
            {
                opc = menu_gestao_equipas();
                switch (opc)
                {
                case '1':
                    listar_equipas();
                    break;
                case '2':
                    registar_equipa();
                    break;
                case '3':
                {
                    listar_equipas();
                    printf("Escolha a equipa para calcular a valia: ");
                    int idx;
                    scanf("%d", &idx);
        
                    if (idx < 0 || idx >= nTeams)
                    {
                        printf("Equipa inválida!\n");
                        break;
                    }
        
                    float valia = calcular_valia_equipa(listaEquipas[idx]);
                    printf("\nA valia total da equipa %s é: %.2f\n", listaEquipas[idx]->name, valia);
                    break;
                }
                    break;
                case '4':
                    break;
                }
            } while (opc != '4');
            break;
        }
        case '2':
        {

            char opc2;

            do
            {
                opc2 = menu_gestao_jogadores();
                switch (opc2)
                {
                case '1':
                    printf("\nEquipas disponiveis: \n");

                    for (int i = 0; i < nTeams; i++)
                    {
                        printf("%d - %s (%d jogadores)\n",
                               i, listaEquipas[i]->name, listaEquipas[i]->nPlayers);
                    }

                    printf("Escolha a equipa: ");
                    int idx;
                    scanf("%d", &idx);

                    if (idx < 0 || idx >= nTeams)
                    {
                        printf("Equipa invalida!\n");
                        break;
                    }
                    else
                    {
                        adicionar_jogador(listaEquipas[idx]);
                        break;
                    }
                case '2':
                    /* listar_jogadores(); */
                    break;
                case '3':
                    /* ranking_jogadores(); */
                    break;
                case '4':
                    break;
                }
            } while (opc2 != '4');
            break;
        }
        case '3':
            printf("\nObrigado por utilizar o programa de gestao de estatisticas de ANDEBOL!\n");
            break;
        }

    } while (op != '3');

    return 0;
}