/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "tipo.h"
#include "cidades.h"

#define tamanhoDaMatriz 43
#define tamanhoMaiorPalavra 24
#define infinito 99999999

struct caminho
{
    int caminho[tamanhoDaMatriz] = {0};
    int custo;
};
typedef struct caminho  stack_element;
#include "Stackld.h"

#include <time.h>

struct ordernar
{
    int caminho;
    int custo;
};
typedef struct ordernar ordenar;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void matriz_adjacente(int matriz[tamanhoDaMatriz][tamanhoDaMatriz]);


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno,aux;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    struct mensagem msg;


    int custo_min=infinito,i,j,k,g=0,flag=0,m=0,ale=0;
    int caminho_min[tamanhoDaMatriz]= {0},caminho_F[45]= {0};
    int matriz[tamanhoDaMatriz][tamanhoDaMatriz]= {0};

    int custo_res,flag2=0,flag3=0,flag4=0;
    int entrada,menu,saida;
    ordenar elementos_fila[42],troca;


    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // bzero(msg,sizeof msg);
    n = read(newsockfd,(struct msg*)&msg,sizeof msg);
    if (n < 0)
        error("ERROR reading from socket");

    printf("Here is the message: %d\n",msg.in);
    aux =msg.in; //atoi (buffer);
    printf("\n\n ---%i---\n",aux);
    entrada=aux;

    if(msg.tipo==1 || msg.tipo==2)
    {
        matriz_adjacente(matriz);
    }
    else
    {
        k=0;
        for(i=0; i<tamanhoDaMatriz; i++)
        {
            for(j=0; j<tamanhoDaMatriz; j++)
            {
                matriz[i][j] = msg.subconjunto[k];
                k+=1;
            }
        }
    }


    Stack q;
    initStack(q);
    stack_element x;
    x.caminho[0]=msg.in;
    x.custo=0;
    push(q,x);
    clock_t tInicio, tFim,tDecorrido;
    tInicio = clock();
    while(!isEmpty(q))
    {
        flag2=0;
        m=0;
        x=pop(q);
        i=0;
        while(x.caminho[i]!=0)
        {
            i++;
        }
        if(msg.tipo==1)
        {
            if(x.caminho[i-1]==msg.in && i==42)
            {

                if(x.custo<custo_min)
                {
                    custo_min=x.custo;
                    while(x.caminho[m]!=0)
                    {
                        caminho_min[m]=x.caminho[m];
                        m++;
                    }
                    break;
                }
            }
            else
            {
                flag2=1;
            }
        }
        else
        {
            if(x.caminho[i-1]==msg.out)
            {

                if(x.custo<custo_min)
                {
                    m=0;
                    custo_min=x.custo;
                    while(x.caminho[m]!=0)
                    {
                        caminho_min[m]=x.caminho[m];
                        m++;
                    }
                }
            }
            else
            {
                flag2=1;
            }

        }
        if(flag2==1)
        {
            m=0;
            j=x.caminho[i-1];
            for(k=1; k<tamanhoDaMatriz; k++)
            {
                if(matriz[j][k]!=0)
                {

                    if(matriz[0][k]==msg.in && i==41)
                    {
                        flag=0;
                    }
                    else
                    {
                        g=0;

                        while(x.caminho[g]!=0)
                        {
                            if(x.caminho[g]==matriz[0][k])
                            {
                                flag=1;
                                break;
                            }
                            g++;
                        }
                    }

                    if(flag==0)
                    {
                        elementos_fila[m].custo=matriz[j][k];
                        elementos_fila[m].caminho=matriz[0][k];
                        m++;
                    }
                    flag=0;
                }
            }
            for(g=0; g<m-1; g++)
            {
                for(k=m-2; g<k; k--)
                {
                    if(elementos_fila[k].custo>elementos_fila[k+1].custo)
                    {
                        troca=elementos_fila[k];
                        elementos_fila[k]=elementos_fila[k+1];
                        elementos_fila[k+1]=troca;
                    }
                }
            }


            if(rand()%100>2)
            {
                ale=0;
            }
            else
            {
                ale=1;
            }

            for(g=m-1; g>=ale; g--)
            {
                custo_res=x.custo;
                x.custo+=elementos_fila[g].custo;
                x.caminho[i]=elementos_fila[g].caminho;
                if(x.custo<custo_min)
                {
                    push(q,x);
                }
                x.caminho[i]=0;
                x.custo=custo_res;
            }


        }
    }

    i=1;
    if(custo_min!=99999)
    {
        msg.caminho_min[0]=caminho_min[0];
        while(caminho_min[i]!=msg.out)
        {
            msg.caminho_min[i]=caminho_min[i];
            i++;
        }
        msg.caminho_min[i]=caminho_min[i];
    }
    tFim = clock();
    tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC/1000));
    msg.tempo=tDecorrido;
    msg.custo_min=custo_min;



    n = write(newsockfd,(int*)&msg,5000);
    if (n < 0)
        error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);
    return 0;
}

void matriz_adjacente(int matriz[tamanhoDaMatriz][tamanhoDaMatriz])
{
    int i=0, j=0;

    for(i=1; i<tamanhoDaMatriz; i++)
    {
        matriz[0][i]=i;
        matriz[i][0]=i;
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

    //matriz[13][18] =350;

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


