#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"
#include "Chrono.hpp"
#include "Game.hpp"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

AI::_	AI::v = {};

Chrono	c;

int cheapMoveScore(const Board& board, const Move& move, Piece opponent)
{
    int score = board.findCaptures(move, opponent).capturedCount * 10000;
    Move last = board.getLastMove();
    if (last.getPiece() != EMPTY)
    {
        int dx = move.getPosition().x - last.getPosition().x;
        int dy = move.getPosition().y - last.getPosition().y;
        score += 20 - (dx * dx + dy * dy);
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
	v.explored_nodes++;
	v.max_depth_explored = std::max(depth, v.max_depth_explored);

    const Piece opp = Game::opponent(toMove);
    const bool maximizing = (toMove == ai);

	if (board.isWin(toMove))
	{
        return Heuristic::evaluate(board, opp);
	}
    if (board.isWin(opp))
	{
        return Heuristic::evaluate(board, opp);
	}
    if (depth >= v.max_depth)
	{
		AI::v.branches_reach_end++;
        return Heuristic::evaluate(board, opp);
	}
	if (c.get() > 0.5)
	{
		AI::v.branches_cut_off++;
        return Heuristic::evaluate(board, opp);
	}

    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(board, toMove);
    if (moves.empty())
        return Heuristic::evaluate(board, opp);

    orderMoves(board, moves, ai, toMove, depth <= 2);

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

	v.max_depth = max_depth_;
	v.explored_nodes = 0;
	v.max_depth_explored = 0;
	v.branches_cut_off = 0;
	v.branches_reach_end = 0;

    Board search = board;
    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(search, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = Game::opponent(ai);
    orderMoves(search, moves, ai, ai, true);

    Move best = moves.front();
    int bestScore = INT_MIN;
	int alpha = INT_MIN;(void)alpha;

	// std::mutex	mutex;
    // for (Move& move : moves)
	// {
    //     search.applyMove(move, opp);
	// 	threads.queue_task([&mutex, &bestScore, &best, &move, search, ai, opp]()
	// 	{
	// 		Board	b = search;
	// 		int score = alphabeta(b, ai, opp, 0, INT_MIN, INT_MAX);

	// 		mutex.lock();
	// 		if (score > bestScore) bestScore = score, best = move;
	// 		mutex.unlock();
	// 	});
    //     search.undo();
    // }

	v.ai_moves.clear();

	for (Move& move : moves)
	{
        search.applyMove(move, ai);
        int score = alphabeta(search, ai, opp, 0, alpha, INT_MAX);
        search.undo();

		v.ai_moves.push_back(AI::_::MoveScore{move, score});

        if (score > bestScore) bestScore = score, best = move;
        if (bestScore > alpha) alpha = bestScore;
    }

	v.threads.wait_finish();

	v.time = c.get();
	if (v.time >= .5)
		std::cerr << "WTF OMG WHATS GOING ON NOOO (time over .5)\n";
    return best;
}
