NAME		:=	gomoku
NAME_BONUS	:=	Gomoku_bonus

SRCS		:= 	main.cpp \
				Move.cpp \

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
	@${CC} ${FLAGS} -o ${NAME} ${OBJS} -lreadline
	@printf "$(_BOLD)$(NAME)$(_RESET) compiled $(_GREEN)$(_BOLD)successfully$(_RESET)\n\n"

${BUILD_DIR}%.o: ${DIR}%.cpp
	@mkdir -p $(dir $@)
	@${CC} ${FLAGS} -o $@ -c $<
	@$(eval CURR_OBJ=$(shell echo $$(( $(CURR_OBJ) + 1 ))))
	@$(eval PERCENT=$(shell echo $$(( $(CURR_OBJ) * 100 / $(OBJS_TOTAL) ))))
	@printf "$(_GREEN)($(_BOLD)%3s%%$(_RESET)$(_GREEN)) $(_RESET)Compiling $(_BOLD)$(_PURPLE)$<$(_RESET)\n" "$(PERCENT)"

${BUILD_DIR}%.o: ${DIR_BONUS}%.cpp
	@mkdir -p $(dir $@)
	@${CC} ${FLAGS_BONUS} -o $@ -c $<
	@$(eval CURR_OBJ=$(shell echo $$(( $(CURR_OBJ) + 1 ))))
	@$(eval PERCENT=$(shell echo $$(( $(CURR_OBJ) * 100 / $(OBJS_TOTAL) ))))
	@printf "$(_GREEN)($(_BOLD)%3s%%$(_RESET)$(_GREEN)) $(_RESET)Compiling $(_BOLD)$(_PURPLE)$<$(_RESET)\n" "$(PERCENT)"

bonus: ${OBJS_BONUS}
	@${CC} ${FLAGS_BONUS} -o ${NAME_BONUS} ${OBJS_BONUS} -lreadline
	@printf "$(_BOLD)$(NAME_BONUS)$(_RESET) compiled $(_GREEN)$(_BOLD)successfully$(_RESET)\n\n"

clean:
	@rm -rf ${OBJS} ${DEPS} ${BUILD_DIR}
	@printf "\n$(_BOLD)All objects are $(_GREEN)cleaned $(_RESET)! 🎉\n\n"

fclean: clean
	@rm -f ${NAME} ${NAME_BONUS} ${DEPS} ${DEPS_BONUS}
	@printf "Cleaned $(_BOLD)$(NAME)$(_RESET) !\n\n"

re: fclean all

-include $(DEPS)
-include $(DEPS_BONUS)

.PHONY: clean fclean re all
