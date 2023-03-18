/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 13:52:41 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 23:42:13 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include "Client.hpp"

class IRCServer;

class Channel {
public:
    Channel(const std::string& name, IRCServer* server);

    const std::string& getName() const;

    bool hasClient(Client* client) const;
    void sendMessage(Client* from, const std::string& message);

    void addClient(Client* client);
    void removeClient(Client* client);
    void broadcastMessage(const std::string& sender, const std::string& message);
    explicit Channel(const std::string& name);
private:
    std::string name_;
    std::map<std::string, Client*> clients_;
    IRCServer* server_;
};

#endif // CHANNEL_HPP