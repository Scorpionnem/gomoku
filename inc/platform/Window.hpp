#pragma once

#include "math/stdint.hpp"
#include "platform/Input.hpp"

#include <SDL2/SDL.h>

#include <map>
#include <string>
#include <iostream>

struct SDL_Window;

class Config;

class Window
{
	public:
		Window();
		~Window();

		Window(const Window &) = delete;
		Window  &operator=(const Window &) = delete;

		void    open(const char *title, u32 width, u32 height);
		void    open(const char *title, u32 size);
		void    close();

		bool    pollEvents(Input &input);

		void    swapBuffers();
		void    clear(unsigned char r, unsigned char g, unsigned char b);

		void    captureMouse(bool captured);
		bool    isMouseCaptured() const;

		bool    isOpen() const;
		u32     width() const;
		u32     height() const;

		void	drawFillRect(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char o = 255)
		{
			SDL_SetRenderDrawColor(_renderer, r, g, b, o);

			SDL_Rect	rect;

			rect.w = width;
			rect.h = height;
			rect.x = x;
			rect.y = y;

			SDL_RenderFillRect(_renderer, &rect);
		}
		void	drawRect(int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char o = 255)
		{
			SDL_SetRenderDrawColor(_renderer, r, g, b, o);

			SDL_Rect	rect;

			rect.w = width;
			rect.h = height;
			rect.x = x;
			rect.y = y;

			SDL_RenderDrawRect(_renderer, &rect);
		}
	private:
		void    _initSDL();
		void    _createWindow(const char *title, u32 width, u32 height);

		SDL_Window		*_window = nullptr;
		SDL_Renderer	*_renderer = nullptr;

		u32				_width = 0;
		u32				_height = 0;
};
