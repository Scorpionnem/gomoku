NAME		:=	Gomoku

SRCS		:= 	main.cpp		\
				Move.cpp		\
				Board.cpp		\
				Heuristic.cpp	\
				AI.cpp			\
				platform/Input.cpp		\
				platform/Window.cpp		\
				Gomoku.cpp		\

DIR				:=	src/
INC_DIR			:=	inc/

BUILD_DIR := .build/

OBJS		:=	$(SRCS:%.cpp=$(BUILD_DIR)%.o)

CC			:= c++

SDL_CFLAGS :=	$(shell sdl2-config --cflags)
SDL_LIBS :=		$(shell sdl2-config --libs)
LFLAGS :=		$(SDL_LIBS) -lGL -lSDL2_ttf

INCLUDE_DIRS :=	-I$(INC_DIR) $(SDL_CFLAGS) -I$(LIB_DIR)

FLAGS 		:= -Wall -Werror -Wextra -g $(INCLUDE_DIRS) -std=c++20 -MMD -MP -O3 # -fsanitize=address -fno-omit-frame-pointer


DEPS		:=	$(OBJS:.o=.d)

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
	@${CC} ${FLAGS} -o ${NAME} ${OBJS} ${LFLAGS}
	@printf "$(_BOLD)$(NAME)$(_RESET) compiled $(_GREEN)$(_BOLD)successfully$(_RESET)\n\n"

${BUILD_DIR}%.o: ${DIR}%.cpp
	@mkdir -p $(dir $@)
	@${CC} ${FLAGS} -o $@ -c $<
	@$(eval CURR_OBJ=$(shell echo $$(( $(CURR_OBJ) + 1 ))))
	@$(eval PERCENT=$(shell echo $$(( $(CURR_OBJ) * 100 / $(OBJS_TOTAL) ))))
	@printf "$(_GREEN)($(_BOLD)%3s%%$(_RESET)$(_GREEN)) $(_RESET)Compiling $(_BOLD)$(_PURPLE)$<$(_RESET)\n" "$(PERCENT)"

clean:
	@rm -rf ${OBJS} ${DEPS} ${BUILD_DIR}
	@printf "\n$(_BOLD)All objects are $(_GREEN)cleaned $(_RESET)! 🎉\n\n"

fclean: clean
	@rm -f ${NAME} ${DEPS}
	@printf "Cleaned $(_BOLD)$(NAME)$(_RESET) !\n\n"

re: fclean all

-include $(DEPS)

.PHONY: clean fclean re all
