NAME :=	gomoku

CXX :=	c++
CXXFLAGS :=	-g -MP -MMD # -Wall -Wextra -Werror -std=c++17 # -fsanitize=address -fno-omit-frame-pointer
LFLAGS := -lSDL2 -lGL

###

EXTERNAL_DIR := external

INCLUDE_DIRS :=	inc/				\
				external/imgui		\
				/usr/include/SDL2		\
				external/glad		\
				external/stb_image	\
				glad				\

SRCS :=	src/main.cpp	\
		src/Window.cpp

SRCS +=	external/glad/glad.cpp\
		external/stb_image/stb_image.cpp

IMGUI := $(EXTERNAL_DIR)/imgui
IMGUI_SRCS_RAW =	imgui.cpp\
					imgui_draw.cpp\
					imgui_widgets.cpp\
					imgui_tables.cpp\
					imgui_demo.cpp\
					backends/imgui_impl_opengl3.cpp\
					backends/imgui_impl_sdl2.cpp
IMGUI_SRCS = $(addprefix $(IMGUI)/, $(IMGUI_SRCS_RAW))
SRCS +=	$(IMGUI_SRCS)

###

INCLUDE_DIRS :=	$(addprefix -I, $(INCLUDE_DIRS))

###

OBJ_DIR :=	obj

OBJS =	$(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS =	$(SRCS:%.cpp=$(OBJ_DIR)/%.d)

###

compile: imgui stb_image glad
	@make -j all --no-print-directory

all: $(NAME)

$(EXTERNAL_DIR):
	@mkdir -p external

imgui: $(EXTERNAL_DIR)
	@if ls external | grep -q "imgui"; then \
		printf ""; \
	else \
		echo "Downloading $(_BOLD)imgui$(_RESET)";\
		git clone https://github.com/ocornut/imgui.git $(IMGUI);\
		echo "Downloaded $(_BOLD)imgui$(_RESET)";\
	fi

stb_image: $(EXTERNAL_DIR)
	@if ls external/stb_image | grep -q "stb_image.h"; then \
		printf ""; \
	else\
		echo "Downloading $(_BOLD)stb_image.h$(_RESET)"; \
		mkdir -p external/stb_image; \
		curl --silent -o external/stb_image/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h;\
		echo "Downloaded $(_BOLD)stb_image.h$(_RESET)"; \
	fi

glad: $(EXTERNAL_DIR)
	@if ls external | grep -q "glad"; then\
		printf "";\
	else \
		echo "Downloading $(_BOLD)glad$(_RESET)";\
		mkdir $(EXTERNAL_DIR)/glad;\
		cd $(EXTERNAL_DIR)/glad;\
		mkdir glad;\
		cd glad;\
		curl https://raw.githubusercontent.com/Manualouest/42_postCC/refs/heads/ft_scop/libs/glad/glad.h --output glad.h;\
		cd ..;\
		curl https://raw.githubusercontent.com/Manualouest/42_postCC/refs/heads/ft_scop/libs/glad/glad.c --output glad.cpp;\
		echo "Downloaded $(_BOLD)glad$(_RESET)";\
	fi

$(NAME): $(OBJS)
	@echo Compiling $(NAME)
	@$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $@ $^ $(LFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

re: fclean compile

fclean: clean
	@echo Removed $(NAME)
	@rm -rf $(NAME)

clean:
	@echo Removed $(OBJ_DIR)
	@rm -rf $(OBJ_DIR)

.PHONY: all clean fclean re compile

-include $(DEPS)
