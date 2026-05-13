/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chrono.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 16:06:42 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/13 16:07:07 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <ctime>
#include <string>

class	Chrono
{
	public:
		Chrono() {}
		~Chrono() {}

		void	start()
		{
			_start = get_time();
		}
		double	get()
		{
			return (get_time() - _start);
		}

		static double get_time()
		{
			struct timespec	current;
			clock_gettime(CLOCK_MONOTONIC, &current);

			double			res;
			res = (current.tv_sec) + (current.tv_nsec) * 1e-9;
			return (res);
		}
	private:
		double		_start = 0;
};
