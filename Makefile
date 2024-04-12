NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic

INCLUDE_DIR = includes
INCLUDE =	ft_irc.hpp \
			Server.hpp \
			Channel.hpp \
			Client.hpp \
			SocketStream.hpp \
			Message.hpp \
			NumericReply.hpp

SRC_DIR = srcs
SRC	=		main.cpp \
			Server.cpp \
			Channel.cpp \
			Client.cpp \
			SocketStream.cpp \
			Message.cpp

OBJ_DIR = objs
OBJ = ${SRC:.cpp=.o}

all:	${NAME}

$(OBJ_DIR):
	mkdir -p ${OBJ_DIR}

${NAME}:	$(addprefix ${OBJ_DIR}/, ${OBJ})
	${CXX} ${CXXFLAGS}  $^ -o $@

${OBJ_DIR}/%.o:	${SRC_DIR}/%.cpp | ${OBJ_DIR}
	${CXX} -c ${CXXFLAGS} $< -I${INCLUDE_DIR} -o $@

clean:
	rm -rf ${OBJ_DIR}

fclean:
	${MAKE} clean
	${RM} ${NAME}
	${RM} ${TEST_NAME}

re:
	${MAKE} fclean
	${MAKE} all

.PHONY:	all clean fclean re
