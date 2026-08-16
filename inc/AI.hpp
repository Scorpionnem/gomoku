#ifndef MANDA_AI_HPP
# define MANDA_AI_HPP

# include "Board.hpp"
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
		};
		static AI::_	v;
        static Move                 bestMove(const Board& board, Piece ai, int depth);

    private:
        static const int            WIN_SCORE = 1000000;
        static constexpr double     TIME_LIMIT = 0.49;   // limite de temps (cahier des charges : 500ms)
        static const int            MAX_CANDIDATES = 10;

        static int                  alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta);
        static void                 orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic);
};

#endif
