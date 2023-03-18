/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:50:06 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 20:43:32 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <iostream>


Client::Client(int socket, const std::string& nickname, const std::string& username)
    : socket_(socket), nickname_(nickname), username_(username), markedForDisconnection_(false), authenticated_(false) {
}

int Client::getSocket() const {
    return socket_;
}

const std::string& Client::getNickname() const {
    return nickname_;
}

const std::string& Client::getUsername() const {
    return username_;
}

void Client::setNickname(const std::string& nickname) {
    nickname_ = nickname;
}

void Client::setUsername(const std::string& username) {
    username_ = username;
}

bool Client::isMarkedForDisconnection() const {
    return markedForDisconnection_;
}

void Client::markForDisconnection() {
    markedForDisconnection_ = true;
}

Client::Client(int socket, const std::string& password)
    : socket_(socket), password_(password), markedForDisconnection_(false), authenticated_(false) {
}

Client::Client(int socket)
    : socket_(socket), markedForDisconnection_(false), authenticated_(false) {
    // You can initialize other members here if necessary
}

bool Client::isAuthenticated() const {
    return authenticated_;
}

void Client::authenticate(const std::string& password) {
    if (password == password_) {
        authenticated_ = true;
    }
}

void Client::sendMessage(const std::string& message) {
    // Send the message through the socket
    int num_bytes_sent = send(socket_, message.c_str(), message.length(), 0);
    if (num_bytes_sent == -1) {
        // Error handling
        std::cout << "Failed to send message to client with socket " << socket_ << std::endl;
    }
}