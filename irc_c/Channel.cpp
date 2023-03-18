/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:53:35 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 23:42:29 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "IRCServer.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name, IRCServer* server)
    : name_(name), server_(server) {}

const std::string& Channel::getName() const {
    return name_;
}

bool Channel::hasClient(Client* client) const {
    return clients_.find(client->getNickname()) != clients_.end();
}

void Channel::sendMessage(Client* from, const std::string& message) {
    std::string response = "Message de " + from->getNickname() + " sur #" + name_ + ": " + message + "\n";
    for (std::map<std::string, Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        Client* client = it->second;
        if (client != from) {
            server_->sendToClient(client, response);
        }
    }
}

void Channel::addClient(Client* client) {
    clients_.insert(std::make_pair(client->getNickname(), client));
}

void Channel::removeClient(Client* client) {
    clients_.erase(client->getNickname());
}

void Channel::broadcastMessage(const std::string& sender, const std::string& message) {
    std::string formattedMessage = sender + ": " + message;
    for (std::map<std::string, Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        it->second->sendMessage(formattedMessage);
    }
}

Channel::Channel(const std::string& name) : name_(name) {}