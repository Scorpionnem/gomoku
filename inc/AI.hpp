#ifndef MANDA_AI_HPP
# define MANDA_AI_HPP

# include "Board.hpp"
# include "ThreadPool.hpp"
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
		AI() {}
		~AI() {}

		int	minimax(const Board& b, Piece player, Piece opponent, bool maximizing, int depth, int max_depth, int alpha, int beta)
		{
			_stats.explored_nodes++;
			_stats.max_depth = std::max(depth, _stats.max_depth);

			Piece	mover = maximizing ? player : opponent;
			Piece	other = maximizing ? opponent : player;

			if (depth >= max_depth || b.isWin(other))
			{
				_stats.max_depth_nodes++;
				return (Heuristic::evaluate(b, player));
			}

			Board	board = b;

			std::vector<Move> moves = Move::getLegalMoves(board, mover);
			if (moves.empty())
				return (Heuristic::evaluate(b, player));

			if (maximizing)
			{
				int	best = INT_MIN;

				for (Move& m : moves)
				{
					Board	bcpy = b;
					bcpy.applyMove(m, other);

					int	score = minimax(bcpy, player, opponent, false, depth + 1, max_depth, alpha, beta);

					best = std::max(best, score);
					alpha = std::max(alpha, best);

					if (beta <= alpha)
						break ;
				}
				return (best);
			}
			else
			{
				int	best = INT_MAX;

				for (Move& m : moves)
				{
					Board	bcpy = b;
					bcpy.applyMove(m, other);

					int	score = minimax(bcpy, player, opponent, true, depth + 1, max_depth, alpha, beta);

					best = std::min(best, score);
					beta = std::min(beta, best);

					if (beta <= alpha)
						break ;
				}
				return (best);
			}
		}

		Move	bestMove(const Board& b, Piece player, int max_depth)
		{
			_time.start();
			_stats = {};
			_evaluated_moves.clear();

			Board	board = b;
			Piece	opponent = Game::opponent(player);

			std::vector<Move> moves = Move::getLegalMoves(board, player);
			if (moves.empty())
				return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, player};

			Move	best_move = moves.front();
			int		best_score = INT_MIN;
			int		depth = 0;

			_evaluated_moves.reserve(moves.size());

			for (Move& m : moves)
			{
				Board	bcpy = b;

				bcpy.applyMove(m, opponent);
				int	score = minimax(bcpy, player, opponent, false, depth, max_depth, INT_MIN, INT_MAX);

				_evaluated_moves.insert(std::make_pair(posToHash(m.getPosition()), MoveScore{score, m}));

				if (score > best_score)
				{
					best_score = score;
					best_move = m;
				}
			}

			_stats.time = _time.get();
			_final_move = best_move;
			return (best_move);
		}

		const Move	&getFinalMove() {return (_final_move);}
		const std::unordered_map<u64, MoveScore>&	getEvaluatedMoves() {return (_evaluated_moves);}
		const Stats&	getStats() {return (_stats);}
	private:
		std::unordered_map<u64, MoveScore>	_evaluated_moves;
		Move								_final_move;
		Stats	_stats;
		Chrono	_time;
};

#endif
