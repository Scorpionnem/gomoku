#ifndef MANDA_AI_HPP
# define MANDA_AI_HPP

# include "Board.hpp"
# include "Move.hpp"
# include <climits>

class AI {
    public:
		static int	explored_nodes;
		static int	max_depth;
		static int	max_depth_explored;
		static double	time;
        static Move                 bestMove(const Board& board, Piece ai, int depth);

    private:
        static const int            WIN_SCORE = 1000000;

        static int                  alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta);
        static void                 orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic);
};

#endif
