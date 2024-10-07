NAME		= parsingConfigtest

CC			= c++

CFLAGS		= -std=c++98 -g3 -Wall -Wextra -Werror

SRC_FILES 	=	src/main.cpp \
				src/ConfigParser.cpp \
				src/Config.cpp \
				src/Exceptions.cpp \
				src/Location.cpp \
				src/Server.cpp \
				


OBJ			= $(SRC_FILES:.cpp=.o)

INC 		=	src/ConfigParser.hpp \
				src/Config.hpp \
				src/Exceptions.hpp \
				src/Location.hpp \
				src/Server.hpp \
				

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