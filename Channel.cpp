/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:53:35 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/19 23:23:21 by chajjar          ###   ########.fr       */
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
    broadcastMessage(from->getNickname(), response);
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

bool Channel::isOperator(Client* client) const {
    return operators_.find(client->getNickname()) != operators_.end();
}

void Channel::addOperator(Client* client) {
    operators_.insert(std::make_pair(client->getNickname(), client));
}

void Channel::removeOperator(Client* client) {
    operators_.erase(client->getNickname());
}

void Channel::setOperator(Client* client) {
    if (!client->isOperator()) {
        client->setOperator(true);
        operators_.insert(std::make_pair(client->getNickname(), client));
    }
}