/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 15:45:50 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/13 18:44:07 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Chrono.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <SDL2/SDL.h>

#include <cstdint>
#include <stdexcept>
#include <map>

class	Window
{
	public:
		/*
			Handles all window informations such as size, delta time and user inputs
		*/
		class	Events
		{
			public:
				double		aspect_ratio() const {return ((double)_width / (double)_height);}
				uint32_t	width() const {return (_width);}
				uint32_t	height() const {return (_height);}

				double	delta_time() const {return (_delta_time);}

				bool	quit() const {return (_quit);}

				bool	key(int k) const {return (_keys.find(k) == _keys.end() ? false : _keys.find(k)->second);}
				bool	key_pressed(int k) const {return (_keys_pressed.find(k) == _keys_pressed.end() ? false : _keys_pressed.find(k)->second);}
				bool	key_released(int k) const {return (_keys_released.find(k) == _keys_released.end() ? false : _keys_released.find(k)->second);}
			private:

				Events() {}
				~Events() {}

				uint32_t		_width = 0;
				uint32_t		_height = 0;
				double			_delta_time = 0;

				bool			_quit = false;

				std::map<int, bool>	_keys;
				std::map<int, bool>	_keys_pressed;
				std::map<int, bool>	_keys_released;

				friend Window;
		};

	public:
		Window() {}
		~Window() {close();}
		void	open(const char *name, uint32_t width, uint32_t height);
		void	close() {_opened = false;}

		const Window::Events	&poll_events();
		void					render();

	private:
		SDL_Window		*_window = NULL;
		SDL_GLContext	_GLContext = NULL;

		bool			_opened = false;

		Window::Events	_events;
		Chrono			_chrono;
		double			_last_frame;
};
