//
// Created by yyy on 29/12/2023.
//

#include "common.h"
#include <strings.h>

int main(int argc, char **argv){
    int                 clientfd, connfd, n;
    struct sockaddr_in  servaddr;
    uint8_t             buff[MAX_LINE+1];
    uint8_t             recvline[MAX_LINE+1];

    // internet socket, TCP (stream)
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "\nerreur lors de la création de la socket !\n");
    } else {
        fprintf(stdout, "\nsocket AF_INET, SOCK_STREAM crée\n");
    }

    // setup de l'adresse :
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family =       AF_INET;            // je spécifie que c'est une adresse internet
    servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);  // INADDR_ANY : j'accepte toutes les connexions
    servaddr.sin_port =         htons(SERVER_PORT); // le port du serveur est défini dans le fichier common.h

    // bind de l'adresse à laquelle on écoute, à celle du serveur
    if ((bind(clientfd, (SA *) &servaddr, sizeof(servaddr))) < 0){
        fprintf(stderr, "erreur de bind entre le client et le serveur\n");
        exit(1);
    }

    if ((listen(clientfd, 10)) < 0){
        fprintf(stderr, "erreur d'écoute du client, côté serveur\n");
        exit(1);
    }

    // boucle émission-reception
    for ( ; ; ){
        struct sockaddr_in  addr;
        socklen_t           addr_len;
        char                client_adress[MAX_LINE+1];

        // accepte des blocs jusqu'à ce qu'une nouvelle connexion arrive
        // retourne une description du fichier à la conneixon

        fprintf(stdout, "en attente d'une connexion sur le port %d\n", SERVER_PORT);
        fflush(stdout);
        connfd = accept(clientfd, (SA *) &addr, &addr_len);

        // network format => presentation format
        inet_ntop(AF_INET, &addr, client_adress, MAX_LINE);
        fprintf(stdout, "Connexion du client : %s\n", client_adress);

        // zero out le buffer de réception pour bien s'assurer qu'il se finisse par un null
        memset(recvline, 0, MAX_LINE);

        // lire le message du client
        while( (n = read(connfd, recvline, MAX_LINE-1) ) > 0){
            // fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);

            // workaround pour trouver la fin du message
            if (recvline[n-1] == '\n'){
                break;
            }

            memset(recvline, 0, MAX_LINE);
        }

        if (n < 0){
            fprintf(stderr, "erreur de lecture du message du client\n");
            exit(1);
        }

        // envoi de la réponse
        snprintf( (char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");

        // NOTE : il faudrait checker manuellement les valeurs de write et close pour vérifier si y'a aucune erreur
        // Pour l'instant, je les ignore

        write(connfd, (char *)buff, strlen((char *)buff));
        close(connfd);

    }

}
