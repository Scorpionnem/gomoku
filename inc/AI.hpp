#ifndef MANDA_AI_HPP
# define MANDA_AI_HPP

# include "Board.hpp"
# include "ThreadPool.hpp"
# include "Move.hpp"
# include <climits>

class AI {
    public:
		struct	_
		{
			struct	MoveScore
			{
				Move	m;
				int		score;
			};
			std::vector<MoveScore>	ai_moves;

			int			explored_nodes;
			int			max_depth_explored;
			int			branches_cut_off;
			int			branches_reach_end;
			int			max_depth;
			double		time;
			ThreadPool	threads;
		};
		static AI::_	v;
        static Move                 bestMove(const Board& board, Piece ai, int depth);

    private:
        static const int            WIN_SCORE = 1000000;

        static int                  alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta);
        static void                 orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic);
};

#endif
