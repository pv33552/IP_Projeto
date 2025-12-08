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
    POSITION position;                                  // posicao a que joga --- Juntar o enum
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
int nPlayers = 0;        // Controlar numero de atletas

/* ---- Funções do programa ---- */

/* ---- Funcoes Auxiliares ---- */
void criaOuLeFicheiroEquipas(TEAM *t)
{
    FILE *fic;
    char *nome = "equipas.dat";

    // Verificar se o ficheiro já existe (tentar ler)
    fic = fopen(nome, "rb");
    if (fic != NULL)
    {
        // Já existe, ler do ficheiro
        printf("O ficheiro '%s' existe. A ler dados...\n", nome);

        size_t lidos = fread(t, sizeof(TEAM), 1, fic);
        if (lidos != 1)
        {
            printf("Erro ao ler o ficheiro.\n");
        }
        else
        {
            printf("Dados lidos com sucesso.\n");
        }

        fclose(fic);
        return;
    }

    // Se não existe, criar e escrever
    fic = fopen(nome, "wb");
    if (fic == NULL)
    {
        fprintf(stderr, "\nImpossivel criar o ficheiro.\n");
        return;
    }

    fwrite(t, sizeof(TEAM), 1, fic);
    fclose(fic);
    printf("Ficheiro '%s' criado e escrito com sucesso.\n", nome);
}

void criaOuLeFicheiroJogadores(PLAYER *p)
{
    PLAYER *fic;
    char *nome = "jogadores.dat";

    // Verificar se o ficheiro já existe (tentar ler)
    fic = fopen(nome, "rb");
    if (fic != NULL)
    {
        // Já existe, ler do ficheiro
        printf("O ficheiro '%s' existe. A ler dados...\n", nome);

        size_t lidos = fread(p, sizeof(PLAYER), 1, fic);
        if (lidos != 1)
        {
            printf("Erro ao ler o ficheiro.\n");
        }
        else
        {
            printf("Dados lidos com sucesso.\n");
        }

        fclose(fic);
        return;
    }

    // Se não existe, criar e escrever
    fic = fopen(nome, "wb");
    if (fic == NULL)
    {
        fprintf(stderr, "\nImpossivel criar o ficheiro.\n");
        return;
    }

    fwrite(p, sizeof(PLAYER), 1, fic);
    fclose(fic);
    printf("Ficheiro '%s' criado e escrito com sucesso.\n", nome);
}

void gravaEquipas()
{
    FILE *fic = fopen("equipas.dat", "wb"); // REESCREVER
    if (!fic)
    {
        printf("Erro ao abrir ficheiro para escrita.\n");
        return;
    }

    // Gravar o número de equipas
    fwrite(&nTeams, sizeof(int), 1, fic);

    // Gravar cada equipa + jogadores
    for (int i = 0; i < nTeams; i++)
    {
        TEAM *t = listaEquipas[i];

        fwrite(t, sizeof(TEAM), 1, fic);

        for (int j = 0; j < t->nPlayers; j++)
        {
            fwrite(t->players[j], sizeof(PLAYER), 1, fic);
        }
    }

    fclose(fic);
    printf("Equipas gravadas com sucesso.\n");
}

/* ---- 1. Listar equipa ---- */
void listar_equipas()
{
    FILE *fic = fopen("equipas.dat", "rb");
    if (!fic)
    {
        printf("Ficheiro ainda nao existe.\n");
        return;
    }

    fread(&nTeams, sizeof(int), 1, fic);

    printf("\n**** LISTA DE EQUIPAS ****\n");

    for (int i = 0; i < nTeams; i++)
    {
        TEAM *t = malloc(sizeof(TEAM));
        fread(t, sizeof(TEAM), 1, fic);

        for (int j = 0; j < t->nPlayers; j++)
        {
            PLAYER *p = malloc(sizeof(PLAYER));
            fread(p, sizeof(PLAYER), 1, fic);
            t->players[j] = p;
        }

        listaEquipas[i] = t;

        printf("%d - %s (%d jogadores)\n", i, t->name, t->nPlayers);
    }

    fclose(fic);
}

