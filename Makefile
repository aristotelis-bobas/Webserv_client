# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: abobas <abobas@student.codam.nl>             +#+                      #
#                                                    +#+                       #
#    Created: 2020/11/01 15:35:01 by abobas        #+#    #+#                  #
#    Updated: 2020/11/01 17:18:13 by abobas        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

SRC = 			main.cpp \
				Log.cpp \
				Client.cpp

SRC_DIR	=		./src/

SRC :=			$(SRC:%=$(SRC_DIR)%)
			
INCLUDE :=		$(INCLUDE:%=$(INCLUDE_DIR)%)

NAME = 			client

FLAGS =			-Wall -Werror -Wextra -std=c++17

C =				gcc

all: 			$(NAME)

$(NAME):		$(SRC)
				$(CXX) $(FLAGS) $(SRC) -o $(NAME)

run:
				sudo ./client test_file

clean:
				rm -rf *.o

fclean:			clean
				rm -rf $(NAME)

re:				fclean all