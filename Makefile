# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jbok <jbok@student.42seoul.kr>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/07 11:20:10 by jbok              #+#    #+#              #
#    Updated: 2023/10/05 11:36:32 by jbok             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic
SRC_DIR = src
SRC = main.cpp IoHandler.cpp Client.cpp
OBJ = ${SRC:.cpp=.o}
OBJ_DIR = obj
INCLUDE = Ft_Irc.hpp IoHandler.hpp Client.hpp
INCLUDE_DIR = include

all:	${NAME}

${NAME}:	$(addprefix ${OBJ_DIR}/, ${OBJ})
	${CXX} ${CXXFLAGS}  $^ -o $@ 

${OBJ_DIR}/${OBJ}: $(addprefix ${SRC_DIR}/, ${SRC})
	if [ ! -d ${OBJ_DIR} ]; then \
  		mkdir "${OBJ_DIR}"; \
	fi
	${CXX} -c ${CXXFLAGS} $^ -I${INCLUDE_DIR}
	mv ${OBJ} ${OBJ_DIR}

clean:
	rm -rf ${OBJ_DIR}

fclean:
	${MAKE} clean
	${RM} ${NAME}

re:
	${MAKE} fclean
	${MAKE} all

.PHONY:	all clean fclean re
