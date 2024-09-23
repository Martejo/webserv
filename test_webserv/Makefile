NAME		= test_webserv

CC			= c++

CFLAGS		= -std=c++98 -g3 -Wall -Wextra -Werror

SRC_FILES 	=	src/main.cpp \
				src/HTTPHandler.cpp \
				src/HTTPRequest.cpp \
				src/HTTPResponse.cpp \


OBJ			= $(SRC_FILES:.cpp=.o)

INC 		=	includes/HTTPHandler.hpp \
				includes/HTTPRequest.hpp \
				includes/HTTPResponse.hpp \
				includes/Webserv.hpp \
				

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