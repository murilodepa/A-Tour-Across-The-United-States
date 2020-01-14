#include <iostream>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "tipo.h"
#include "cidades.h"

#define tamanhoDaMatriz 43
#define tamanhoMaiorPalavra 24

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void matriz_adjacente(int [tamanhoDaMatriz][tamanhoDaMatriz], int [tamanhoDaMatriz][tamanhoDaMatriz]);

void imprimir_cidades(char [tamanhoDaMatriz][tamanhoMaiorPalavra]);

int imprimir_menu(int [tamanhoDaMatriz][tamanhoDaMatriz], int [tamanhoDaMatriz][tamanhoDaMatriz], char [tamanhoDaMatriz][tamanhoMaiorPalavra]);

int imprimir_submenu(char [tamanhoDaMatriz][tamanhoMaiorPalavra], int []);

void BublleSort (int [], int);

void copiar_Matriz(int [tamanhoDaMatriz][tamanhoDaMatriz], int [tamanhoDaMatriz][tamanhoDaMatriz]);

void opcao_de_entrada(int, int, int *, int *, int [], int);

int opcao_subconjunto(int [tamanhoDaMatriz][tamanhoDaMatriz], int [tamanhoDaMatriz][tamanhoDaMatriz], char [tamanhoDaMatriz][tamanhoMaiorPalavra], int [], int *);

int verificar_numero(char );

int imprimir_opcao_diaria(int, int [], char [tamanhoDaMatriz][tamanhoMaiorPalavra], int [], int, int *, int *, int *);

void calcular_saida(int, int *, int *, int *, int *);

void converter_dias(int, int *, int *, int *);

