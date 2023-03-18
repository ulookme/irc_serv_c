/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:48:29 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 20:39:25 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
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
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <map>

class Client {
    
public:
    Client(int socket, const std::string& nickname, const std::string& username);
    Client(int socket, const std::string& password);
    Client(int socket);
    bool isAuthenticated() const;
    void authenticate(const std::string& password);
    int getSocket() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;

    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    bool isMarkedForDisconnection() const;
    void markForDisconnection();
    void sendMessage(const std::string& message);
    
private:

    int socket_;
    std::string nickname_;
    std::string username_;
    bool markedForDisconnection_;
    bool authenticated_; // Add a flag to check if the client is authenticated
    std::string password_;
};

#endif // CLIENT_HPP