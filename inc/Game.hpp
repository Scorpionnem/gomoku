#pragma once

# include "Board.hpp"
# include "Move.hpp"
# include <string>

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
        static Piece opponent(Piece p) {
            return p == BLACK ? WHITE : BLACK;
        }

        static std::string toString(Piece piece) {
            switch (piece) {
                case BLACK: return "BLACK";
                case WHITE: return "WHITE";
                case EMPTY: return "EMPTY";
                case ILLEGAL: return "ILLEGAL";
                default : return ("ERROR");
            }
        }

        Piece getCurrentPlayer() const { return _currentPlayer; }
        Board& getBoard() { return _board; }
};
