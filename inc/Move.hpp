#ifndef MANDA_MOVE_HPP
#define MANDA_MOVE_HPP

#include <vector>

class Board;

enum MoveType {
    PLACEMENT = 0,
    CAPTURE = 1,
};

enum Piece {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
    ILLEGAL = 3,
};

class Move {

    private:
        int     _x;
        int     _y;
        Piece   _piece;
        MoveType _type;
        std::vector<std::pair<int,int>> _removedPositions;

    public:
        Move(): _x(0), _y(0), _piece(EMPTY), _type(PLACEMENT), _removedPositions({}) {};
        Move(int x, int y, Piece piece): _x(x), _y(y), _piece(piece), _type(PLACEMENT), _removedPositions({}) {};
        Move(int x, int y, Piece piece, MoveType type): _x(x), _y(y), _piece(piece), _type(type), _removedPositions({}) {};

        int getX() const { return _x; }
        int getY() const { return _y; }
        MoveType getType() const { return _type; }
        std::vector<std::pair<int,int>> getRemovedPositions() const { return _removedPositions; }
        Piece getPiece() const { return _piece; }

        void setX(int x) { _x = x; }
        void setY(int y) { _y = y; }
        void setPiece(Piece piece) { _piece = piece; }
        void setType(MoveType type) { _type = type; }
        void setRemovedPositions(std::vector<std::pair<int,int>> removedPositions) { _removedPositions = removedPositions; }

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
        
        static bool isIllegalMove(Board board, Move m);
    };

#endif