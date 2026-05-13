/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 18:30:54 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/13 18:45:00 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Window.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

void	Window::open(const char *name, uint32_t w, uint32_t h)
{
	_events._width = w;
	_events._height = h;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		throw (std::runtime_error(SDL_GetError()));

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	_window = SDL_CreateWindow(name,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		_events._width, _events._height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!_window)
	{
		SDL_Quit();
		throw (std::runtime_error(SDL_GetError()));
	}

	_GLContext = SDL_GL_CreateContext(_window);
	if (!_GLContext)
	{
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw (std::runtime_error(SDL_GetError()));
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		SDL_GL_DeleteContext(_GLContext);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		throw (std::runtime_error("Failed to initialize GLAD"));
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glViewport(0, 0, _events._width, _events._height);

	SDL_GL_SetSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(_window, _GLContext);
	ImGui_ImplOpenGL3_Init();

	_opened = true;
	_chrono.start();
	_last_frame = _chrono.get();
}

void	Window::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(_window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const Window::Events	&Window::poll_events()
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
			case SDL_QUIT:
				_events._quit = true;
				break ;
			// case SDL_WINDOWEVENT:
			// 	if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			// 	{
			// 		_width = event.window.data1;
			// 		_height = event.window.data2;
			// 		glViewport(0, 0, _width, _height);
			// 	}
			// 	break ;
			case SDL_KEYDOWN:
				_events._keys[event.key.keysym.sym] = true;
				_events._keys_pressed[event.key.keysym.sym] = true;
				break ;
			case SDL_KEYUP:
				_events._keys[event.key.keysym.sym] = false;
				_events._keys_released[event.key.keysym.sym] = true;
				break ;
			// case SDL_MOUSEMOTION:
			// 	_events._mouseDeltaX = event.motion.xrel;
			// 	_events._mouseDeltaY = event.motion.yrel;
			// 	break ;
			// case SDL_MOUSEBUTTONDOWN:
			// 	_events._mouseBtn[event.button.button] = true;
			// 	_events._mouseBtnPressed[event.button.button] = true;
			// 	break ;
			// case SDL_MOUSEBUTTONUP:
			// 	_events._mouseBtn[event.button.button] = false;
			// 	_events._mouseBtnLifted[event.button.button] = true;
			// 	break ;
			// case SDL_MOUSEWHEEL:
			// 	_events._mouseScroll = event.wheel.y;
			// 	break ;
		}
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	return (_events);
}
