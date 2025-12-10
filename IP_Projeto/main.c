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
    FILE *fic;
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
        valia_total += calcular_valia(p); // chama a função de valia individual
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
    if (!p)
    {
        printf("Memoria insuficiente!\n");
        return;
    }

    printf("\nIntroduza o nome do atleta: ");
    scanf(" %[^\n]", p->name);

    if (p->name[0] == '\0')
    {
        printf("\nNome invalido.\n");
        free(p);
        return;
    }

    /* --------------------------
       VALIDACAO 1: NUMERO ID (7 dígitos)
       -------------------------- */
    do
    {
        printf("\nIntroduza o numero de identificacao do atleta (7 digitos): ");
        scanf("%d", &p->num_id);

        if (p->num_id < 0000000 || p->num_id > 9999999)
            printf("Numero invalido! Tem de ter exatamente 7 digitos.\n");

    } while (p->num_id < 0000000 || p->num_id > 9999999);

    /* --------------------------
       VALIDACAO 2: ANO DE NASCIMENTO
       -------------------------- */
    do
    {
        printf("\nIntroduza o ano de nascimento do atleta: ");
        scanf("%d", &p->year);

        if (p->year < 1950 || p->year > 2025)
            printf("Ano invalido! Tem de estar entre 1950 e 2025.\n");

    } while (p->year < 1950 || p->year > 2025);

    /* --------------------------
       VALIDACAO 3: POSICAO
       -------------------------- */
    int pos;
    do
    {
        printf("\nIntroduza a posicao do atleta (0-PONTA, 1-LATERAL, 2-CENTRAL, 3-PIVO, 4-GR): ");
        scanf("%d", &pos);

        if (pos < 0 || pos > 4)
            printf("Posicao invalida!\n");

    } while (pos < 0 || pos > 4);

    p->position = (POSITION)pos;

    /* ---- Estatísticas do jogador ---- */

    printf("\n**** Introduza as estatisticas do jogador ****\n");

    /* Pontos */
    do
    {
        printf("Pontos marcados: ");
        scanf("%f", &p->mPontos);

        if (p->mPontos < 0)
            printf("Valor invalido! Nao pode ser negativo.\n");

    } while (p->mPontos < 0);

    /* Remates */
    do
    {
        printf("Remates: ");
        scanf("%f", &p->mRemates);

        if (p->mRemates < 0)
            printf("Valor invalido!\n");

    } while (p->mRemates < 0);

    /* Perdas de bola */
    do
    {
        printf("Perdas de bola: ");
        scanf("%f", &p->mPerdas);

        if (p->mPerdas < 0)
            printf("Valor inválido!\n");

    } while (p->mPerdas < 0);

    /* Assistências */
    do
    {
        printf("Assistencias: ");
        scanf("%f", &p->mAssist);

        if (p->mAssist < 0)
            printf("Valor invalido!\n");

    } while (p->mAssist < 0);

    /* Fintas */
    do
    {
        printf("Fintas: ");
        scanf("%f", &p->mFintas);

        if (p->mFintas < 0)
            printf("Valor invalido!\n");

    } while (p->mFintas < 0);

    /* Minutos jogados */
    do
    {
        printf("Minutos jogados: ");
        scanf("%d", &p->tMinutos);

        if (p->tMinutos < 0)
            printf("Valor invalido!\n");

    } while (p->tMinutos < 0);

    printf("\nEstatisticas registadas com sucesso!\n");

    /* Guardar na equipa */
    t->players[t->nPlayers] = p;
    t->nPlayers++;
    gravaEquipas();

    printf("\nO atleta %s foi adicionado a equipa %s com sucesso.\n", p->name, t->name);

    /* Guardar no ficheiro */
    FILE *fic = fopen("jogadores.dat", "ab");
    if (!fic)
    {
        printf("Erro ao abrir ficheiro jogadores.dat\n");
        return;
    }

    fwrite(p, sizeof(PLAYER), 1, fic);
    fclose(fic);

    printf("Atleta gravado no ficheiro.\n");
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

    fseek(fic, index * sizeof(PLAYER), SEEK_SET);
    fread(&p, sizeof(PLAYER), 1, fic);

    printf("\n**** Gestao do Jogador: %s ****\n", p.name);
    printf("1 - Atualizar informacao\n");
    printf("2 - Eliminar jogador\n");
    printf("3 - Voltar atras\n");
    printf("Escolha uma opcao: ");

    int opc;
    scanf("%d", &opc);
    getchar(); // limpar buffer

    if (opc == 3)
    {
        fclose(fic);
        printf("\nOperacao cancelada.\n");
        return;
    }

    if (opc == 2)
    {
        fclose(fic);
        remover_jogador(index);
        return;
    }

    printf("\nNovo nome (ENTER mantém '%s'): ", p.name);
    char tmp[50];
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
    {
        tmp[strcspn(tmp, "\n")] = 0;
        strcpy(p.name, tmp);
    }

    printf("Ano de nascimento (%d): ", p.year);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
    {
        int novoAno = atoi(tmp);
        if (novoAno >= 1950 && novoAno <= 2025)
            p.year = novoAno;
        else
            printf("Ano invalido! Mantido: %d\n", p.year);
    }

    /* ================================
       ATUALIZAÇÃO DAS ESTATÍSTICAS
    ================================= */

    printf("\n**** Atualizar Estatisticas ****\n");

    printf("Pontos (%.1f): ", p.mPontos);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.mPontos = atof(tmp); // conerter para inteiro

    printf("Remates (%.1f): ", p.mRemates);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.mRemates = atof(tmp); // conerter para inteiro

    printf("Perdas (%.1f): ", p.mPerdas);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.mPerdas = atof(tmp); // conerter para inteiro

    printf("Assistencias (%.1f): ", p.mAssist);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.mAssist = atof(tmp); // conerter para inteiro

    printf("Fintas (%.1f): ", p.mFintas);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.mFintas = atof(tmp); // conerter para inteiro

    printf("Minutos (%d): ", p.tMinutos);
    fgets(tmp, sizeof(tmp), stdin);
    if (tmp[0] != '\n')
        p.tMinutos = atoi(tmp); // conerter para inteiro

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
void calcular_valia_jogadores(TEAM **listaEquipas, int nTeams)
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

    printf("\nEscolha a equipa (ou -1 para todas): ");
    int idE;
    scanf("%d", &idE);

    /* ---- CASO 1: TODAS AS EQUIPAS ---- */
    if (idE == -1)
    {
        printf("\n*** VALIA DE TODAS AS EQUIPAS ***\n");
        for (int i = 0; i < nTeams; i++)
        {
            TEAM *t = listaEquipas[i];
            printf("\n**** Equipa: %s ****\n", t->name);

            for (int j = 0; j < t->nPlayers; j++)
            {
                PLAYER *p = t->players[j];

                if (p->tMinutos <= 100)
                {
                    printf("   %s - Nao jogou minutos insuficientes (%d).\n",
                           p->name, p->tMinutos);
                    continue;
                }

                float valia = calcular_valia(p);
                printf("   %s - Valia: %.2f\n", p->name, valia);
            }
        }
        return;
    }

    /* ---- CASO 2: APENAS 1 EQUIPA ---- */

    if (idE < 0 || idE >= nTeams)
    {
        printf("\nEquipa invalida!\n");
        return;
    }

    TEAM *t = listaEquipas[idE];

    printf("\n**** Ranking dos jogadores da equipa: %s ****\n", t->name);

    for (int j = 0; j < t->nPlayers; j++)
    {
        PLAYER *p = t->players[j];

        if (p->tMinutos <= 100)
        {
            printf("   %s - Nao jogou minutos insuficientes (%d).\n",
                   p->name, p->tMinutos);
            continue;
        }

        float valia = calcular_valia(p);
        printf("   %s - Valia: %.2f\n", p->name, valia);
    }
}