int main(int argc, char *argv[])
{
    try
    {
        struct mensagem msg;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        int sockfd, portno, n;
        int i, j, k, g=0, flag=0, m=0, ale=0, subconjunto=0, flagCidadeLink=0, dia, mes, ano, dias_totais=0, flag_diaria=0;
        int caminho_min[tamanhoDaMatriz-1]= {0},caminho_res[tamanhoDaMatriz-1]= {0};
        int cidades_subconjunto[42] = {0}, quantidade_de_dias[43] = {0};
        int matriz[tamanhoDaMatriz][tamanhoDaMatriz]= {0}, matriz_aux[tamanhoDaMatriz][tamanhoDaMatriz]= {0};
        int entrada=0, menu_anterior=0, menu=0, saida=0;

        char buffer[256];
        if (argc < 3)
        {
            fprintf(stderr,"usage %s hostname port\n", argv[0]);
            exit(0);
        }
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");
        server = gethostbyname(argv[1]);
        if (server == NULL)
        {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,
              (char *)&serv_addr.sin_addr.s_addr,
              server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
            error("ERROR connecting");

        menu = imprimir_menu(matriz, matriz_aux, cidades);
        menu_anterior=menu;
        msg.tipo = menu;

        if(menu==3)
        {
            menu = opcao_subconjunto(matriz, matriz_aux, cidades, cidades_subconjunto, &subconjunto);
            k=0;
            for(i=0; i<tamanhoDaMatriz; i++)
            {
                for(j=0; j<tamanhoDaMatriz; j++)
                {
                    msg.subconjunto[k]=matriz[i][j];
                    k+=1;
                }
            }

            if(menu==1)
            {
                msg.tipo = 3;
            }
            else
            {
                msg.tipo = 4;
            }
        }
        //scanf("%d",&msg.tipo);

        opcao_de_entrada(menu_anterior, menu, &entrada, &saida, cidades_subconjunto, subconjunto);

        if(menu == 1)

            flag_diaria = imprimir_opcao_diaria(menu_anterior, quantidade_de_dias, cidades, cidades_subconjunto, subconjunto, &dia, &mes, &ano);

        msg.in = entrada;
        msg.out = saida;

        system("clear");
        n = write(sockfd,(struct msg*)&msg,sizeof msg);

        if (n < 0)
            error("ERROR writing to socket");
        //bzero(msg,sizeof msg);
        n = read(sockfd,(struct msg*)&msg,sizeof msg);
        if (n < 0)
            error("ERROR reading from socket");


        i=1;
        if(msg.custo_min!=99999 && flag_diaria==1)
        {
            printf("\n DATA DE CHEGADA NOS ESTADOS UNIDOS: %d/%d/%d\n\n", dia, mes, ano);

            printf(" %s -> %s (Em torno de %d Km)\n",cidades[msg.caminho_min[0]],cidades[msg.caminho_min[1]], matriz[msg.caminho_min[0]][msg.caminho_min[1]]);
            printf(" CHEGADA %s: %d/%d/%d - ", cidades[msg.caminho_min[0]], dia, mes, ano);
            calcular_saida(quantidade_de_dias[msg.caminho_min[0]], &dia, &mes, &ano, &dias_totais);
            quantidade_de_dias[msg.caminho_min[0]]=0;
            printf(" SAIDA %s: %d/%d/%d\n\n", cidades[msg.caminho_min[0]], dia,mes,ano);
            while(msg.caminho_min[i]!=saida)
            {
                    printf(" %s -> %s - (Em torno de %d Km)  \n",cidades[msg.caminho_min[i]],cidades[msg.caminho_min[i+1]], matriz[msg.caminho_min[i]][msg.caminho_min[i+1]]);
                    printf(" CHEGADA %s: %d/%d/%d - ", cidades[msg.caminho_min[i]], dia, mes, ano);
                    calcular_saida(quantidade_de_dias[msg.caminho_min[i]], &dia, &mes, &ano, &dias_totais);
                    quantidade_de_dias[msg.caminho_min[i]]=0;
                    printf(" SAIDA %s: %d/%d/%d\n\n", cidades[msg.caminho_min[i]], dia,mes,ano);
                    i++;
       

            }

            printf(" DATA DE EMBARQUE PARA O BRASIL: %d/%d/%d\n\n", dia,mes,ano);

            dia=0;
            mes=0;
            ano=0;
            converter_dias(dias_totais, &dia, &mes, &ano);
        }
        else
        {
            if(msg.custo_min!=99999 && flag_diaria!=1)
            {
                printf(" %s -> %s (Em torno de %d Km)\n",cidades[msg.caminho_min[0]],cidades[msg.caminho_min[1]], matriz[msg.caminho_min[0]][msg.caminho_min[1]]);

                while(msg.caminho_min[i]!=saida)
                {
                        printf(" %s -> %s (Em torno de %d Km)  \n",cidades[msg.caminho_min[i]],cidades[msg.caminho_min[i+1]], matriz[msg.caminho_min[i]][msg.caminho_min[i+1]]);
                        i++;

                }
            }
        }

        printf("\n\n DISTANCIA TOTAL QUE IRA PERCORRER: %d Km",msg.custo_min);
        printf("\n\n TEMPO DE EXECUCAO: %d ms",msg.tempo);
        msg.tempo = (msg.tempo/1000);
        printf("\n APROXIMADAMENTE (TEMPO DE EXECUCAO): %d s\n",msg.tempo);

        close(sockfd);
    }
    catch(int e)
    {
    }

    return 0;
}


void matriz_adjacente(int matriz[tamanhoDaMatriz][tamanhoDaMatriz], int matriz_aux[tamanhoDaMatriz][tamanhoDaMatriz])
{
    int i=0, j=0;

    for(i=1; i<43; i++)
    {
        matriz[0][i]=i;
        matriz[i][0]=i;
        matriz_aux[0][i]=i;
        matriz_aux[i][0]=i;
    }

    matriz[2][1] =170;
    matriz[6][1] =500;
    matriz[1][2] =170;
    matriz[3][2] = 640;
    matriz[6][2] = 420;
    matriz[7][2] = 580;

    matriz[2][3] =640;
    matriz[4][3] =380;
    matriz[7][3] =300;

    matriz[3][4] =380;
    matriz[5][4] =120;
    matriz[8][4] =270;
    matriz[9][4] =160;

    matriz[4][5] =120;
    matriz[9][5] =140;
    matriz[13][5] =350;

    matriz[1][6] =500;
    matriz[2][6] =420;
    matriz[7][6] =420;
    matriz[10][6] =670;

    matriz[2][7] =580;
    matriz[3][7] =300;
    matriz[6][7] =420;
    matriz[9][7] =780;
    matriz[11][7] =520;

    matriz[4][8] =270;
    matriz[9][8] =290;
    matriz[11][8] =420;
    matriz[12][8] =470;

    matriz[4][9] =160;
    matriz[5][9] =140;
    matriz[7][9] =780;
    matriz[8][9] =290;
    matriz[12][9] =440;

    matriz[6][10] =670;
    matriz[14][10] =930;
    matriz[19][10] =1340;

    matriz[7][11] =520;
    matriz[8][11] =420;
    matriz[13][11] =660;
    matriz[14][11] =530;

    matriz[8][12] =470;
    matriz[9][12] =440;
    matriz[13][12] =360;

    matriz[5][13] =350;
    matriz[11][13] =660;
    matriz[12][13] =360;
//matriz[18][13] =350;
    matriz[22][13] =1070;
    matriz[23][13] =990;

    matriz[10][14] =930;
    matriz[11][14] =530;
    matriz[15][14] =120;
    matriz[16][14] =160;
    matriz[17][14] =170;
    matriz[20][14] =540;

    matriz[14][15] =120;
    matriz[17][15] =180;
    matriz[22][15] =730;

    matriz[14][16] =160;
    matriz[17][16] =80;

    matriz[14][17] =170;
    matriz[15][17] =180;
    matriz[16][17] =80;

// matriz[13][18] =350;

    matriz[10][19] =1340;
    matriz[20][19] =380;
    matriz[25][19] =340;
    matriz[26][19] =410;

    matriz[14][20] =540;
    matriz[19][20] =380;
    matriz[21][20] =190;
    matriz[26][20] =470;

    matriz[20][21] =190;
    matriz[22][21] =550;
    matriz[27][21] =250;

    matriz[13][22] =1070;
    matriz[15][22] =730;
    matriz[21][22] =550;
    matriz[23][22] =280;
    matriz[24][22] =250;
    matriz[28][22] =320;
    matriz[34][22] =790;

    matriz[13][23] =990;
    matriz[22][23] =280;
    matriz[24][23] =310;

    matriz[22][24] =250;
    matriz[23][24] =310;
    matriz[30][24] =530;

    matriz[19][25] =340;
    matriz[26][25] =90;

    matriz[19][26] =410;
    matriz[20][26] =470;
    matriz[25][26] =90;
    matriz[27][26] =300;
    matriz[31][26] =280;
    matriz[32][26] = 180;
    matriz[41][26] =700;

    matriz[21][27] =250;
    matriz[26][27] =300;
    matriz[28][27] =400;
    matriz[29][27] =290;
    matriz[32][27] =250;

    matriz[22][28] =320;
    matriz[27][28] =400;
    matriz[29][28] =140;

    matriz[27][29] = 290;
    matriz[28][29] =140;
    matriz[33][29] =210;
    matriz[34][29] =390;

    matriz[24][30] =530;
    matriz[34][30] =470;
    matriz[35][30] =640;
    matriz[37][30] =860;

    matriz[26][31] =280;
    matriz[38][31] =650;
    matriz[40][31] =640;
    matriz[41][31] =530;

    matriz[26][32] =180;
    matriz[27][32] =250;
    matriz[33][32] =290;
    matriz[41][32] =590;

    matriz[29][33] =210;
    matriz[32][33] =290;
    matriz[34][33] =240;
    matriz[42][33] =710;

    matriz[22][34] =790;
    matriz[29][34] =390;
    matriz[30][34] =470;
    matriz[33][34] =240;
    matriz[35][34] =440;
    matriz[42][34] =560;

    matriz[30][35] =640;
    matriz[34][35] =440;
    matriz[36][35] =180;
    matriz[37][35] =230;

    matriz[35][36] =180;
    matriz[37][36] =30;

    matriz[30][37] =860;
    matriz[35][37] =230;
    matriz[36][37] =30;

    matriz[31][38] =650;
    matriz[39][38] =170;
    matriz[40][38] =150;

    matriz[38][39] = 170;
    matriz[40][39] =210;

    matriz[31][40] =640;
    matriz[38][40] =150;
    matriz[39][40] =210;

    matriz[26][41] =700;
    matriz[31][41] =530;
    matriz[32][41] =590;
    matriz[40][41] =240;
    matriz[42][41] =200;

    matriz[33][42] =710;
    matriz[34][42] =560;
    matriz[41][42] =200;
}

void imprimir_cidades(char cidades [43][24])
{
    int i=0;

    system("clear");

    printf("\n\t\t\t       TOUR NOS ESTADOS UNIDOS\n\n");

    for(i=1; i<15; i++)
    {
        printf("%3d %-20s \t %3d %-20s \t %3d %-20s \n", i, cidades[i], (i+14), cidades[i+14], (i+28), cidades[i+28]);
    }
}

int imprimir_menu(int matriz[tamanhoDaMatriz][tamanhoDaMatriz], int matriz_aux[tamanhoDaMatriz][tamanhoDaMatriz], char cidades [tamanhoDaMatriz][tamanhoMaiorPalavra])
{
    int menu;

    do
    {
        system("clear");
        printf("\n 1 - TRAJETO ENTRE TODAS AS CIDADES DO MAPA;\n");
        printf(" 2 - MENOR PERCUSO ENTRE DUAS CIDADES;\n");
        printf(" 3 - ESCOLHER UM SUBCONJUNTO DAS CIDADES PRESENTES NO MAPA PARA SEREM VISITAS.\n\n");
        printf(" DIGITE A OPCAO DESEJADA: ");
        __fpurge(stdin);
        scanf("%d", &menu);
        __fpurge(stdin);
    }
    while(menu != 1 && menu !=2 && menu != 3);

    matriz_adjacente(matriz, matriz_aux);
    imprimir_cidades(cidades);

    return menu;
}

int imprimir_submenu(char cidades [43][24], int cidades_subconjunto[])
{
    int menu, i=0;

    do
    {
        // system("clear");
        printf("\n 1 - TRAJETO ENTRE TODAS AS CIDADES DO SUBCONJUNTO;\n");
        printf(" 2 - MENOR PERCUSO ENTRE DUAS CIDADES DO SUBCONJUNTO.\n\n");

        printf(" DIGITE A OPCAO DESEJADA: ");
        __fpurge(stdin);
        scanf("%d", &menu);
        __fpurge(stdin);
    }
    while(menu != 1 && menu != 2);

    printf("\n");
    while(i<15)
    {
        if(cidades_subconjunto[i] != 0 && cidades_subconjunto[i+14] == 0 && cidades_subconjunto[i+28] == 0)
        {
            printf("%3d %-20s\n", cidades_subconjunto[i], cidades[cidades_subconjunto[i]]);
        }
        else
        {
            if(cidades_subconjunto[i]!= 0 && cidades_subconjunto[i+14]!=0 && cidades_subconjunto[i+28]==0)
            {
                printf("%3d %-20s \t %3d %-20s\n", cidades_subconjunto[i], cidades[cidades_subconjunto[i]], cidades_subconjunto[i+14], cidades[cidades_subconjunto[i+14]]);
            }
            else
            {
                if(cidades_subconjunto[i]!= 0 && cidades_subconjunto[i+14]!=0 && cidades_subconjunto[i+28]!=0)
                    printf("%3d %-20s \t %3d %-20s \t %3d %-20s\n", cidades_subconjunto[i], cidades[cidades_subconjunto[i]], cidades_subconjunto[i+14], cidades[cidades_subconjunto[i+14]], cidades_subconjunto[i+28], cidades[cidades_subconjunto[i+28]]);
            }
        }
        i++;
    }

    return menu;
}

void converter_dias(int dias_totais, int *dia, int *mes, int *ano)
{

    int aux;

    while(dias_totais!=0)
    {
        if(dias_totais<30)
        {
            aux=dias_totais;
            dias_totais=0;
        }
        else
        {
            aux=30;
            dias_totais=(dias_totais-30);
        }

        if((aux+*dia) > 30)
        {
            *mes=*mes+1;
            *dia=((aux+*dia)-30);
        }
        else
        {
            if((aux+*dia) == 30)
            {
                *mes=*mes+1;
                *dia=0;
            }
            else
            {
                if((aux+*dia) < 30)
                {
                    *dia=(aux+*dia);
                }
            }
        }

        if(*mes>12)
        {
            *mes=0;
            *ano=*ano+1;
        }
    }

    if(*dia>0 && *mes==0 && *ano==0)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d DIAS", *dia);
    }
    else if(*dia>0 && *mes==1 && *ano==0)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 MES E %d DIAS", *dia);
    }
    else if(*dia>0 && *mes>1 && *ano==0)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d MESES E %d DIAS", *mes, *dia);
    }
    else if(*dia==0 && *mes==1 && *ano==0)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 MES");
    }
    else if(*dia==0 && *mes>1 && *ano==0)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d MESES", *mes);
    }
    else if(*dia==0 && *mes==0 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO");
    }
    else if(*dia==0 && *mes==1 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO E 1 MES");
    }
    else if(*dia==0 && *mes>1 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO E %d MESES", *mes);
    }
    else if(*dia==1 && *mes>1 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO, %d MESES E 1 DIA", *mes);
    }
    else if(*dia>1 && *mes>1 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO, %d MESES E %d DIAS", *mes, *dia);
    }
    else if(*dia>1 && *mes==0 && *ano==1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: 1 ANO E %d DIAS", *dia);
    }
    else if(*dia==0 && *mes==0 && *ano>1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d ANOS", *ano);
    }
    else if(*dia>0 && *mes==0 && *ano>1)
    {
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d ANOS E %d DIAS", *ano, *dia);
    }
    else
        printf("\n TEMPO TOTAL DO TOUR NOS ESTADOS UNIDOS: %d ANOS, %d MESES E %d DIAS", *ano, *mes, *dia);
}

