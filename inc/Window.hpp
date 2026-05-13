/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 15:45:50 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/13 16:11:32 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Chrono.hpp"

#include <cstdint>

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

				double		delta_time() const {return (_delta_time);}
			private:

				Events() {}
				~Events() {}

				uint32_t		_width = 0;
				uint32_t		_height = 0;
				double			_delta_time = 0;

				friend Window;
		};

	public:
		Window() {}
		~Window() {close();}
		void	open(const char *name, uint32_t width, uint32_t height)
		{
			(void)name;
			(void)width;
			(void)height;
		}
		void	close() {_opened = false;}

		const Window::Events	&poll_events()
		{
			return (_events);
		}
		void					render()
		{

		}

	private:
		bool			_opened = false;

		Window::Events	_events;
		Chrono			_chrono;
};
