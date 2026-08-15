#ifndef MANDA_BOARD_HPP
# define MANDA_BOARD_HPP

# include <algorithm>
# include <vector>
# include "Move.hpp"

# define BOARD_SIZE 19
# define WIN_CAPTURES 5

const int AXES[4][2] = {
    {1, 0}, {0, 1}, {1, 1}, {1, -1}
};
const int DIRS[8][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
};

struct CaptureInfo {
    int capturedCount = 0;
    std::vector<Position> removedPositions = {};
};

class Board {

    private:
        Piece                   _board[BOARD_SIZE][BOARD_SIZE];
        std::vector<Move>       _history = {};
        int                     _captureCount[2] = {0, 0};

        int                     countRay(Position position, Position direction, Piece player, int maxSteps) const;
        bool                    hasFiveInARow(Position position, Piece player) const;
        std::vector<Position>   getWinningStones(Position loc, Piece player) const;
        bool                    isInStones(const std::vector<Position>& stones, Position pos) const;
        bool                    canOpponentBreakFive(Position loc, Piece player) const;

    public:
        Board() { std::fill(&_board[0][0], &_board[0][0] + BOARD_SIZE * BOARD_SIZE, EMPTY); };
        ~Board() {};

		Piece	    (*getBoard())[BOARD_SIZE] {return (_board);}
        Piece       getPiece(Position position) const { return _board[position.x][position.y]; }

        bool        isWin(Piece player) const;
        void        play(Move move);
        void        applyMove(Move move, Piece opponent);
        void        undo();

        void        setPiece(Position position, Piece piece) { _board[position.x][position.y] = piece; }
        void        setLastMove(Move move) { _history.back() = move; }

        Move        getLastMove() const { if (_history.empty()) return {{BOARD_SIZE, BOARD_SIZE}, EMPTY}; return _history.back(); };
        int         getCaptureCount(Piece player) const { if (player == BLACK || player == WHITE) return _captureCount[player]; return -1; }

        void        incrementCaptureCount(Piece player, int captured) { if (player == BLACK || player == WHITE) _captureCount[player] += captured; }
        bool        isOutOfBounds(Position position) const { return position.x < 0 || position.x >= BOARD_SIZE || position.y < 0 || position.y >= BOARD_SIZE; }

        CaptureInfo findCaptures(const Move& m, Piece opponent) const;
        void        applyCaptures(CaptureInfo captureInfo) { for (auto& pos : captureInfo.removedPositions) _board[pos.x][pos.y] = EMPTY;}
};

#endif