int verificar_numero(char c)
{
    if(c == 48)
        return 1;
    else if(c == 49)
        return 1;
    else if(c == 50)
        return 1;
    else if(c == 51)
        return 1;
    else if(c == 52)
        return 1;
    else if(c == 53)
        return 1;
    else if(c == 54)
        return 1;
    else if(c == 55)
        return 1;
    else if(c == 56)
        return 1;
    else if(c == 57)
        return 1;
    else
        return 0;
}

int imprimir_opcao_diaria(int menu, int dias[], char cidades[tamanhoDaMatriz][tamanhoMaiorPalavra], int cidades_sub[], int subconjunto, int *dia,int *mes,int *ano)
{
    int flag=0, i=0;
    char entrada;

    printf("\n");
    do
    {
        __fpurge(stdin);
        printf(" DESEJA ESPECIFICAR QUANTOS DIAS VAI FICAR EM CADA DESTINO <S/N>? ");
        __fpurge(stdin);
        scanf("%c", &entrada);
        __fpurge(stdin);
    }
    while(entrada != 's' && entrada != 'n' && entrada != 'S' && entrada != 'N');

    if(entrada == 's' || entrada == 'S')
    {
        do
        {
            system("clear");
            printf("\n      DATA DE CHEGADA NOS ESTADOS UNIDOS ");
            __fpurge(stdin);
            printf("\n\n DIGITE O DIA: ");
            scanf("%d", dia);
            __fpurge(stdin);
            printf(" DIGITE O MES: ");
            scanf("%d", mes);
            __fpurge(stdin);
            printf(" DIGITE O ANO: ");
            scanf("%d", ano);
            __fpurge(stdin);

            flag=0;
            printf("\n");

            if(*dia>31)
                flag=1;
            else if(*dia>28 && *mes==2)
                flag=1;
            else if(*dia>30 && (*mes == 4 || *mes == 6 || *mes == 9 || *mes == 11))
                flag=1;
            if(*mes < 0 || *mes > 12)
                flag=1;
            else if(*ano<2019 || *ano > 2050)
                flag=1;
            else
                flag=0;
        }
        while(flag==1);


        if(menu == 1)
        {
            i=1;

            while(i<tamanhoDaMatriz)
            {
                do
                {
                    printf(" DIGITE QUANTOS DIAS PRETENDE FICAR EM %s: ", cidades[i]);
                    scanf("%d", &dias[i]);
                    i++;
                }
                while(dias[i] < 0);
            }
        }
        else
        {
            if(menu==3)
            {

                i=1;

                while(i<subconjunto+1)
                {
                    do
                    {
                        printf(" DIGITE QUANTOS DIAS PRETENDE FICAR EM %s: ", cidades[cidades_sub[(i-1)]]);
                        scanf("%d", &dias[cidades_sub[(i-1)]]);
                        i++;
                    }
                    while(dias[i] < 0);
                }
            }
        }
        return 1;
    }
    return 0;
}

