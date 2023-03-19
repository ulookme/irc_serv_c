/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clicli.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 19:04:11 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/19 05:10:58 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port> <password>" << std::endl;
        return 1;
    }

    const char *server_address = argv[1];
    int port = std::stoi(argv[2]);
    std::string password = argv[3];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_address, &server_sockaddr.sin_addr) <= 0) {
        std::cerr << "Invalid server address" << std::endl;
        return 1;
    }

    if (connect(sock, reinterpret_cast<sockaddr *>(&server_sockaddr), sizeof(server_sockaddr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    // Send the password
    std::string password_message = "PASS " + password + "\r\n";
    send(sock, password_message.c_str(), password_message.length(), 0);

    // Send the NICK command
    std::string nickname = "my_nickname";
    std::string nick_message = "NICK " + nickname + "\r\n";
    send(sock, nick_message.c_str(), nick_message.length(), 0);

    // Send the USER command
    std::string username = "my_username";
    std::string realname = "My Real Name";
    std::string user_message = "USER " + username + " 0 * :" + realname + "\r\n";
    send(sock, user_message.c_str(), user_message.length(), 0);

    // Send the JOIN command
    std::string channel = "#test";
    std::string join_message = "JOIN " + channel + "\r\n";
    send(sock, join_message.c_str(), join_message.length(), 0);

    // Read and print server messages
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        ssize_t bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << buffer;
    }

    close(sock);
    return 0;
}