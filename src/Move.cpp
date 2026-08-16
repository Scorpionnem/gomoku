#include "Move.hpp"
#include "Board.hpp"

bool Move::isIllegalMove(const Board& board, const Move& m) {
    if (board.isOutOfBounds(m.getPosition()) || board.getPiece(m.getPosition()) != EMPTY)
        return true;

    Board tmp = board;
    Move moveInstance;
    return moveInstance.isDoubleThree(tmp, m, m.getPiece());
}

std::vector<Move> Move::getNearbyMoves(const Board& board, Piece player) {
    std::vector<Move> moves;
    bool visited[BOARD_SIZE][BOARD_SIZE] = {};

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (board.isEmpty({x, y}))
                continue;

            for (int dx = -2; dx <= 2; ++dx) {
                for (int dy = -2; dy <= 2; ++dy) {
                    Position next = {x + dx, y + dy};
                    if (board.isOutOfBounds(next) || board.getPiece(next) != EMPTY) continue;
                    if (visited[next.x][next.y]) continue;

                    visited[next.x][next.y] = true;
                    moves.push_back({next, player});
                }
            }
        }
    }
    return moves;
}

std::vector<Move> Move::getIllegalMoves(Board& board, Piece player) const {
    std::vector<Move> illegal;
    for (Move& m : getNearbyMoves(board, player))
        if (isDoubleThree(board, m, player)) illegal.push_back(m);
    return illegal;
}

std::vector<Move> Move::getLegalMoves(Board& board, Piece player) {
    std::vector<Move> nearby = getNearbyMoves(board, player);
    if (nearby.empty()) return {{{BOARD_SIZE / 2, BOARD_SIZE / 2}, player}};

    std::vector<Move> legal;
    for (Move& m : nearby)
        if (!isDoubleThree(board, m, player)) legal.push_back(m);
    return legal;
}

bool Move::countPattern(
    const Board& board,
    Position position,
    Position direction,
    Piece player,
    const std::vector<int>& pattern
) {
    int len = pattern.size();

    for (int offset = 0; offset < len; ++offset) {
        if (pattern[offset] != 1) continue;

        bool match = true;

        for (int i = 0; i < len; ++i) {
            Position next = {
                position.x + (i - offset) * direction.x,
                position.y + (i - offset) * direction.y
            };

            if (board.isOutOfBounds(next)) {
                match = false;
                break;
            }

            Piece cell = board.getPiece(next);

            if (pattern[i] == 1) {
                if (cell != player) {
                    match = false;
                    break;
                }
            }
            else if (pattern[i] == 0) {
                if (cell != EMPTY) {
                    match = false;
                    break;
                }
            }
        }

        if (match)
            return true;
    }
    return false;     
};

bool Move::isFreeThree(const Board& board, Position position, Position direction, Piece player)
{
    if (countPattern(board, position, direction, player, {0, 1, 1, 1, 0}))
        return true;
    if (countPattern(board, position, direction, player, {0, 1, 1, 0, 1, 0}))
        return true;
    if (countPattern(board, position, direction, player, {0, 1, 0, 1, 1, 0}))
        return true;

    return false;
}

// Nombre de pierres `player` a distance <=3 du coup le long d'un axe (2 sens).
static int axisAllyCount(const Board& board, Position p, Position d, Piece player)
{
    int count = 0;
    for (int step = 1; step <= 3; ++step)
    {
        Position a = {p.x + step * d.x, p.y + step * d.y};
        if (!board.isOutOfBounds(a) && board.getPiece(a) == player) ++count;
        Position b = {p.x - step * d.x, p.y - step * d.y};
        if (!board.isOutOfBounds(b) && board.getPiece(b) == player) ++count;
    }
    return count;
}

// Condition necessaire (jamais de faux negatif) : un double-three impose au
// moins 2 axes ayant chacun >=2 allies proches. Filtre bon marche qui evite la
// detection complete sur l'immense majorite des coups.
static bool mightBeDoubleThree(const Board& board, Position p, Piece player)
{
    int axes = 0;
    for (auto& d : AXES)
        if (axisAllyCount(board, p, {d[0], d[1]}, player) >= 2 && ++axes >= 2)
            return true;
    return false;
}

bool Move::isDoubleThree(Board& board, const Move& m, Piece player)
{
    if (!mightBeDoubleThree(board, m.getPosition(), player))
        return false;

    board.play(m);
    int freeThreeCount = 0;

    for (auto& d : AXES)
        if (isFreeThree(board, m.getPosition(), {d[0], d[1]}, player))
            freeThreeCount++;

    board.undo();

    return freeThreeCount >= 2;
}
