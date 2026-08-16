#ifndef MANDA_BOARD_HPP
# define MANDA_BOARD_HPP

# include <algorithm>
# include <vector>
# include "Move.hpp"
# include <iostream>
# include "math/stdint.hpp"

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
        Piece                   _board[BOARD_SIZE * BOARD_SIZE];
        std::vector<Move>       _history = {};
        int                     _captureCount[2] = {0, 0};

        bool                    hasFiveInARow(Position position, Piece player) const;
        std::vector<Position>   getWinningStones(Position loc, Piece player) const;
        bool                    isInStones(const std::vector<Position>& stones, Position pos) const;
        bool                    canOpponentBreakFive(Position loc, Piece player) const;
        bool                    matchCaptureRay(Position from, Position dir, Piece pair, Piece end, Position& p1, Position& p2) const;

        static u64              ZOB_PIECE[BOARD_SIZE * BOARD_SIZE][2];
        static u64              ZOB_CAPTURE[2][16];
        u64                     _hash = 0;
        static void             initZobrist();
        u64                     computeHashFromScratch() const;

    public:
        Board() {
            static bool once = (initZobrist(), true);
            (void)once;
            std::fill(&_board[0], &_board[0] + BOARD_SIZE * BOARD_SIZE, EMPTY);
        };
        ~Board() {};

		Piece*      getBoard() {return (_board);}
        Piece       getPiece(Position position) const { return _board[position.x + BOARD_SIZE * position.y]; }
        bool        isEmpty(Position position) const { return !isOutOfBounds(position) && getPiece(position) == EMPTY; }
        int         countRay(Position position, Position direction, Piece player, int maxSteps) const;

        bool        isWin(Piece player) const;
        void        play(const Move& move);
        void        applyMove(const Move& move, Piece opponent);
        void        undo();

        void        setPiece(Position position, Piece piece) { 
            Piece old = getPiece(position);
            int i = position.x + BOARD_SIZE * position.y;
            
            if (old == BLACK || old == WHITE) 
                _hash ^= ZOB_PIECE[i][old];
            _board[i] = piece;
            if (piece == BLACK || piece == WHITE)
                _hash ^= ZOB_PIECE[i][piece];
        }

        void        setLastMove(const Move& move) { _history.back() = move; }

        Move        getLastMove() const { if (_history.empty()) return {{BOARD_SIZE, BOARD_SIZE}, EMPTY}; return _history.back(); };
        int         getCaptureCount(Piece player) const { if (player == BLACK || player == WHITE) return _captureCount[player]; return -1; }

        void        incrementCaptureCount(Piece player, int captured) { 
            if (player != BLACK && player != WHITE) return;
            auto idx = [](int n) { return std::min(std::max(n, 0), 15); };
            _hash ^= ZOB_CAPTURE[player][idx(_captureCount[player])];
            _captureCount[player] += captured;
            _hash ^= ZOB_CAPTURE[player][idx(_captureCount[player])];
        }

        bool        isOutOfBounds(Position position) const { return position.x < 0 || position.x >= BOARD_SIZE || position.y < 0 || position.y >= BOARD_SIZE; }

        CaptureInfo findCaptures(const Move& m, Piece opponent) const;
        int         countCaptureThreats(Piece player) const;
        void        applyCaptures(const CaptureInfo& captureInfo) { for (auto& pos : captureInfo.removedPositions) setPiece(pos, EMPTY);}

        static u64  ZOB_SIDE;
        u64         hash() const { return _hash; }

    };

#endif
