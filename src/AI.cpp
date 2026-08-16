#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"
#include "Chrono.hpp"
#include "Game.hpp"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

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

int AI::cheapMoveScore(const Board& board, const Move& move, Piece opponent)
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

    std::sort(scored.begin(), scored.end(), [&](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
        return a.first > b.first;
    });

    for (size_t i = 0; i < scored.size(); ++i)
        moves[i] = std::move(scored[i].second);
}

int AI::toTT(int score, int depth)
{
    if (score > WIN_SCORE / 2)
        return score + depth;
    if (score < -WIN_SCORE / 2)
        return score - depth;
    return score;
}

int AI::fromTT(int score, int depth)
{
    if (score > WIN_SCORE / 2)
        return score - depth;
    if (score < -WIN_SCORE / 2)
        return score + depth;
    return score;
}

void AI::storeTT(u64 key, int height, int depth, int score, Bound bound, const Move& best)
{
    TTEntry& e = _tt[key & (TT_SIZE - 1)];
    if (e.key == key && e.depth > height)
        return;
    e.key = key;
    e.depth = height;
    e.score = toTT(score, depth);
    e.bound = bound;
    e.best = best;
}

void AI::putTTMoveFirst(std::vector<Move>& moves, const Move& ttMove)
{
    if (ttMove.getPiece() == EMPTY)
        return;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        if (moves[i].getPosition() == ttMove.getPosition())
        {
            if (i != 0)
                std::swap(moves[0], moves[i]);
            return;
        }
    }
}

int AI::alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta)
{
	_stats.explored_nodes++;
	_stats.max_depth = std::max(_stats.max_depth, depth);

    const Piece opp = Game::opponent(toMove);
    const bool maximizing = (toMove == ai);
    const int height = _stats.depth_limit - depth;
    const u64 key = board.hash() ^ (toMove == BLACK ? 0 : Board::ZOB_SIDE);

	if (board.isWin(opp))
    {
        int score = (opp == ai) ? (WIN_SCORE - depth) : (depth - WIN_SCORE);
        storeTT(key, 10000, depth, score, Bound::EXACT, {});
        return score;
    }

	if (_time.get() > TIME_LIMIT)
	{
		_stats.stopped_nodes++;
        return Heuristic::evaluate(board, ai);
	}

    if (depth >= _stats.depth_limit)
	{
		_stats.max_depth_nodes++;
        return Heuristic::evaluate(board, ai);
	}

    TTEntry& e = _tt[key & (TT_SIZE - 1)];
    Move ttMove;
    if (e.key == key)
    {
        ttMove = e.best;
        if (e.depth >= height)
        {
            int s = fromTT(e.score, depth);
            if (e.bound == Bound::EXACT)
                return s;
            if (e.bound == Bound::LOWER)
                alpha = std::max(alpha, s);
            if (e.bound == Bound::UPPER)
                beta = std::min(beta, s);
            if (alpha >= beta)
                return s;
        }
    }

    const int alphaOrig = alpha;

    std::vector<Move> moves = Move::getLegalMoves(board, toMove);
    if (moves.empty())
        return Heuristic::evaluate(board, ai);

    orderMoves(board, moves, ai, toMove, false);
    putTTMoveFirst(moves, ttMove);

    if ((int)moves.size() > MAX_CANDIDATES)
        moves.resize(MAX_CANDIDATES);

    int best = maximizing ? INT_MIN : INT_MAX;
    Move nodeBest = moves.front();
    bool timeout = false;

    for (Move& move : moves)
    {
        if (_time.get() > TIME_LIMIT)
        {
            timeout = true;
            break;
        }

        board.applyMove(move, opp);
        int score = alphabeta(board, ai, opp, depth + 1, alpha, beta);
        board.undo();

        if (maximizing)
        {
            if (score > best)
            {
                best = score;
                nodeBest = move;
            }
            if (best > alpha)
                alpha = best;
        }
        else
        {
            if (score < best)
            {
                best = score;
                nodeBest = move;
            }
            if (best < beta)
                beta = best;
        }
        if (beta <= alpha)
            break;
    }

    if (timeout)
    {
        if (best == INT_MIN || best == INT_MAX)
            return Heuristic::evaluate(board, ai);
        return best;
    }

    Bound bound = Bound::EXACT;
    if (best <= alphaOrig)
        bound = Bound::UPPER;
    else if (best >= beta)
        bound = Bound::LOWER;
    storeTT(key, height, depth, best, bound, nodeBest);

    return best;
}

Move AI::bestMove(const Board& board, Piece ai, int max_depth_)
{
	_time.start();

	_stats.depth_limit = max_depth_;
	_stats.max_depth = 0;
	_stats.max_depth_nodes = 0;
	_stats.explored_nodes = 0;
	_stats.stopped_nodes = 0;
	_stats.time = 0;
    std::fill(_tt.begin(), _tt.end(), TTEntry{});

    Board search = board;
    std::vector<Move> moves = Move::getLegalMoves(search, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = Game::opponent(ai);
    orderMoves(search, moves, ai, ai, true);

    Move best = moves.front();

	for (int depth = 1; depth <= max_depth_; ++depth)
	{
		if (_time.get() > TIME_LIMIT)
			break;

		_stats.depth_limit = depth;

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

			if (_time.get() > TIME_LIMIT) { completed = false; break; }
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
	_stats.time = _time.get();
	if (_stats.time >= TIME_LIMIT + 0.005)
		std::cerr << "WTF OMG WHATS GOING ON NOOO (time over " << TIME_LIMIT << ")\n";
    return best;
}
