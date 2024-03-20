NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -g -fsanitize=address
SRC_DIR = srcs
SRC = main.cpp SocketStream.cpp Client.cpp Server.cpp ClientManager.cpp CommandHandler.cpp Channel.cpp
OBJ = ${SRC:.cpp=.o}
OBJ_DIR = objs
INCLUDE = ft_irc.hpp SocketStream.hpp Client.hpp Server.hpp Channel.hpp
INCLUDE_DIR = includes

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
