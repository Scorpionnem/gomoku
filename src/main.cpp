#include "platform/Window.hpp"

#include <iostream>

class	Gomoku
{
	#define TILES 19
	#define TILE_SIZE 32
	#define PIECE_SIZE 24
	#define WINDOW_SIZE (TILES * TILE_SIZE)
	#define WINDOW_TITLE "Gomoku"
	public:
		void	run()
		{
			init();
			loop();
		}
	private:
		void	init()
		{
			win.open(WINDOW_TITLE, WINDOW_SIZE);
		}
		void	loop()
		{
			running = true;

			while (running)
			{
				Input	input;
				if (!win.pollEvents(input) || input.isDown(SDLK_ESCAPE))
					running = false;

				if (input.isDown(SDL_BUTTON_LEFT))
				{
					std::cout << "X: " << input.mouseX() / TILE_SIZE << " Y: " << input.mouseY() / TILE_SIZE << std::endl;
				}

				renderBoardBackground();
				renderCursor(input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE);

				drawPiece(0, 0, 0, 0, 0);

				win.swapBuffers();

				SDL_Delay(16);
			}
		}
		void	drawTile(int x, int y, int r, int g, int b)
		{
			win.drawFillRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, r, g, b);
		}
		void	drawPiece(int x, int y, int r, int g, int b)
		{
			win.drawFillRect(x * TILE_SIZE + 4,
							 y * TILE_SIZE + 4,
							 PIECE_SIZE, PIECE_SIZE, r, g, b);
		}
		void	renderCursor(int x, int y)
		{
			win.drawRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, 0);
		}
		void	renderBoardBackground()
		{
			for (int x = 0; x < TILES; x++)
				for (int y = 0; y < TILES; y++)
				{
					if ((x + y) % 2)
						drawTile(x, y, 230, 167, 80);
					else
						drawTile(x, y, 204, 141, 53);
				}
		}
	private:
		Window	win;
		bool	running = false;
};

int	main(int ac, char **av)
{
	(void)ac;(void)av;

	Gomoku	gmk;

	try
	{
		gmk.run();
	} catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}
