#ifndef MANDA_MOVE_HPP
#define MANDA_MOVE_HPP

#include <vector>

class Board;

struct Position {
    int x = 0;
    int y = 0;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

enum MoveType {
    PLACEMENT = 0,
    CAPTURE = 1,
};

enum Piece {
    BLACK = 0,
    WHITE = 1,
    EMPTY = 2,
    ILLEGAL = 3
};

class Move {

    private:
        Position                _position = {0, 0};
        Piece                   _piece = EMPTY;
        MoveType                _type = PLACEMENT;
        std::vector<Position>   _removedPositions = {};

        bool                    countPattern(Board& board, Position position, Position direction, Piece player, const std::vector<int>& pattern) const;
        bool                    isDoubleThree(Board board, Move m, Piece player) const;
        bool                    isFreeThree(Board board, Position position, Position direction, Piece player) const;

    public:
        Move() {};
        Move(Position position, Piece piece) : _position(position), _piece(piece) {};
        Move(Position position, Piece piece, MoveType type) : _position(position), _piece(piece), _type(type) {};

        Position                getPosition() const { return _position; }
        Piece                   getPiece() const { return _piece; }
        MoveType                getType() const { return _type; }
        std::vector<Position>   getRemovedPositions() const { return _removedPositions; }

        void                    setPosition(Position position) { _position = position; }
        void                    setPiece(Piece piece) { _piece = piece; }
        void                    setType(MoveType type) { _type = type; }
        void                    setRemovedPositions(std::vector<Position> removedPositions) { _removedPositions = removedPositions; }

        std::vector<Move>       getIllegalMoves(Board board, Piece player);
        static bool             isIllegalMove(Board board, Move m);
    };

#endif