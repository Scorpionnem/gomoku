#ifndef MANDA_BOARD_HPP
#define MANDA_BOARD_HPP

#include <vector>
#include <iostream>
#include "Move.hpp"

# define BOARD_SIZE 19
# define WIN_CAPTURES 5

struct CaptureInfo {
    int capturedCount = 0;
    std::vector<std::pair<int,int>> removedPositions = {};
};

class Board {

    private:
        Piece                       _board[BOARD_SIZE][BOARD_SIZE];
        std::vector<Move>           _history;
        int                         _captureCount[2] = {0, 0};

    public:
        Board() {
            for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++)
                    _board[i][j] = EMPTY;
        };
        
        ~Board() {};

		auto	getBoard() {return (_board);}

        void play(Move move) {
            _board[move.getX()][move.getY()] = move.getPiece();
            _history.push_back(move);
        };

        bool hasFiveInARow(int x, int y, Piece player) const {
            if (getPiece(x, y) != player)
                return false;

            const int dirs[4][2] = {
                {1, 0},
                {0, 1},
                {1, 1},
                {1, -1}
            };

            for (auto& d : dirs) {
                int count = 1;

                for (int step = 1; step < 5; ++step) {
                    int nx = x + step * d[0];
                    int ny = y + step * d[1];
                    if (isOutOfBounds(nx, ny) || getPiece(nx, ny) != player)
                        break;
                    ++count;
                }

                for (int step = 1; step < 5; ++step) {
                    int nx = x - step * d[0];
                    int ny = y - step * d[1];
                    if (isOutOfBounds(nx, ny) || getPiece(nx, ny) != player)
                        break;
                    ++count;
                }

                if (count >= 5)
                    return true;
            }
            return false;
        }

        bool isWin(Piece player) {
            if (getCaptureCount(player) >= WIN_CAPTURES * 2)
                return true;

            Move last = getLastMove();
            if (last.getPiece() != player)
                return false;

            return hasFiveInARow(last.getX(), last.getY(), player);
        }

        void undo() {
            if (_history.empty())
                return;
            Move last_move = getLastMove();

            if (last_move.getType() == CAPTURE) {
                for (auto& pos : last_move.getRemovedPositions())
                    _board[pos.first][pos.second] = last_move.getPiece() == BLACK ? WHITE : BLACK;
                incrementCaptureCount(last_move.getPiece(), -last_move.getRemovedPositions().size());
            }

            _board[last_move.getX()][last_move.getY()] = EMPTY;
            _history.pop_back();
        }

        void setPiece(int x, int y, Piece piece) { _board[x][y] = piece; }
        void setLastMove(Move move) { _history.back() = move; }

        bool isOutOfBounds(int x, int y) const { return x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE; }

        Move getLastMove() const {
            if (_history.empty())
                return {BOARD_SIZE, BOARD_SIZE, EMPTY};
            return _history.back();
        };

        void incrementCaptureCount(Piece player, int captured) {
            if (player == BLACK || player == WHITE)
                _captureCount[player] += captured;
        }

        int getCaptureCount(Piece player) const {
            if (player == BLACK || player == WHITE)
                return _captureCount[player];
            return -1;
        }

        Piece getPiece(int x, int y) const { return _board[x][y]; }

        CaptureInfo findCaptures(const Move& m, Piece opponent) {
            const Piece me = m.getPiece();
            const int dirs[8][2] = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1},
                {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
            };
            CaptureInfo captureInfo ;
            const int x = m.getX();
            const int y = m.getY();
            for (auto& d : dirs) {
                int x1 = x + d[0],     y1 = y + d[1];
                int x2 = x + 2*d[0],   y2 = y + 2*d[1];
                int x3 = x + 3*d[0],   y3 = y + 3*d[1];
                if (isOutOfBounds(x1, y1) || isOutOfBounds(x2, y2) || isOutOfBounds(x3, y3))
                    continue;
                if (getPiece(x1, y1) == opponent &&
                    getPiece(x2, y2) == opponent &&
                    getPiece(x3, y3) == me) {
                    captureInfo.removedPositions.push_back({x1, y1});
                    captureInfo.removedPositions.push_back({x2, y2});
                    captureInfo.capturedCount += 2;
                }
            }
            return captureInfo;
        }

        void applyCaptures(CaptureInfo captureInfo) {
            for (auto& pos : captureInfo.removedPositions)
                _board[pos.first][pos.second] = EMPTY;
        }
};

#endif