/* ___________________________________________________RANKING*/
void relatorio_valias()
{
    carregarEquipas(); // GARANTE QUE OS DADOS SAO LIDOS, nao estava a carregar os dados

    if (nTeams == 0)
    {
        printf("\nNao ha equipas registadas.\n");
        return;
    }

    TEAM *mais_valiosa = NULL; // comecar a null para podermos comparar e guaradr a mais valiosa
    TEAM *menos_valiosa = NULL;// comecar a null para podermos comparar e guaradr a mais valiosa, assim qualquer valor real vai ser menor
    float maior = -999999, menor = 999999; // valores iniciais extremos para comparar, em maior usar um valor negativo e maior um valor exagerado, assim qualquer valor real vai ser menor

    PLAYER *jogador_mais = NULL;// comecar a null para podermos comparar e guaradr a mais valiosa
    PLAYER *jogador_menos = NULL;// comecar a null para podermos comparar e guaradr a mais valiosa
    float maior_j = -999999, menor_j = 999999; // valores iniciais extremos para comparar, em maior usar um valor negativo e maior um valor exagerado

    for (int i = 0; i < nTeams; i++)
    {
        TEAM *t = listaEquipas[i];
        float v = calcular_valia_equipa(t);

        if (v > maior)
        {
            maior = v;
            mais_valiosa = t;
        }
        if (v < menor)
        {
            menor = v;
            menos_valiosa = t;
        }

        // Jogadores
        for (int j = 0; j < t->nPlayers; j++)
        {
            PLAYER *p = t->players[j];
            float val = calcular_valia(p);

            if (val > maior_j)
            {
                maior_j = val;
                jogador_mais = p;
            }
            if (val < menor_j)
            {
                menor_j = val;
                jogador_menos = p;
            }
        }
    }

    printf("\n**** RELATORIO DE VALIAS ****\n");

    printf("\nEquipa mais valiosa: %s (%.2f)\n", mais_valiosa->name, maior);
    printf("Equipa menos valiosa: %s (%.2f)\n", menos_valiosa->name, menor);

    printf("\nJogador mais valioso: %s (%.2f)\n", jogador_mais->name, maior_j);
    printf("⬇️ Jogador menos valioso: %s (%.2f)\n", jogador_menos->name, menor_j);

    printf("\n");
}


///////////////////////////////////////////////////
char menu_principal()
{
    char op;

    printf("\n **** Menu de opcoes ****    \n");
    printf("#-------------------------#\n");
    printf("| 1 - Gestao de equipas   |\n");
    printf("| 2 - Gestao de jogadores |\n");
    printf("| 3 - * Equipa e jogadpr mais valiosa e menos valiosa?? |\n");
    printf("| 4 - Sair                |\n");
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
                    calcular_valia_jogadores(listaEquipas, nTeams);
                    break;
                case '4':
                    break;
                }
            } while (opc2 != '4');
            break;
        }
        case '3':
            relatorio_valias();
            break;
        case '4':
            printf("\nObrigado por utilizar o programa de gestao de estatisticas de ANDEBOL!\n");
            break;
        }

    } while (op != '4');

    return 0;
}