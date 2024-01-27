NAME	= webserv

CC		= c++
FLAGS	= -Wall -Wextra -Werror -std=c++11 -g
RM		= rm -rf
SRC		= index.cpp Request.cpp Response.cpp Location.cpp Config.cpp ServerConfig.cpp CodesTypes.cpp
HDR		= Request.hpp Response.hpp Location.hpp Config.hpp ServerConfig.hpp CodesTypes.hpp
OBJ		= ${SRC:%.cpp=%.o}

.PHONY: all clean fclean re

all: ${NAME}

${NAME}: ${OBJ} ${HDR} Makefile
	${CC} ${FLAGS} ${OBJ} -o ${NAME}

%.o: %.cpp ${HDR} Makefile
	${CC} ${FLAGS} -c $< -o $@

clean:
	${RM} ${OBJ}

fclean: clean
	${RM} ${NAME}

re: fclean all