/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 19:02:39 by chajjar           #+#    #+#             */
/*   Updated: 2023/03/18 19:11:40 by chajjar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include <iostream>
#include <thread>

#include "IRCServer.hpp"

#include <iostream>
#include <string>

#include "IRCServer.hpp"

int main() {
    int port = 6667;
    std::string password = "1234";

    IRCServer server(port, password);
    server.run();

    return 0;
}