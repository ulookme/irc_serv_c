/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:43:14 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/19 19:54:09 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sstream>
#include <sys/socket.h>
#include <map> // Remplace unordered_map par map pour la compatibilité C++98

class Client; // Déclaration anticipée de la classe Client
class Channel; // Déclaration anticipée de la classe Channel

class IRCServer {
public:
    IRCServer(const std::string& address, int port, const std::string& password);
    ~IRCServer();

    void run();

    void joinChannel(Client* client, const std::string& channel_name);
    void leaveChannel(Client* client, const std::string& channel_name);
    void sendMessageToChannel(Client* sender, const std::string& channel_name, const std::string& message);
    void sendToClient(Client* client, const std::string& message);
private:
    int createServerSocket(int port);
    void setNonBlocking(int sockfd);
    void acceptNewClient();
    void handleClient(Client* client);
    void disconnectClient(Client* client);
    void processClientCommand(Client* client, const std::string& command_line);
    Channel* createChannel(const std::string& channel_name);
    Channel* getOrCreateChannel(const std::string& channel_name);
    Client* getClient(const std::string& nickname) const;
    void sendPrivateMessage(Client* from, Client* to, const std::string& message);
    void sendChannelMessage(Client* from, const std::string& channel_name, const std::string& message);
    void removeClient(Client* client);
    //void sendToClient(Client* client, const std::string& message);
    bool init();
    int serverSocket_;
    int port_;
    std::string password_;
    struct sockaddr_in serverAddress_;
    std::vector<Client*> clients_;
    std::map<std::string, Channel*> channels_; // Utilise map au lieu d'unordered_map pour la compatibilité C++98
};

#endif // IRC_SERVER_HPP