#ifndef MANDA_BOARD_HPP
# define MANDA_BOARD_HPP

# include <algorithm>
# include <vector>
# include "Move.hpp"
# include <iostream>

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

    public:
        Board() { std::fill(&_board[0], &_board[0] + BOARD_SIZE * BOARD_SIZE, EMPTY); };
        ~Board() {};

		Piece*      getBoard() {return (_board);}
        Piece       getPiece(Position position) const { return _board[position.x + BOARD_SIZE * position.y]; }
        bool        isEmpty(Position position) const { return !isOutOfBounds(position) && getPiece(position) == EMPTY; }
        int         countRay(Position position, Position direction, Piece player, int maxSteps) const;

        bool        isWin(Piece player) const;
        void        play(const Move& move);
        void        applyMove(const Move& move, Piece opponent);
        void        undo();

        void        setPiece(Position position, Piece piece) { _board[position.x + BOARD_SIZE * position.y] = piece; }
        void        setLastMove(const Move& move) { _history.back() = move; }

        Move        getLastMove() const { if (_history.empty()) return {{BOARD_SIZE, BOARD_SIZE}, EMPTY}; return _history.back(); };
        int         getCaptureCount(Piece player) const { if (player == BLACK || player == WHITE) return _captureCount[player]; return -1; }

        void        incrementCaptureCount(Piece player, int captured) { if (player == BLACK || player == WHITE) _captureCount[player] += captured; }
        bool        isOutOfBounds(Position position) const { return position.x < 0 || position.x >= BOARD_SIZE || position.y < 0 || position.y >= BOARD_SIZE; }

        CaptureInfo findCaptures(const Move& m, Piece opponent) const;
        int         countCaptureThreats(Piece player) const;
        void        applyCaptures(const CaptureInfo& captureInfo) { for (auto& pos : captureInfo.removedPositions) _board[pos.x + BOARD_SIZE * pos.y] = EMPTY;}


        void printBoard() {
            std::cout << "   ";
            for (int j = 0; j < BOARD_SIZE; j++)
                std::cout << (j % 10) << " ";
            std::cout << "\n";
            for (int i = 0; i < BOARD_SIZE; i++) {
                std::cout << (i < 10 ? " " : "") << i << " ";
                for (int j = 0; j < BOARD_SIZE; j++) {
                    switch (_board[j + BOARD_SIZE * i]) {
                        case BLACK:
                            std::cout << "\033[31;1m●\033[0m" << " ";
                            break;
                        case WHITE:
                            std::cout << "\033[34;1m●\033[0m" << " ";
                            break;
                        case ILLEGAL:
                            std::cout << "\033[32;1m▴\033[0m" << " ";
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