void calcular_saida(int qtd_dia, int *dia, int *mes, int *ano, int *dias_totais)
{
    int qtdd_dias;
    *dias_totais = (*dias_totais+qtd_dia);

    while(qtd_dia !=0)
    {
        if(qtd_dia<30)
        {
            qtdd_dias=qtd_dia;
            qtd_dia=0;
        }
        else
        {
            qtdd_dias=30;
            qtd_dia = (qtd_dia-30);
        }

        if((qtdd_dias+*dia) > 28 && *mes == 2)
        {
            *mes=3;
            *dia = (qtdd_dias+*dia)-28;
        }
        else
        {
            if((qtdd_dias+*dia) > 30 && (*mes == 4 || *mes == 6 || *mes == 9 || *mes == 11))
            {
                *mes=*mes+1;
                *dia = (qtdd_dias+*dia)-30;
            }
            else
            {
                if((qtdd_dias+*dia) > 31 && (*mes != 4 || *mes != 6 || *mes != 9 || *mes != 11))
                {
                    *mes=*mes+1;
                    *dia = (qtdd_dias+*dia)-31;
                }
                else
                {
                    *dia=*dia+qtdd_dias;
                }
            }
        }

        if(*mes == 13)
        {
            *ano=*ano+1;
            *mes=1;
        }
    }
}

