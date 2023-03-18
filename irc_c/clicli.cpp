/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clicli.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 19:04:11 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 20:57:29 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int port = 6667;
    const char* server_ip = "127.0.0.1";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    std::string password;
    std::cout << "Entrez le mot de passe du serveur: ";
    std::cin >> password;

    std::string auth_message = "AUTH " + password + "\n";
    send(sockfd, auth_message.c_str(), auth_message.size(), 0);

    // Le client est maintenant connecté au serveur et a envoyé le mot de passe.
    // Vous pouvez implémenter une boucle pour lire les messages du clavier et les envoyer au serveur.

    close(sockfd);
    return 0;
}