#pragma once

#include "math/stdint.hpp"
#include "platform/Input.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

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

		void	drawPiece(int x, int y, bool white);
		void	drawBoard();

		void	drawText(const std::string &s, int x, int y, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255)
		{
			SDL_Color color = {r, g, b, 255};
			SDL_Surface *surf = TTF_RenderUTF8_Blended(font, s.c_str(), color);
			SDL_Texture *tex = SDL_CreateTextureFromSurface(_renderer, surf);

			int w = surf->w, h = surf->h;
			SDL_FreeSurface(surf);

			SDL_Rect dst = {x, y, w, h};
			SDL_RenderCopy(_renderer, tex, NULL, &dst);

			SDL_DestroyTexture(tex);
		}
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

		SDL_Renderer	*getRenderer() {return (_renderer);}
	private:
		void    _initSDL();
		void    _createWindow(const char *title, u32 width, u32 height);

		SDL_Window		*_window = nullptr;
		SDL_Renderer	*_renderer = nullptr;

		SDL_Texture		*board_tex = nullptr;
		SDL_Texture		*black_tex = nullptr;
		SDL_Texture		*white_tex = nullptr;
		SDL_Texture		*hotbar_tex = nullptr;

		TTF_Font		*font = nullptr;

		u32				_width = 0;
		u32				_height = 0;
};
