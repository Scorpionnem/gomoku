#include "Move.hpp"
#include "Board.hpp"

bool Move::isIllegalMove(Board board, Move m) {
    Move moveInstance;
    if (
        (board.getPiece(m.getPosition()) != EMPTY) ||
        (board.isOutOfBounds(m.getPosition()))
    )
        return true;

    std::vector<Move> illegalMoves = moveInstance.getIllegalMoves(board, m.getPiece());
    
    for (Move& move : illegalMoves) {
        if (move.getPosition() == m.getPosition())
            return true;
    }
    return false;
}

std::vector<Move> Move::getIllegalMoves(Board board, Piece player) {
    std::vector<Move> moves;
    bool visited[19][19] = {{false}};

    for (int y = 0; y < 19; ++y) {
        for (int x = 0; x < 19; ++x) {
            if (board.getPiece({x, y}) == EMPTY)
                continue;

            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    Position next = {x + dx, y + dy};

                    if (board.isOutOfBounds(next))
                        continue;
                    if (board.getPiece(next) != EMPTY || visited[next.y][next.x])
                        continue;

                    visited[next.y][next.x] = true;
                    Move m{next, player};

                    if (isDoubleThree(board, m, player))
                        moves.push_back(m);
                }
            }
        }
    }
    return moves;
}

bool Move::countPattern(
    Board& board,
    Position position,
    Position direction,
    Piece player,
    const std::vector<int>& pattern
) const{
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

bool Move::isFreeThree(Board board, Position position, Position direction, Piece player) const
{
    if (countPattern(board, position, direction, player, {0, 1, 1, 1, 0}))
        return true;
    if (countPattern(board, position, direction, player, {0, 1, 1, 0, 1, 0}))
        return true;
    if (countPattern(board, position, direction, player, {0, 1, 0, 1, 1, 0}))
        return true;

    return false;
}

bool Move::isDoubleThree(Board board, Move m, Piece player) const
{
    board.play(m);
    int freeThreeCount = 0;

    for (auto& d : AXES)
        if (isFreeThree(board, m.getPosition(), {d[0], d[1]}, player))
            freeThreeCount++;

    board.undo();

    return freeThreeCount >= 2;
}
