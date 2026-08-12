#ifndef MANDA_MOVE_HPP
#define MANDA_MOVE_HPP

#include <vector>

class Board;

enum Piece {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    ILLEGAL = 3,
};

class Move {

    private:
        int _x;
        int _y;
        Piece _piece;

    public:
        Move(): _x(0), _y(0), _piece(EMPTY) {};
        Move(int x, int y, Piece piece): _x(x), _y(y), _piece(piece) {};

        int getX() const { return _x; }
        int getY() const { return _y; }
        Piece getPiece() const { return _piece; }

        void setX(int x) { _x = x; }
        void setY(int y) { _y = y; }
        void setPiece(Piece piece) { _piece = piece; }

        std::vector<Move> getIllegalMoves(Board board, Piece player);
        bool countPattern(
            Board& board,
            int x,
            int y,
            int dx,
            int dy,
            Piece player,
            const std::vector<int>& pattern
        ) const;
        bool isDoubleThree(Board board, Move m, Piece player) const;
        bool isFreeThree(Board board, int x, int y, int dx, int dy, Piece player) const;
        
        // TEMPORARY DEBUG FUNCTION
        static void printIllegalMoves(Board board, Piece player);
        
        static bool isIllegalMove(Board board, Move m);
    };

#endif