/* ---- 1.1 Remover equipa ---- */
void remover_equipa(int idx)
{
    if (idx < 0 || idx >= nTeams)
    {
        printf("Equipa invalida!\n");
        return;
    }

    TEAM *t = listaEquipas[idx];

    // Libertar jogadores
    for (int j = 0; j < t->nPlayers; j++)
        free(t->players[j]);

    free(t);

    // Mover restantes equipas
    for (int i = idx; i < nTeams - 1; i++)
        listaEquipas[i] = listaEquipas[i + 1];

    nTeams--;

    printf("\nEquipa removida com sucesso!\n");

    // Atualizar ficheiro
    FILE *fic = fopen("equipas.dat", "wb");
    if (!fic)
    {
        printf("Erro a atualizar ficheiro!\n");
        return;
    }

    fwrite(&nTeams, sizeof(int), 1, fic);

    for (int i = 0; i < nTeams; i++)
    {
        TEAM *t2 = listaEquipas[i];
        fwrite(t2, sizeof(TEAM), 1, fic);

        for (int j = 0; j < t2->nPlayers; j++)
            fwrite(t2->players[j], sizeof(PLAYER), 1, fic);
    }

    fclose(fic);
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

        // Gravar as equipas no ficheiro
        gravaEquipas();

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
        printf("\nA equipa nao tem jogadores inscritos.\n");
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
    // adicionar validação
    scanf("%d", &p->num_id);

    printf("\nIntroduza o ano de nascimento do atleta: ");
    // adicionar validação
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
    gravaEquipas();

    printf("\nO atleta %s foi adicionado a equipa %s com sucesso.\n", p->name, t->name);

    // Gravar jogador no ficheiro "jogadores.dat" (modo append)
    FILE *fic = fopen("jogadores.dat", "ab");
    if (!fic)
    {
        printf("Erro ao abrir 'jogadores.dat' para escrita.\n");
        return;
    }

    fwrite(p, sizeof(PLAYER), 1, fic);
    fclose(fic);

    printf("Atleta %s gravado no ficheiro 'jogadores.dat'.\n", p->name);
}

/* ---- 2.2 Listar jogadores ----- */
void remover_jogador(int index)
{
    FILE *fic = fopen("jogadores.dat", "rb");
    if (!fic)
    {
        printf("\nErro ao abrir ficheiro\n");
        return;
    }

    FILE *temp = fopen("temp.dat", "wb");
    if (!temp)
    {
        printf("\nErro ao criar ficheiro temporario\n"); // para copiar os dados e substituir o ficheiro original
        fclose(fic);
        return;
    }

    PLAYER p;
    int i = 0;

    // Copiar todos menos o jogador a remover
    while (fread(&p, sizeof(PLAYER), 1, fic) == 1)
    {
        if (i != index)
            fwrite(&p, sizeof(PLAYER), 1, temp);

        i++;
    }

    fclose(fic);
    fclose(temp);

    // Substituir ficheiro original pelo ficheiro temporário
    remove("jogadores.dat");
    rename("temp.dat", "jogadores.dat");

    printf("\nJogador removido com sucesso!\n");
}

void atualizar_jogador(int index)
{
    FILE *fic = fopen("jogadores.dat", "rb+");
    if (!fic)
    {
        printf("Erro ao abrir o ficheiro.\n");
        return;
    }

    PLAYER p;

    // Selecionar o jogador pelo índice
    fseek(fic, index * sizeof(PLAYER), SEEK_SET);
    fread(&p, sizeof(PLAYER), 1, fic);

    printf("\n**** Gestao do Jogador: %s ****\n", p.name);
    printf("1 - Atualizar informacao\n");
    printf("2 - Eliminar jogador\n");
    printf("3 - Voltar atras\n");
    printf("Escolha uma opcao: ");

    int opc;
    scanf("%d", &opc);

    if (opc == 3)
    {
        fclose(fic);
        printf("\nOperacao cancelada.\n");
        return;
    }

    if (opc == 2)
    {
        fclose(fic); // Fechar antes de remover
        remover_jogador(index);
        return;
    }

    // ---- Atualização de dados ----
    getchar(); // limpar buffer

    printf("\nNovo nome (ENTER mantem nome actual): ");
    char novoNome[50];
    fgets(novoNome, sizeof(novoNome), stdin);
    if (novoNome[0] != '\n')
    {
        novoNome[strcspn(novoNome, "\n")] = 0;
        strcpy(p.name, novoNome);
    }

    printf("Novo ano nascimento (%d): ", p.year);
    int ano;
    if (scanf("%d", &ano) == 1)
        p.year = ano;

    // Guardar no mesmo sítio
    fseek(fic, index * sizeof(PLAYER), SEEK_SET);
    fwrite(&p, sizeof(PLAYER), 1, fic);

    fclose(fic);
    printf("\nJogador atualizado com sucesso!\n");
}

