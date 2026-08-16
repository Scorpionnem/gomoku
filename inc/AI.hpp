#ifndef MANDA_AI_HPP
# define MANDA_AI_HPP

# include "Board.hpp"
# include "Move.hpp"
# include "Heuristic.hpp"
# include "Game.hpp"
# include "Chrono.hpp"
# include <climits>
# include <unordered_map>
# include "math/stdint.hpp"

inline constexpr u64	posToHash(Position p)
{
	return (static_cast<u64>(p.x) << 32 | static_cast<u64>(p.y));
}

inline constexpr u32	hashToPosX(u64 h)
{
	return (h >> 32);
}

inline constexpr u32	hashToPosY(u64 h)
{
	return (h & 0x00000000ffffffff);
}

class AI
{
	public:
		struct	MoveScore
		{
			int		score;
			Move	m;
		};
		struct	Stats
		{
			double	time;
			// max depth explored
			int		max_depth;
			// nodes that reached max_depth
			int		max_depth_nodes;
			// nodes that were stopped before reaching max_depth
			int		stopped_nodes;
			int		explored_nodes;
		};

	public:
        static const int            WIN_SCORE = 1000000;
        static constexpr double     TIME_LIMIT = 0.495;
        static const int            MAX_CANDIDATES = 6;

		Move	bestMove(const Board& b, Piece player, int max_depth);

		const Move	&getFinalMove() {return (_final_move);}
		const std::unordered_map<u64, MoveScore>&	getEvaluatedMoves() {return (_evaluated_moves);}
		const Stats&	getStats() {return (_stats);}
		void			orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic);
		int				alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta);
	private:
		std::unordered_map<u64, MoveScore>	_evaluated_moves;
		Move								_final_move;
		Stats	_stats;
		Chrono	_time;
};

#endif