void BublleSort (int Vetor[], int Tamanho)
{
    int Cont1=0, Cont2=0, Auxiliar=0;

    for(Cont1=0; Cont1<(Tamanho-1); Cont1++)
    {
        for(Cont2=(Tamanho-2); Cont2>=Cont1; Cont2--)
        {
            if(Vetor[Cont2] > Vetor[Cont2+1])
            {
                Auxiliar = Vetor[Cont2];
                Vetor[Cont2] = Vetor[Cont2+1];
                Vetor[Cont2+1]=Auxiliar;
            }
        }
    }
}

void copiar_Matriz(int matriz[tamanhoDaMatriz][tamanhoDaMatriz], int matriz_aux[tamanhoDaMatriz][tamanhoDaMatriz])
{
    int i=0, j=0;
    for(i=0; i<tamanhoDaMatriz; i++)
    {
        for(j=0; j<tamanhoDaMatriz; j++)
        {
            matriz[i][j] = matriz_aux[i][j];
        }
    }
}

void opcao_de_entrada(int menu_anterior, int menu, int *entrada, int *saida, int cidades_subconjunto[], int subconjunto)
{
    if(menu_anterior == 1 || menu_anterior == 2)
    {
        do
        {
            printf("\n DIGITE O NUMERO REFERENTE A CIDADE DE PARTIDA: ");
            scanf("%d", entrada);
            __fpurge(stdin);
        }
        while(*entrada < 1 || *entrada > 42);

        if(menu==1)
        {
            *saida= *entrada;
        }
        else
        {
            do
            {
                printf("\n DIGITE O NUMERO REFERENTE A CIDADE DE CHEGADA, (DIFERENTE DA DE PARTIDA): ");
                scanf("%d", saida);
                __fpurge(stdin);
            }
            while(*entrada == *saida || *saida < 1 || *saida > 42);
        }
    }
    else
    {
        int i=0, flag=0;

        do
        {
            printf("\n DIGITE O NUMERO REFERENTE A CIDADE DE PARTIDA: ");
            scanf("%d", entrada);
            __fpurge(stdin);

            while(i<subconjunto)
            {
                if(*entrada == cidades_subconjunto[i])
                    flag=1;
                i++;
            }
        }
        while(flag!=1);

        if(menu==1)
        {
            *saida = *entrada;
        }
        else
        {
            do
            {
                printf("\n DIGITE O NUMERO REFERENTE A CIDADE DE CHEGADA, (DIFERENTE DA DE PARTIDA): ");
                scanf("%d", saida);
                __fpurge(stdin);

                i=0;
                flag=0;


                while(i<subconjunto)
                {
                    if(*saida == cidades_subconjunto[i])
                        flag=1;
                    i++;
                }

            }
            while(*entrada == *saida || flag != 1);
        }
    }
}

