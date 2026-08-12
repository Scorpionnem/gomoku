NAME		:=	gomoku
NAME_BONUS	:=	Gomoku_bonus

SRCS		:= 	main.cpp \
				Move.cpp \
				TerminalUI.cpp \

SRCS_BONUS	:= 	main_bonus.cpp \

DIR			:=	srcs/manda/

DIR_BONUS	:=	srcs/bonus/

BUILD_DIR := .build/

OBJS		:=	$(SRCS:%.cpp=$(BUILD_DIR)%.o)
OBJS_BONUS	:=	$(SRCS_BONUS:%.cpp=$(BUILD_DIR)%.o)

CC			:= c++

FLAGS 		:= -Wall -Werror -Wextra -g -I ./includes/manda/ -std=c++20 -MMD -MP
FLAGS_BONUS := -Wall -Werror -Wextra -g -I ./includes/bonus/ -std=c++20 -MMD -MP

DEPS		:=	$(OBJS:.o=.d)
DEPS_BONUS	:=	$(OBJS_BONUS:.o=.d)

.DEFAULT_GOAL := all

TPUT 					= tput -T xterm-256color
_RESET 					:= $(shell $(TPUT) sgr0)
_BOLD 					:= $(shell $(TPUT) bold)
_ITALIC 				:= $(shell $(TPUT) sitm)
_UNDER 					:= $(shell $(TPUT) smul)
_GREEN 					:= $(shell $(TPUT) setaf 2)
_YELLOW 				:= $(shell $(TPUT) setaf 3)
_RED 					:= $(shell $(TPUT) setaf 1)
_GRAY 					:= $(shell $(TPUT) setaf 8)
_PURPLE 				:= $(shell $(TPUT) setaf 5)

OBJS_TOTAL	= $(words $(OBJS))
N_OBJS		:= $(shell find $(DIR) -type f -name $(OBJS) 2>/dev/null | wc -l)
OBJS_TOTAL	:= $(shell echo $$(( $(OBJS_TOTAL) - $(N_OBJS) )))
CURR_OBJ	= 0

all: ${NAME}

${NAME}: ${OBJS}
	@${CC} ${FLAGS} -o ${NAME} ${OBJS}
	@printf "$(_BOLD)$(NAME)$(_RESET) compiled $(_GREEN)$(_BOLD)successfully$(_RESET)\n\n"

${BUILD_DIR}%.o: ${DIR}%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
