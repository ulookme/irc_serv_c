/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:43:49 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/19 05:02:14 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <sstream>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>


IRCServer::IRCServer(const std::string& address, int port, const std::string& password)
    : port_(port), password_(password) {
    serverSocket_ = createServerSocket(port_);
    setNonBlocking(serverSocket_);
}


bool IRCServer::init() {
    int opt = 1;

    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == -1) {
        perror("socket creation failed");
        return false;
    }

    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return false;
    }

    if (bind(serverSocket_, (struct sockaddr *)&serverAddress_, sizeof(serverAddress_)) < 0) {
        perror("bind failed");
        return false;
    }

    if (listen(serverSocket_, 10) < 0) {
        perror("listen failed");
        return false;
    }

    return true;
}

//IRCServer::IRCServer(int port, const std::string& password)
//    : port_(port), password_(password) {
//    serverSocket_ = createServerSocket(port);
//    setNonBlocking(serverSocket_);
//}

IRCServer::~IRCServer() {
    close(serverSocket_);

    for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        delete *it;
    }

    channels_.clear();
}

void IRCServer::run() {
    fd_set read_fds;
    int max_fd;

    // Ajouter un message pour indiquer que le serveur est prêt et à l'écoute
    std::cout << "Server is running and listening on port " << port_ << std::endl;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(serverSocket_, &read_fds);
        max_fd = serverSocket_;

        for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
            int client_fd = (*it)->getSocket();
            FD_SET(client_fd, &read_fds);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
        }

        timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int select_result = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (select_result < 0) {
            perror("select");
            continue;
        }

        if (FD_ISSET(serverSocket_, &read_fds)) {
            acceptNewClient();
        }

        for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
            int client_fd = (*it)->getSocket();
            if (FD_ISSET(client_fd, &read_fds)) {
                handleClient(*it);
            }
        }

        // Gérer la déconnexion des clients ici, par exemple en utilisant une liste de clients à supprimer.
        for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end();) {
            Client* client = *it;
            if (client->isMarkedForDisconnection()) {
                disconnectClient(client);
                it = clients_.erase(it);
            } else {
                ++it;
            }
        }
    }
}


int IRCServer::createServerSocket(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    memset(&serverAddress_, 0, sizeof(serverAddress_));
    serverAddress_.sin_family = AF_INET;
    serverAddress_.sin_addr.s_addr = INADDR_ANY;
    serverAddress_.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&serverAddress_, sizeof(serverAddress_)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return server_socket;
}

void IRCServer::setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        exit(1);
    }
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) < 0) {
        perror("fcntl");
        exit(1);
    }
}

void IRCServer::acceptNewClient() {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    int client_fd = accept(serverSocket_, (struct sockaddr*)&client_addr, &addr_size);
    if (client_fd < 0) {
        perror("accept");
        return;
    }

    // Mettre le socket en mode non bloquant
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        close(client_fd);
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(client_fd, F_SETFL, flags) < 0) {
        perror("fcntl F_SETFL");
        close(client_fd);
        return;
    }

    // Créer un nouvel objet Client et l'ajouter à la liste des clients
    Client* new_client = new Client(client_fd);
    clients_.push_back(new_client);

    std::cout << "Nouveau client connecté: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
}


void IRCServer::handleClient(Client* client) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    int bytes_received = recv(client->getSocket(), buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("recv");
            // Marquer le client pour déconnexion
            client->markForDisconnection();
        }
        return;
    }

    if (bytes_received == 0) {
        // Le client s'est déconnecté
        std::cout << "Client déconnecté: " << client->getSocket() << std::endl;
        // Marquer le client pour déconnexion
        client->markForDisconnection();
        return;
    }

    buffer[bytes_received] = '\0';
    std::string received_data(buffer);

    // Traiter les commandes reçues
    // Vous pouvez diviser les données reçues en lignes et traiter chaque ligne comme une commande individuelle
    std::istringstream data_stream(received_data);
    std::string command_line;
    while (std::getline(data_stream, command_line, '\n')) {
        processClientCommand(client, command_line);
    }
}


void IRCServer::disconnectClient(Client* client) {
    // Fermer la socket du client
    close(client->getSocket());

    // Supprimer le client de la liste des clients actifs
    std::vector<Client*>::iterator it = std::find(clients_.begin(), clients_.end(), client);
    if (it != clients_.end()) {
        clients_.erase(it);
    }

    // Supprimer le client de tous les channels auxquels il est connecté
    for (std::map<std::string, Channel*>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
        it->second->removeClient(getClient(client->getNickname()));
    }

    // Libérer la mémoire allouée pour le client
    delete client;
    std::cout << "Client déconnecté et supprimé avec succès." << std::endl;
}

