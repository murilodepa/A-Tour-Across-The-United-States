#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include "tipo.h"

main()
{
	int sock, length,tam,cont =1;
	struct sockaddr_in name;
	struct mensagem msg;
	char buf[1024];

        /* Cria o socket de comunicacao */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock<0) {
	/*
	/- houve erro na abertura do socket
	*/
		perror("opening datagram socket");
		exit(1);
	}
	/* Associa */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = 0;
	if (bind(sock,(struct sockaddr *)&name, sizeof name ) < 0) {
		perror("binding datagram socket");
		exit(1);
	}
        /* Imprime o numero da porta */
	length = sizeof(name);
	if (getsockname(sock,(struct sockaddr *)&name, &length) < 0) {
		perror("getting socket name");
		exit(1);
	}
	printf("Socket port #%d\n",ntohs(name.sin_port));

	/* Le */
	//if (read(sock,buf,1024)<0)
        //        perror("receiving datagram packet");
	while(1){
	msg.contador++;
	recvfrom(sock,(char*)&msg,sizeof msg,0,(struct sockaddr*)&name,&tam);
	printf("%d,%d,%x \n",name.sin_port,name.sin_family,name.sin_addr.s_addr);
	printf("IP: %s\n" ,inet_ntoa(name.sin_addr));
        printf("  %s\n", buf);
	msg.contador=cont++;
	printf("\n \n \n \n \n");

	sendto(sock,(char*)&msg,sizeof msg,0,(struct sockaddr *)&name,sizeof name);
	}
        close(sock);
        exit(0);
}
