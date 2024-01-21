//
// Created by yyy on 29/12/2023.
//

#include "common.h"
#include <strings.h>

int main(int argc, char **argv){
    int                 clientfd, connfd, n; // connexion file descriptor : socket info à propos du client
    struct sockaddr_in  servaddr;
    uint8_t             recvline[MAX_LINE+1];
    int                 nb_connexions = 0;

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

    // écoute pour la réponse du client (pour la connexion, pas la transmission)
    if ((listen(clientfd, 10)) < 0){
        fprintf(stderr, "erreur d'écoute du client, côté serveur\n");
        exit(1);
    }

    // boucle émission-reception
    for ( ; ; ){
        struct sockaddr_in  addr;
        socklen_t           addr_len;
        char                client_adress[MAX_LINE+1];

        char                *current_file = "";
        char                *files_list[5] = {
                                "dsstats_exo1(2_3)_noemie.jpg",
                                "dsstats_exo1(3)_raph.jpg",
                                "screen mysql cli.jpg",
                                "logo uca.jpeg",
                                "2012-03-12-Drone-Parrot.jpg"
                            };
        /*
        FILE                *fileptr = fopen(current_file, "rb"); // readmode, binary

        // définition du fichier à traîter dans la requête : à envoyer
        current_file = files_list[nb_connexions];

        if (fileptr == NULL){
            perror("fopen");
            printf("\nCouldn't open file %s\n", current_file);
            exit(1);
        }
         */

        // accepte des blocs jusqu'à ce qu'une nouvelle connexion arrive
        // retourne une description du fichier à la conneixon

        fprintf(stdout, "en attente d'une connexion sur le port %d\n", SERVER_PORT);
        fflush(stdout);
        connfd = accept(clientfd, (SA *) &addr, &addr_len);

        // network format => presentation format
        inet_ntop(AF_INET, &addr, client_adress, MAX_LINE);
        fprintf(stdout, "Connexion du client : %s\n", client_adress);
        nb_connexions++;

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

        // print du message du client
        fprintf(stdout, "message : %s\n", recvline);

        // envoi du fichier jpeg à la socket (direct)
        //sendFileToSocket(fileptr, connfd);

        // version 2 envoi du fichier jpeg à la socket (via http)
        // serveImage(fileptr, connfd);

        // taille de la loop à fixer (ou faire une while not null)
        for (int j = 0; j < 5; j++){
            fprintf(stdout, "\nFILE %d : %s\n", j, files_list[j]);
            serveImage(fopen(files_list[j], "rb"), connfd);
            sleep(2);
        }

        // fermeture de la socket
        fprintf(stdout, "closing socket %d on port %d\n", nb_connexions, SERVER_PORT);
        if (close(connfd) == -1){
            perror("close connfd");
        }
    }

    return 0;

}

// de chatgpt
void serveImage(FILE *file, int clientSocket) {

    int filesize = sizeof(file);
    // char *filebuff[filesize];

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Construire la réponse HTTP avec le type de contenu
    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: image/jpeg\r\n\r\n");

    // Envoyer l'en-tête de la réponse au client
    send(clientSocket, response, strlen(response), 0);

    // Lire et envoyer le contenu du fichier
    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    fclose(file);
}

/*
 void sendFileToSocket(FILE *file, int socket) {
    // Lire la taille totale du fichier
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allouer un tampon pour stocker le fichier entier
    char *buffer = (char *)malloc(file_size);
    if (buffer == NULL) {
        perror("Erreur lors de l'allocation du tampon");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Lire le fichier dans le tampon
    size_t bytesRead = fread(buffer, 1, file_size, file);
    if (bytesRead != file_size) {
        perror("Erreur lors de la lecture du fichier");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Envoyer le contenu du fichier à la socket
    ssize_t sentBytes = send(socket, buffer, file_size, 0);
    if (sentBytes == -1) {
        perror("Erreur lors de l'envoi des données sur la socket");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    printf("Nombre d'octets envoyés : %zd\n", sentBytes);

    // Libérer la mémoire et fermer le fichier
    free(buffer);
    fclose(file);
}
 */