void IRCServer::processClientCommand(Client* client, const std::string& command_line) {
    std::istringstream cmd_stream(command_line);
    std::string command;
    cmd_stream >> command;

    if (command == "PASS") {
        std::string password;
        cmd_stream >> password;
        client->authenticate(password);
        if (client->isAuthenticated()) {
            // Envoyer un message indiquant que l'authentification a réussi
            std::string response = "Authentification réussie.\n";
            send(client->getSocket(), response.c_str(), response.size(), 0);
        } else {
            // Envoyer un message indiquant que l'authentification a échoué
            std::string response = "Erreur d'authentification.\n";
            send(client->getSocket(), response.c_str(), response.size(), 0);
        }
    } else if (client->isAuthenticated()) {
        if (command == "NICK") {
            std::string nickname;
            cmd_stream >> nickname;
            client->setNickname(nickname);
            // Envoyer un message indiquant que le surnom a été mis à jour
            std::string response = "Surnom mis à jour: " + nickname + "\n";
            send(client->getSocket(), response.c_str(), response.size(), 0);
        } else if (command == "USER") {
            std::string username, hostname, realname;
            cmd_stream >> username >> hostname >> realname;
            client->setUsername(username);
            // Envoyer un message indiquant que le nom d'utilisateur a été mis à jour
            std::string response = "Nom d'utilisateur mis à jour: " + username + "\n";
            send(client->getSocket(), response.c_str(), response.size(), 0);
        } else if (command == "JOIN") {
            std::string channel_name;
            cmd_stream >> channel_name;
            joinChannel(new Client(*client), channel_name);
            // Envoyer un message indiquant que le client a rejoint le canal
            std::string response = "Vous avez rejoint le canal: " + channel_name + "\n";
            send(client->getSocket(), response.c_str(), response.size(), 0);
        } else if (command == "PRIVMSG") {
            std::string target, message;
            cmd_stream >> target;
            std::getline(cmd_stream, message);
            if (!message.empty() && message[0] == ' ') {
                message.erase(0, 1); // Supprime l'espace au début du message
            }

            if (target.empty() || message.empty()) {
                // Erreur : cible ou message vide
            } else if (target[0] == '#') {
                sendMessageToChannel(new Client(*client), target, message);
            } else {
                std::vector<Client*>::iterator it;
                for (it = clients_.begin(); it != clients_.end(); ++it) {
                    if ((*it) != client && (*it)->getNickname() == target) {
                        sendPrivateMessage(client, (*it), message);
                        return;
                    }
                }
                // Envoyer un message indiquant que la cible n'existe pas
                std::string response = "La cible n'existe pas.\n";
                send(client->getSocket(), response.c_str(), response.size(), 0);
            }
        }
        // Ajoutez d'autres commandes ici en utilisant la structure if-else
    } else {
        // Envoyer un message indiquant que le client doit s'authentifier
        std::string response = "Veuillez vous authentifier avec la commande PASS.\n";
        send(client->getSocket(), response.c_str(), response.size(), 0);
    }
}

//Channel* IRCServer::createChannel(const std::string& channel_name) {
//    Channel* channel = new Channel(channel_name);
//    channels_[channel_name] = channel;
//    return channel;
//}

void IRCServer::sendPrivateMessage(Client* from, Client* to, const std::string& message) {
    if (from && to) {
        std::string formattedMessage = from->getNickname() + ": " + message;
        to->sendMessage(formattedMessage);
    } else {
        // Gérer le cas où le destinataire n'est pas connecté
        if (from) {
            std::string errorMessage = "Erreur: le destinataire n'est pas connecté.";
            from->sendMessage(errorMessage);
        } else {
            // Gérer le cas où l'expéditeur n'est pas connecté
            std::cerr << "Erreur: l'expéditeur n'est pas connecté." << std::endl;
        }
    }
}
void IRCServer::sendChannelMessage(Client* from, const std::string& channel_name, const std::string& message) {
    std::map<std::string, Channel*>::iterator channel_iter;
    channel_iter = channels_.find(channel_name);
    if (channel_iter != channels_.end()) {
        Channel* channel = channel_iter->second;
        channel->sendMessage(from, message);
    }
}

void IRCServer::sendToClient(Client* client, const std::string& message) {
    send(client->getSocket(), message.c_str(), message.size(), 0);
}

void IRCServer::joinChannel(Client* client, const std::string& channel_name) {
    std::map<std::string, Channel*>::iterator channel_iter = channels_.find(channel_name);
    Channel* channel;
    if (channel_iter == channels_.end()) {
        Channel* new_channel = new Channel(channel_name);
        channels_[channel_name] = channel;
    } else {
        channel = channel_iter->second;
    }
    channel->addClient(client);
}

void IRCServer::leaveChannel(Client* client, const std::string& channel_name) {
    std::map<std::string, Channel*>::iterator channel_iter = channels_.find(channel_name);
    if (channel_iter != channels_.end()) {
        channel_iter->second->removeClient(client);
    }
}

void IRCServer::sendMessageToChannel(Client* sender, const std::string& channel_name, const std::string& message) {
    std::map<std::string, Channel*>::iterator channel_iter = channels_.find(channel_name);
    if (channel_iter != channels_.end()) {
        Channel* channel = channel_iter->second;
        channel->broadcastMessage(sender->getNickname(), message);
    } else {
        // Handle the case where the channel does not exist
        std::string errorMessage = "Error: the channel " + channel_name + " does not exist.";
        sender->sendMessage(errorMessage);
    }
}


Client* IRCServer::getClient(const std::string& nickname) const {
    for (std::vector<Client*>::const_iterator it = clients_.begin(); it != clients_.end(); ++it) {
        if ((*it)->getNickname() == nickname) {
            return *it;
        }
    }
    return nullptr;
}