int opcao_subconjunto(int matriz[tamanhoDaMatriz][tamanhoDaMatriz], int matriz_aux[tamanhoDaMatriz][tamanhoDaMatriz], char cidades [tamanhoDaMatriz][tamanhoMaiorPalavra], int cidades_subconjunto[], int *subconjunto)
{
    int flagCidadeLink=0, cont1=0, cont2=0, cont3=0, flag=0, i=0;

    do
    {
        do
        {
            printf("\n DIGITE QUANTAS CIDADES PERTENCEM A ESSE SUBCONJUNTO DESEJADO: ");
            scanf("%d", subconjunto);
            __fpurge(stdin);
        }
        while(*subconjunto < 1 || *subconjunto > 42);

        printf("\n");

        cont1=0;
        while(cont1<*subconjunto) // Verificar se todas as cidades estaoo conectadas
        {
            do
            {
                i=0;
                flag=0;
                printf(" DIGITE O NUMERO CORRESPONDENTE A CIDADE DE NUMERO %d DO SUBCONJUNTO: ", cont1+1);
                scanf("%d", &cidades_subconjunto[cont1]);
                __fpurge(stdin);

                while(i<cont1)
                {
                    if(cidades_subconjunto[cont1]==cidades_subconjunto[i])
                        flag=1;
                    i++;
                }
            }
            while(flag==1);
            cont1++;
        }

        flagCidadeLink=0;
        cont1=0, cont2=1, cont3=0;

        while(cont3<*subconjunto && flagCidadeLink!=2)
        {
            while(cont2<*subconjunto)
            {
                if(matriz[cidades_subconjunto[cont1]][cidades_subconjunto[cont2]]!=0)
                {
                    flagCidadeLink=1;
                    matriz_aux[cidades_subconjunto[cont1]][cidades_subconjunto[cont2]] = matriz[cidades_subconjunto[cont1]][cidades_subconjunto[cont2]];
                }
                cont2++;
            }
            if(flagCidadeLink!=0)
            {
                cont3+=1;
                cont1=cont3;
                cont2=0;
                flagCidadeLink=0;
            }
            else
            {
                printf("\n ERRO!!! ALGUMA CIDADE DIGITADA NAO TEM CONEXAO COM A OUTRA!");
                flagCidadeLink=2;
            }
        }
    }
    while(flagCidadeLink==2);

    copiar_Matriz(matriz, matriz_aux);

    BublleSort (cidades_subconjunto, *subconjunto);

    return imprimir_submenu(cidades, cidades_subconjunto);
}


