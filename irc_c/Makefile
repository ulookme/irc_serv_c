# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chajjar <chajjar@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/18 19:12:11 by chajjar           #+#    #+#              #
#    Updated: 2023/03/19 19:41:37 by chajjar          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc_server

SRC = main.cpp IRCServer.cpp Client.cpp Channel.cpp
OBJ = $(SRC:.cpp=.o)
CC = c++
FLAGS =  -g -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ)
		$(CC) $(FLAGS) -o $(NAME) $(OBJ)

clean:
		rm -f $(OBJ)
fclean: clean 
		rm -f $(NAME)
re: clean all

.PHONY: all clean fclean re 