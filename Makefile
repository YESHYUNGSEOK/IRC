NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -g -fsanitize=address

INCLUDE_DIR = includes
INCLUDE =	ft_irc.hpp \
			Server.hpp \
			Channel.hpp \
			Client.hpp \
			SocketStream.hpp \
			CommandHandler.hpp \
			Message.hpp \
			NumericReply.hpp

SRC_DIR = srcs
SRC	=		main.cpp \
			Server.cpp \
			Channel.cpp \
			Client.cpp \
			SocketStream.cpp \
			CommandHandler.cpp \
			Message.cpp \
			NumericReply.cpp

OBJ_DIR = objs
OBJ = ${SRC:.cpp=.o}

TEST_NAME = ${NAME}_test
TEST_DIR = test
TEST_SRC = test.cpp
TEST_OBJ = ${TEST_SRC:.cpp=.o}

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

test:
	${MAKE} fclean
	${MAKE} ${TEST_NAME}
	./${TEST_NAME}
	${MAKE} fclean

${TEST_NAME}:
	c++ ./test/test.cpp -I./includes ./srcs/Message.cpp -o ${TEST_NAME}

# ${TEST_NAME}:	$(addprefix ${TEST_DIR}/,${TEST_OBJ}) $(addprefix ${OBJ_DIR}/, ${OBJ})
# 	${CXX} ${CXXFLAGS} $^ -o $@

# ${OBJ_DIR}/%.o:	${TEST_DIR}/%.cpp | ${OBJ_DIR}
# 	${CXX} -c ${CXXFLAGS} $< -I${INCLUDE_DIR} -o $@

.PHONY:	all clean fclean re test
