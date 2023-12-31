//
// Created by yyy on 29/12/2023.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 18074

#define MAX_LINE 4096
#define SA struct sockaddr

/* fonction custom pour quitter le programme correctement (en cas d'erreur) */
// void err_n_die(const char *fmt, ...);

// ici, à chaque erreur,je vais simplement fprintf sur stderr "erreur", ET exit(1)

int main(int argc, char **argv){

    // variables locales
    int                 sockfd, n;
    int                 sendbytes;
    struct sockaddr_in  servaddr;
    char                sendline[MAX_LINE];
    char                recvline[MAX_LINE];

    // check de bon usage du client
    if (argc != 2){
        fprintf(stderr, "mauvaise formulation, il faut : %s <adresse du serveur>\n", argv[0]);
        exit(1);
    }

    // création de la socket de stream (TCP)
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        // en gros, if == -1
        fprintf(stderr, "erreur lors de la création de la socket !, sockfd = %d\n", sockfd);
        exit(1);
    } else {
        fflush(stdout);
        fprintf(stdout, "socket AF_INET, SOCK_STREAM crée avec succès ! : sockfd = %d\n", sockfd);
    }

    fprintf(stdout, "here\n");

    // initialise tous les octets du bloc de mémoire pointé par servaddr à 0
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET,
    servaddr.sin_port   = htons(SERVER_PORT); // htons : host to network, short

    // convertir la représentation textuelle de l'IP du serveur en une représentation binaire équivalente
    // expl : "1.2.3.4" => [ 1 , 2 , 3 , 4 ]
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        fprintf(stderr, "erreur du inet_pton pour %s\n", argv[1]);
        exit(1);
    }

    // tentative de connexion au serveur
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
        fprintf(stderr, "la tentative de connexion a échouée !\n");
        exit(1);
    }

    // connecté, donc on prépare le message à envoyer (ici, requête GET)
    // 2x slash r slash n pour indiquer que c'est la fin de ma requête
    sprintf(sendline, "skibilidopdopyesyes\n");
    sendbytes = strlen(sendline);

    // envoi de la requête -- on s'assure qu'on envoit bien tout
    // ce code est fragile car il stoppe si seulement quelques bytes sont envoyés
    // normalement, on va réessayer, sauf si la valeur de retour est -1 (erreur)

    if (write(sockfd, sendline, sendbytes) != sendbytes){
        fprintf(stderr, "erreur d'écriture du message\n");
    }

    // ligne équivalente à celle bzero
    memset(recvline, 0, MAX_LINE);

    // lecture de la réponse du serveur
    while ( (n = read(sockfd, recvline, MAX_LINE-1)) > 0){
        memset(recvline, 0, MAX_LINE);
        printf("\n%s\n", recvline);
    }

    // affichage de la réponse en entier
    fprintf(stdout, "_réponse : %s_\n", recvline);

    if (n < 0){
        fprintf(stderr, "erreur de lecture de la réponse du serveur\n");
    }

    fprintf(stdout, "fin du programme !");
    fflush(stdout);


    exit(0); // fin, avec succès !

}

/*
void err_n_die(const char *fmt, ...){
    int errno_save;
    va_list ap;

    // n'importe quel appel système/librairie peut définir errno, donc on l'enregistre ici
    errno_save = errno;

    // print fmt+args sur la stdout
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // print le message d'erreur si errno a été set
    if (errno_save != 0){
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    va_end(ap);

    exit(1);

}
*/