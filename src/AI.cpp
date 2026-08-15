#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"
#include "Chrono.hpp"
#include "Game.hpp"

#include <iostream>

int	explored_nodes = 0;
int	max_depth = 0;
int	max_depth_explored = 0;

int AI::alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta) {
	explored_nodes++;
	max_depth_explored = std::max(depth, max_depth_explored);

    const Piece opp = Game::opponent(toMove);
    const bool maximizing = (toMove == ai);

    if (depth >= max_depth)
        return Heuristic::evaluate(board, ai);

    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(board, toMove);
    if (moves.empty()) {
        int score = Heuristic::evaluate(board, ai);
		return score;
    }

    if (maximizing)
	{
        int best = INT_MIN;
        for (Move& move : moves)
		{
            board.applyMove(move, opp);
            int score = alphabeta(board, ai, opp, depth + 1, alpha, beta);
            board.undo();

            if (score > best)
				best = score;
            if (best > alpha)
				alpha = best;
            if (beta <= alpha)
				break ;
        }
        return best;
    }

    int best = INT_MAX;
    for (Move& move : moves)
	{
        board.applyMove(move, opp);
        int score = alphabeta(board, ai, opp, depth + 1, alpha, beta);
        board.undo();

        if (score < best)
			best = score;
        if (best < beta)
			beta = best;
        if (beta <= alpha)
			break ;
    }
    return best;
}

Move AI::bestMove(const Board& board, Piece ai, int max_depth_)
{
	max_depth = max_depth_;

	Chrono	c;

	explored_nodes = 0;
	max_depth_explored = 0;

    Board search = board;
    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(search, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = Game::opponent(ai);
    Move best = moves.front();
    int bestScore = INT_MIN;
    int alpha = INT_MIN;

    for (Move& move : moves) {
        search.applyMove(move, opp);
        int score = alphabeta(search, ai, opp, 0, alpha, INT_MAX);
        search.undo();

        if (score > bestScore) bestScore = score, best = move;
        if (bestScore > alpha) alpha = bestScore;
    }

	std::cout << "Move found in: " << c.get() << " seconds" << std::endl;
	std::cout << "Explored nodes " << explored_nodes << std::endl;
	std::cout << "Max depth " << max_depth_explored << std::endl;
    return best;
}
