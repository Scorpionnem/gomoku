#ifndef MANDA_GAME_HPP
#define MANDA_GAME_HPP

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

        Piece getCurrentPlayer() const { return _currentPlayer; }
        Board& getBoard() { return _board; }
        
};

#endif