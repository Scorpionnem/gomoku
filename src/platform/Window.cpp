#include "platform/Window.hpp"
#include "Gomoku.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>

Window::Window()
{
}

Window::~Window()
{
	close();
}

void    Window::open(const char *title, u32 size)
{
	open(title, size, size);
}

void    Window::open(const char *title, u32 width, u32 height)
{
	if (_window)
		throw std::runtime_error("Window::open: already open");

	_initSDL();
	_createWindow(title, width, height);
	_width = width;
	_height = height;


	
	SDL_Surface	*surface = SDL_LoadBMP("gomoku_board.bmp");
	if (!surface)
		throw std::runtime_error("loadbmp");

	board_tex = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!board_tex)
		throw std::runtime_error("createtexture");
	SDL_FreeSurface(surface);



	surface = SDL_LoadBMP("black_piece.bmp");
	if (!surface)
		throw std::runtime_error("loadbmp");

	black_tex = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!black_tex)
		throw std::runtime_error("createtexture");
	SDL_FreeSurface(surface);



	surface = SDL_LoadBMP("white_piece.bmp");
	if (!surface)
		throw std::runtime_error("loadbmp");

	white_tex = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!white_tex)
		throw std::runtime_error("createtexture");
	SDL_FreeSurface(surface);
}

void    Window::close()
{
	if (board_tex)
	{
		SDL_DestroyTexture(board_tex);
		board_tex = nullptr;
	}
	if (white_tex)
	{
		SDL_DestroyTexture(white_tex);
		white_tex = nullptr;
	}
	if (black_tex)
	{
		SDL_DestroyTexture(black_tex);
		black_tex = nullptr;
	}
	if (_renderer)
	{
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
	if (_window)
	{
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}
	if (SDL_WasInit(SDL_INIT_VIDEO))
		SDL_Quit();
}

bool    Window::pollEvents(Input &input)
{
	SDL_Event   event;
	bool        alive;

	alive = true;
	input.beginFrame();
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			alive = false;
		else if (event.type == SDL_KEYDOWN && !event.key.repeat)
			input.press(event.key.keysym.sym);
		else if (event.type == SDL_KEYUP)
			input.release(event.key.keysym.sym);
		else if (event.type == SDL_MOUSEBUTTONDOWN)
			input.press(event.button.button);
		else if (event.type == SDL_MOUSEBUTTONUP)
			input.release(event.button.button);
		else if (event.type == SDL_MOUSEMOTION)
			input.addMouseDelta(static_cast<float>(event.motion.xrel), static_cast<float>(event.motion.yrel));
		else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			_width = static_cast<u32>(event.window.data1);
			_height = static_cast<u32>(event.window.data2);
		}
	}
	SDL_GetMouseState(&input._mouseX, &input._mouseY);
	return (alive);
}

void    Window::swapBuffers()
{
	SDL_RenderPresent(_renderer);
	clear(0, 0, 0);
}

void    Window::clear(unsigned char r, unsigned char g, unsigned char b)
{
	SDL_SetRenderDrawColor(_renderer, r, g, b, 255);
	SDL_RenderClear(_renderer);
}

void    Window::captureMouse(bool captured)
{
	SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
}

bool    Window::isMouseCaptured() const
{
	return (SDL_GetRelativeMouseMode() == SDL_TRUE);
}

bool    Window::isOpen() const
{
	return (_window != nullptr);
}

u32     Window::width() const
{
	return (_width);
}

u32     Window::height() const
{
	return (_height);
}

void    Window::_initSDL()
{
	if (SDL_WasInit(SDL_INIT_VIDEO))
		return ;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());
}

void    Window::_createWindow(const char *title, u32 width, u32 height)
{
	_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_BORDERLESS);
	if (!_window)
	{
		close();
		throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());
	}
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	if (!_renderer)
	{
		close();
		throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());
	}
}

void	Window::drawPiece(int x, int y, bool white)
{
	SDL_Rect	rect = {
		.x = x,
		.y = y,
		.w = TILE_SIZE,
		.h = TILE_SIZE,
	};

	SDL_RenderCopy(_renderer, white ? white_tex : black_tex, NULL, &rect);
}

void	Window::drawBoard()
{
	SDL_Rect	rect = {
		.x = 0,
		.y = 0,
		.w = WINDOW_SIZE,
		.h = WINDOW_SIZE,
	};

	SDL_RenderCopy(_renderer, board_tex, NULL, &rect);
}
