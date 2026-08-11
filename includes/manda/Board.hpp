#ifndef MANDA_BOARD_HPP
#define MANDA_BOARD_HPP

#include <vector>
#include <iostream>
#include "Move.hpp"

# define BOARD_SIZE 19

class Board {

    private:
        Piece                       _board[BOARD_SIZE][BOARD_SIZE];
        std::vector<Move>           _moves;

    public:
        Board() {
            for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++)
                    _board[i][j] = EMPTY;
        };
        
        ~Board() {};


        void play(Move move) {
            _board[move.getX()][move.getY()] = move.getPiece();
            _moves.push_back(move);
        };
        
        void undo() {
            if (_moves.empty())
                return;
            Move last_move = getLastMove();
            _board[last_move.getX()][last_move.getY()] = EMPTY;
            _moves.pop_back();
        }

        Move getLastMove() const {
            if (_moves.empty())
                return {BOARD_SIZE, BOARD_SIZE, EMPTY};
            return _moves.back();
        };

        Piece getPiece(int x, int y) const { return _board[x][y]; }

        void printBoard() {
            std::cout << "   ";
            for (int j = 0; j < BOARD_SIZE; j++)
                std::cout << (j % 10) << " ";
            std::cout << "\n";
            for (int i = 0; i < BOARD_SIZE; i++) {
                std::cout << (i < 10 ? " " : "") << i << " ";
                for (int j = 0; j < BOARD_SIZE; j++) {
                    switch (_board[i][j]) {
                        case BLACK:
                            std::cout << "\033[31;1m●\033[0m" << " ";
                            break;
                        case WHITE:
                            std::cout << "\033[34;1m●\033[0m" << " ";
                            break;
                        default:
                            std::cout << "\033[90m·\033[0m" << " ";
                            break;
                    }
                }
                std::cout << "\n";
            }
        }
};

#endif