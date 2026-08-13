#pragma once

#include "Board.hpp"
#include "Move.hpp"

class Game {

    private:
        Board _board;
        Piece _currentPlayer;
    
    public:
        Game() : _board(), _currentPlayer(BLACK) {};
        ~Game() {};

        void setCurrentPlayer(Piece player) { _currentPlayer = player; }

        Piece opponent() const {
            return _currentPlayer == BLACK ? WHITE : BLACK;
        }

        Piece getCurrentPlayer() const { return _currentPlayer; }
        Board& getBoard() { return _board; }
};
