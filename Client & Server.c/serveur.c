    /*  envoie d'une requete http serveur.c */
     
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

char buffer[512];
int my_socket;

int main(void){ //prog return 0; }

int client_socket;
struct sockaddr_in my_address, client_address;
int my_address_longueur, lg;

bzero(&my_address,sizeof(my_address));
my_address.sin_port = htons(port);
my_address.sin_family = AF_INET;
my_address.sin_addr.s_addr = htonl(INADDR_ANY);

//creation de socket
if ((my_socket = socket(AF_INET,SOCK_STREAM,0))== -1)
{
printf(“creation rate\n”);
exit(0);
}
signal(SIGINT,fin);
//bind serveur - socket
bind(my_socket,(struct sockaddr *)&my_address,sizeof(my_address));
my_address_longueur = sizeof(client_address);

//ecoute sur la socket 
listen(my_socket,5);

//accept la connexion
my_address_longueur = sizeof(client_address);
while(1)
{
client_socket = accept(ma_socket,
(struct sockaddr *)&client_address,
&my_address_longueur);

if (fork() == 0)
{
close(ma_socket);

lg = read(client_socket,buffer, 512);
printf("le serveur a recu: %s\n",buffer);
sprintf(buffer,"%s du serveur",buffer);
write(client_socket,buffer, 512);
shutdown(client_socket,2);
close(client_socket);
exit(0);

}
}
shutdown(ma_socket,2);
close(ma_socket);
}[/code]