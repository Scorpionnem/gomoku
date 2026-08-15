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

			// Incrementes par les threads de recherche -> atomiques.
			std::atomic<int>	explored_nodes;
			std::atomic<int>	branches_cut_off;
			std::atomic<int>	branches_reach_end;
			// Ecrits uniquement par le thread principal.
			int			max_depth_explored;
			int			max_depth;
			double		time;
			ThreadPool	threads;
		};
		static AI::_	v;
        static Move                 bestMove(const Board& board, Piece ai, int depth);

    private:
        static const int            WIN_SCORE = 1000000;
        static constexpr double     TIME_LIMIT = 0.49;   // gate douce de l'iterative deepening
        static constexpr double     HARD_LIMIT = 0.5;    // limite dure du cahier des charges (500ms)
        static const int            MAX_CANDIDATES = 6;

        static int                  alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta);
        static void                 orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic);
};

#endif
