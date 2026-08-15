#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"
#include "Chrono.hpp"
#include "Game.hpp"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

int	AI::explored_nodes = 0;
int	AI::max_depth = 0;
int	AI::max_depth_explored = 0;
double	AI::time = 0;

Chrono	c;

static int cheapMoveScore(const Board& board, const Move& move, Piece opponent)
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

int AI::alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta) {
	explored_nodes++;
	max_depth_explored = std::max(depth, max_depth_explored);

    const Piece opp = Game::opponent(toMove);
    const bool maximizing = (toMove == ai);

    if (depth >= max_depth || c.get() > .45)
        return Heuristic::evaluate(board, ai);

    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(board, toMove);
    if (moves.empty())
        return Heuristic::evaluate(board, ai);

    orderMoves(board, moves, ai, toMove, depth <= 1);

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

	max_depth = max_depth_;
	explored_nodes = 0;
	max_depth_explored = 0;

    Board search = board;
    Move moveInstance;
    std::vector<Move> moves = moveInstance.getLegalMoves(search, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = Game::opponent(ai);
    orderMoves(search, moves, ai, ai, true);

    Move best = moves.front();
    int bestScore = INT_MIN;
    int alpha = INT_MIN;

    for (Move& move : moves) {
        if (c.get() > 0.40)
            break;
        search.applyMove(move, opp);
        int score = alphabeta(search, ai, opp, 0, alpha, INT_MAX);
        search.undo();

        if (score > bestScore) bestScore = score, best = move;
        if (bestScore > alpha) alpha = bestScore;
    }

	time = c.get();
    return best;
}
