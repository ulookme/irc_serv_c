/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 19:02:39 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/19 23:33:46 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port> <password>" << std::endl;
        return 1;
    }

    const char *server_address = argv[1];
    int port = std::stoi(argv[2]);
    std::string password = argv[3];

    std::cout << "Server address: " << server_address << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Password: " << password << std::endl;
    
    try {
        IRCServer server(server_address, port, password);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}