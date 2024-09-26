NAME = webserv
CC = c++
FLAG =  -Wall -Werror -Wextra -std=c++11
HEADERS = -I includes
SRC_DIR := ./
OBJ_DIR := obj
DIR_DUP = mkdir -p $(@D)
SRC := \
	src/main.cpp \
	src/Client.cpp \
	src/ClientUtils.cpp \
	src/Server.cpp \
	src/ServerUtils.cpp \
	src/Config.cpp \
	src/ConfigUtils.cpp \
	src/ConfigUtils2.cpp \
	src/ServerBlock.cpp \
	src/ServerBlockUtils.cpp \
	src/Location.cpp \
	src/Cgi.cpp \
	src/Parsing.cpp \
	src/ParsingUtils.cpp \
	src/Status.cpp \

SRC := $(SRC:%=$(SRC_DIR)/%)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# color 
BOLD		= \033[1m
ITALIC		= \033[3m
UNDER 		= \033[4m
CYAN		= \033[36;1m
GREEN		= \033[32;1m
INDIGO		= \033[38;2;75;0;130m
CORAL		= \033[38;2;255;127;80m
RESET		= \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAG) $(OBJ) -o $(NAME) $(HEADERS)
	@echo "$(BOLD) $(GREEN) Compilation $(NAME) done$(RESET)"

$(OBJ_DIR)/%.o: ./$(SRC_DIR)/%.cpp
	@$(DIR_DUP)
	@$(CC) $(FLAG) $(HEADERS) -c -o $@ $<

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(BOLD) $(CYAN) Clean objects Done $(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo "$(BOLD) $(CYAN) Clean $(NAME) done $(RESET)"

re: fclean all

.PHONY: all clean fclean re
