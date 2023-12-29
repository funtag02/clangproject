//
// Created by yyy on 29/12/2023.
//

#ifndef CLANGPROJECT_COMMON_H
#define CLANGPROJECT_COMMON_H

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

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

// constantes utiles
#define SERVER_PORT 18074

#define MAX_LINE 4096
#define SA struct sockaddr

// char *bin2hex
// (méthode pour convertir un binaire en hexadécimal, puis l'afficher)

// char err_n_die

#endif //CLANGPROJECT_COMMON_H
