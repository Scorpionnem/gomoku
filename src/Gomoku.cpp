#include "Gomoku.hpp"

#include <vector>
#include <map>

void	Gomoku::init()
{
	win.open(WINDOW_TITLE, WINDOW_SIZE);
}

void	Gomoku::loop()
{
	Input	input;

	running = true;

	while (running)
	{
		if (!win.pollEvents(input) || input.isDown(SDLK_ESCAPE))
		{
			running = false;
		}

		if (state == State::GAME)
		{
			game(input);
		}
		else if (state == State::MENU)
			;

		win.swapBuffers();
		SDL_Delay(16);
	}
}

u32		Gomoku::getAction(Input &input, Gomoku::Player p)
{
	if (p == HUMANPLAYER)
	{
		if (input.wasPressed(SDL_BUTTON_LEFT))
		{
			int	x = input.mouseX() / TILE_SIZE;
			int	y = input.mouseY() / TILE_SIZE;

			return (x + y);
		}
		return (0);
	}
	else if (p == AIPLAYER)
	{

	}
	return (0);
}

void	Gomoku::game(Input &input)
{
	renderBoardBackground();

	u32	p = getAction(input, pTurn == P1TURN ? p1 : p2);
	if (p != 0)
	{
		pTurn = pTurn == P1TURN ? P2TURN : P1TURN;
		plays.insert(p);
	}

	// for (auto _p : plays)
	// {
	// 	if (t == P1TURN)
	// 		drawPiece(x, y, 0, 0, 0);
	// 	else
	// 		drawPiece(x, y, 255, 255, 255);
	// }

	renderOutline(input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE, 0, 0, 0);
	if (input.wasPressed(SDL_BUTTON_LEFT))
		renderOutline(input.mouseX() / TILE_SIZE, input.mouseY() / TILE_SIZE, 0, 255, 0);
}

void	Gomoku::drawTile(int x, int y, int r, int g, int b)
{
	win.drawFillRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, r, g, b);
}

void	Gomoku::drawPiece(int x, int y, int r, int g, int b)
{
	win.drawFillRect(x * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						y * TILE_SIZE + ((TILE_SIZE - PIECE_SIZE) / 2),
						PIECE_SIZE, PIECE_SIZE, r, g, b);
}

void	Gomoku::renderOutline(int x, int y, int r, int g, int b)
{
	win.drawRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, r, g, b);
}

void	Gomoku::renderBoardBackground()
{
	for (int x = 0; x < TILES; x++)
		for (int y = 0; y < TILES; y++)
		{
			if ((x + y) % 2)
				drawTile(x, y, 230, 167, 80);
			else
				drawTile(x, y, 204, 141, 53);
		}
	drawTile(9, 9, 140, 90, 20);
}
