NAME		= test_webserv

CC			= c++

CFLAGS		= -std=c++98 -g3 -Wall -Wextra -Werror

SRC_FILES 	=	src/main.cpp \
				src/HttpRequest.cpp \
				src/HttpResponse.cpp \
				src/ServerConfig.cpp \
				src/RouteConfig.cpp \
				src/WebServer.cpp \
				src/ListeningSocket.cpp \
				src/ListeningSocketHandler.cpp \
				src/DataSocket.cpp \
				src/DataSocketHandler.cpp \
				


OBJ			= $(SRC_FILES:.cpp=.o)

INC 		=	includes/HttpRequest.hpp \
				includes/HttpResponse.hpp \
				includes/ServerConfig.hpp \
				includes/RouteConfig.hpp \
				includes/WebServer.hpp \
				includes/ListeningSocket.hpp \
				includes/ListeningSocketHandler.hpp \
				includes/DataSocket.hpp \
				includes/DataSocketHandler.hpp \
				

%.o   : %.cpp $(INC)
	${CC} ${CFLAGS} -c $< -o $@ -I./includes

all: $(NAME)

$(NAME): $(OBJ)
	@echo -n Compiling executable $(NAME)...
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo Done.

clean:
	@echo -n Making clean...
	@rm -rf $(OBJ)
	@echo Done.

fclean: clean
	@echo -n Making fclean...
	@rm -f $(NAME)
	@echo Done.

test: re
	./$(NAME)
	@make fclean

re: fclean all

.PHONY : all clean fclean re test