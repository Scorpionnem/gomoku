#include "Move.hpp"
#include "Board.hpp"

void Move::printIllegalMoves(Board board, Piece player, int cursorX, int cursorY) {
    Move moveInstance;
    std::vector<Move> illegalMoves = moveInstance.getIllegalMoves(board, player);
    for (Move& move : illegalMoves) {
        move.setPiece(ILLEGAL);
        board.play(move);
    }
    board.printBoard(cursorX, cursorY);
    for (const Move& move: illegalMoves) {
        (void)move;
        board.undo();
    }
}

bool Move::isIllegalMove(Board board, Move m) {
    Move moveInstance;
    if (
        (board.getPiece(m.getX(), m.getY()) != EMPTY) ||
        (board.isOutOfBounds(m.getX(), m.getY()))
    )
        return true;

    std::vector<Move> illegalMoves = moveInstance.getIllegalMoves(board, m.getPiece());
    
    for (Move& move : illegalMoves) {
        if (move.getX() == m.getX() && move.getY() == m.getY())
            return true;
    }
    return false;
}

std::vector<Move> Move::getIllegalMoves(Board board, Piece player) {
    std::vector<Move> moves;
    bool visited[19][19] = {{false}};

    for (int y = 0; y < 19; ++y) {
        for (int x = 0; x < 19; ++x) {
            if (board.getPiece(y, x) == EMPTY)
                continue;

            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (board.isOutOfBounds(nx, ny))
                        continue;
                    if (board.getPiece(ny, nx) != EMPTY || visited[ny][nx])
                        continue;

                    visited[ny][nx] = true;
                    Move m{ny, nx, player};

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
    int x,
    int y,
    int dx,
    int dy,
    Piece player,
    const std::vector<int>& pattern
) const{
    int len = pattern.size();

    for (int offset = 0; offset < len; ++offset) {
        if (pattern[offset] != 1) continue;

        bool match = true;

        for (int i = 0; i < len; ++i) {
            int nx = x + (i - offset) * dx;
            int ny = y + (i - offset) * dy;

            if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19) {
                match = false;
                break;
            }

            Piece cell = board.getPiece(nx, ny);

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

bool Move::isFreeThree(Board board, int x, int y, int dx, int dy, Piece player) const
{
    if (countPattern(board, x, y, dx, dy, player, {0, 1, 1, 1, 0}))
        return true;

    if (countPattern(board, x, y, dx, dy, player, {0, 1, 1, 0, 1, 0}))
        return true;

    if (countPattern(board, x, y, dx, dy, player, {0, 1, 0, 1, 1, 0}))
        return true;

    return false;
}

bool Move::isDoubleThree(Board board, Move m, Piece player) const
{
    board.play(m);
    int freeThreeCount = 0;
    const int dirs[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (auto& d : dirs)
        if (isFreeThree(board, m.getX(), m.getY(), d[0], d[1], player))
            freeThreeCount++;

    board.undo();

    return freeThreeCount >= 2;
}