#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"
#include "Chrono.hpp"
#include "Game.hpp"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

Chrono	c;

static int lineWeight(int len)
{
    switch (len)
    {
        case 0:  return 1;
        case 1:  return 12;
        case 2:  return 120;
        case 3:  return 1200;
        default: return 12000;
    }
}

int cheapMoveScore(const Board& board, const Move& move, Piece opponent)
{
    const Piece me = move.getPiece();
    const Position p = move.getPosition();

    int score = board.findCaptures(move, opponent).capturedCount * 100000;

    for (auto& d : AXES)
    {
        Position fwd = {d[0], d[1]};
        Position bwd = {-d[0], -d[1]};

        int mine = board.countRay(p, fwd, me, 4) + board.countRay(p, bwd, me, 4);
        int opp  = board.countRay(p, fwd, opponent, 4) + board.countRay(p, bwd, opponent, 4);

        score += lineWeight(mine);
        score += (lineWeight(opp) * 3) / 4;
    }
    return score;
}

void AI::orderMoves(Board& board, std::vector<Move>& moves, Piece ai, Piece toMove, bool useHeuristic)
{
    const Piece opp = Game::opponent(toMove);
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(moves.size());

    for (Move& move : moves)
    {
        int score;
        if (useHeuristic)
        {
            board.applyMove(move, opp);
            score = Heuristic::evaluate(board, ai);
            board.undo();
        }
        else
            score = cheapMoveScore(board, move, opp);
        scored.push_back({score, std::move(move)});
    }

    const bool maximizing = (toMove == ai);
    std::sort(scored.begin(), scored.end(), [&](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
        if (useHeuristic && !maximizing)
            return a.first < b.first;
        return a.first > b.first;
    });

    for (size_t i = 0; i < scored.size(); ++i)
        moves[i] = std::move(scored[i].second);
}

int AI::alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta)
{
	_stats.explored_nodes++;
	_stats.max_depth = std::max(depth, _stats.max_depth);

    const Piece opp = Game::opponent(toMove);
    const bool maximizing = (toMove == ai);

	if (board.isWin(opp))
        return (opp == ai) ? (WIN_SCORE - depth) : (depth - WIN_SCORE);

    if (depth >= _stats.max_depth)
	{
		_stats.stopped_nodes++;
        return Heuristic::evaluate(board, ai);
	}
	if (c.get() > TIME_LIMIT)
	{
		_stats.stopped_nodes++;
        return Heuristic::evaluate(board, ai);
	}

    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(board, toMove);
    if (moves.empty())
        return Heuristic::evaluate(board, ai);

    orderMoves(board, moves, ai, toMove, false);

    if ((int)moves.size() > MAX_CANDIDATES)
        moves.resize(MAX_CANDIDATES);

    int best = maximizing ? INT_MIN : INT_MAX;
    for (Move& move : moves)
    {
        board.applyMove(move, opp);
        int score = alphabeta(board, ai, opp, depth + 1, alpha, beta);
        board.undo();

        if (maximizing)
        {
            if (score > best)
                best = score;
            if (best > alpha)
                alpha = best;
        }
        else
        {
            if (score < best)
                best = score;
            if (best < beta)
                beta = best;
        }
        if (beta <= alpha)
            break;
    }
    return best;
}

Move AI::bestMove(const Board& board, Piece ai, int max_depth_)
{
	c.start();

	_stats.max_depth = max_depth_;
	_stats.explored_nodes = 0;
	_stats.stopped_nodes = 0;
	_stats.time = 0;

    Board search = board;
    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(search, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = Game::opponent(ai);
    orderMoves(search, moves, ai, ai, true);

    Move best = moves.front();

	for (int depth = 1; depth <= max_depth_; ++depth)
	{
		if (c.get() > TIME_LIMIT)
			break;

		_stats.max_depth = depth;

		std::vector<MoveScore> scored;
		scored.reserve(moves.size());

		int alpha = INT_MIN;
		int bestScore = INT_MIN;
		Move depthBest = moves.front();
		bool completed = true;

		for (Move& move : moves)
		{
			search.applyMove(move, opp);
			int score = alphabeta(search, ai, opp, 0, alpha, INT_MAX);
			search.undo();

			scored.push_back(MoveScore{score, move});

			if (score > bestScore) { bestScore = score; depthBest = move; }
			if (bestScore > alpha) alpha = bestScore;

			if (c.get() > TIME_LIMIT) { completed = false; break; }
		}

		if (!completed)
			break;

		best = depthBest;

		std::sort(scored.begin(), scored.end(),
			[](const MoveScore& a, const MoveScore& b) { return a.score > b.score; });
		for (size_t i = 0; i < scored.size(); ++i)
			moves[i] = scored[i].m;

        _evaluated_moves.clear();
        for (const MoveScore& move : scored)
            _evaluated_moves.insert({posToHash(move.m.getPosition()), move});

		if (bestScore >= WIN_SCORE - depth || bestScore <= depth - WIN_SCORE)
			break;
	}


    _final_move = best;
	_stats.time = c.get();
	if (_stats.time >= TIME_LIMIT)
		std::cerr << "WTF OMG WHATS GOING ON NOOO (time over .5)\n";
    return best;
}
