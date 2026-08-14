#include "AI.hpp"
#include "Heuristic.hpp"
#include "Move.hpp"

int AI::alphabeta(Board& board, Piece ai, Piece toMove, int depth, int alpha, int beta) {
    const Piece opp = (toMove == BLACK) ? WHITE : BLACK;
    const bool maximizing = (toMove == ai);

    if (board.isWin(ai)) return WIN_SCORE + depth;
    if (board.isWin(ai == BLACK ? WHITE : BLACK)) return -WIN_SCORE - depth;
    if (depth == 0) return Heuristic::evaluate(board, ai);

    Move helper;
    std::vector<Move> moves = helper.getLegalMoves(board, toMove);
    if (moves.empty()) return Heuristic::evaluate(board, ai);

    if (maximizing) {
        int best = INT_MIN;
        for (Move& move : moves) {
            board.applyMove(move, opp);
            int score = alphabeta(board, ai, opp, depth - 1, alpha, beta);
            board.undo();

            if (score > best) best = score;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        return best;
    }

    int best = INT_MAX;
    for (Move& move : moves) {
        board.applyMove(move, opp);
        int score = alphabeta(board, ai, opp, depth - 1, alpha, beta);
        board.undo();

        if (score < best) best = score;
        if (best < beta) beta = best;
        if (beta <= alpha) break;
    }
    return best;
}

Move AI::bestMove(Board board, Piece ai, int depth) {
    Move helper;
    std::vector<Move> moves = helper.getLegalMoves(board, ai);
    if (moves.empty())
        return {{BOARD_SIZE / 2, BOARD_SIZE / 2}, ai};

    const Piece opp = (ai == BLACK) ? WHITE : BLACK;
    Move best = moves.front();
    int bestScore = INT_MIN;
    int alpha = INT_MIN;

    for (Move& move : moves) {
        board.applyMove(move, opp);
        int score = alphabeta(board, ai, opp, depth - 1, alpha, INT_MAX);
        board.undo();

        if (score > bestScore) bestScore = score, best = move;
        if (bestScore > alpha) alpha = bestScore;
    }
    return best;
}