void listar_jogadores()
{
    FILE *fic = fopen("jogadores.dat", "rb");
    if (!fic)
    {
        printf("O ficheiro nao existe.\n");
        return;
    }

    PLAYER temp;
    int count = 0;

    printf("\n**** Lista de Jogadores ****\n");

    while (fread(&temp, sizeof(PLAYER), 1, fic) == 1)
    {
        printf("\nJogador %d:\n", count + 1);
        printf("Nome: %s\n", temp.name);
        printf("ID: %d\n", temp.num_id);
        printf("Ano de nascimento: %d\n", temp.year);

        char *posicao;
        switch (temp.position)
        {
        case PONTA:
            posicao = "PONTA";
            break;
        case LATERAL:
            posicao = "LATERAL";
            break;
        case CENTRAL:
            posicao = "CENTRAL";
            break;
        case PIVO:
            posicao = "PIVO";
            break;
        case GR:
            posicao = "GR";
            break;
        default:
            posicao = "Desconhecida";
            break;
        }

        printf("Posicao: %s\n", posicao);
        printf("Estatisticas: Pontos=%.1f, Remates=%.1f, Perdas=%.1f, Assist=%.1f, Fintas=%.1f, Minutos=%d\n",
               temp.mPontos, temp.mRemates, temp.mPerdas,
               temp.mAssist, temp.mFintas, temp.tMinutos);

        count++;
    }

    fclose(fic);

    if (count == 0)
    {
        printf("Nao ha jogadores gravados.\n");
        return;
    }

    // Perguntar ao utilizador se quer atualizar alguém
    char opc;
    printf("\nDeseja atualizar informacao de um atleta? (S/N): ");
    scanf(" %c", &opc);

    if (opc == 'S' || opc == 's') // 'S' ou 's'
    {
        int idx;
        printf("\nEscolha o numero do jogador a atualizar (1 a %d): ", count);
        scanf("%d", &idx);

        if (idx < 1 || idx > count)
        {
            printf("Indice invalido.\n");
            return;
        }

        atualizar_jogador(idx - 1); // converter para índice base 0
    }
}

/* ---- 2.3 Ranking de jogadores ----- */
void calcular_valia_jogador_escolhido(TEAM **listaEquipas, int nTeams)
{
    if (nTeams == 0)
    {
        printf("\nNao existem equipas carregadas.\n");
        return;
    }

    printf("\n**** LISTA DE EQUIPAS ****\n");
    for (int i = 0; i < nTeams; i++)
    {
        printf("%d - %s (%d jogadores)\n",
               i, listaEquipas[i]->name, listaEquipas[i]->nPlayers);
    }

    int idE;
    printf("\nEscolha a equipa (ou -1 para voltar): ");
    scanf("%d", &idE);

    if (idE == -1)
        return;

    if (idE < 0 || idE >= nTeams)
    {
        printf("\nEquipa invalida!\n");
        return;
    }

    TEAM *t = listaEquipas[idE];

    if (t->nPlayers == 0)
    {
        printf("\nA equipa '%s' nao tem jogadores!\n", t->name);
        return;
    }

    printf("\n**** JOGADORES DA EQUIPA %s ****\n", t->name);
    for (int i = 0; i < t->nPlayers; i++)
    {
        printf("%d - %s\n", i, t->players[i]->name);
    }

    int idJ;
    printf("\nEscolha o jogador: ");
    scanf("%d", &idJ);

    if (idJ < 0 || idJ >= t->nPlayers)
    {
        printf("\nJogador invalido!\n");
        return;
    }

    PLAYER *p = t->players[idJ];
    float valia = calcular_valia(p);

    printf("\nValia do jogador %s: %.2f\n", p->name, valia);
}

///////////////////////////////////////////////////
char menu_principal()
{
    char op;

    printf("\n **** Menu de opcoes ****    \n");
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

void carregarEquipas()
{
    FILE *fic = fopen("equipas.dat", "rb");
    if (!fic)
    {
        nTeams = 0;
        return;
    }

    fread(&nTeams, sizeof(int), 1, fic);

    for (int i = 0; i < nTeams; i++)
    {
        TEAM *t = malloc(sizeof(TEAM));
        fread(t, sizeof(TEAM), 1, fic);

        for (int j = 0; j < t->nPlayers; j++)
        {
            PLAYER *p = malloc(sizeof(PLAYER));
            fread(p, sizeof(PLAYER), 1, fic);
            t->players[j] = p;
        }

        listaEquipas[i] = t;
    }

    fclose(fic);
}

int main()
{

    char op;

    printf("\nBem-vindo ao programa de gestao de estatisticas de ANDEBOL!\n");
    TEAM tempTeam = {0};
    PLAYER tempPlayer = {0};

    criaOuLeFicheiroEquipas(&tempTeam);
    criaOuLeFicheiroJogadores(&tempPlayer);
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
                    printf("\nDeseja remover uma equipa? (S/N): ");
                    char r;
                    scanf(" %c", &r);

                    if (r == 'S' || r == 's')
                    {
                        printf("Introduza o indice da equipa que pretende remover: ");
                        int idx;
                        scanf("%d", &idx);

                        remover_equipa(idx);
                    }
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
                        printf("Equipa invalida!\n");
                        break;
                    }

                    float valia = calcular_valia_equipa(listaEquipas[idx]);
                    printf("\nA valia total da equipa %s e: %.2f\n", listaEquipas[idx]->name, valia);
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
                    carregarEquipas();
                    listar_equipas();

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
                    listar_jogadores();
                    break;
                case '3':
                    carregarEquipas();
                    calcular_valia_jogador_escolhido(listaEquipas, nTeams);
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