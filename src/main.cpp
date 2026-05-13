/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 15:41:24 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/13 16:11:16 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Window.hpp"

int	main(void)
{
	Window	win;

	win.open("Gomoku", 512, 512);

	while (1)
	{
		const Window::Events	&ev = win.poll_events();

		win.render();
	}
	return (0);
}
