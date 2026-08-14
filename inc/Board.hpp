#ifndef MANDA_BOARD_HPP
#define MANDA_BOARD_HPP

#include <vector>
#include "Move.hpp"

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
        Piece                       _board[BOARD_SIZE][BOARD_SIZE];
        std::vector<Move>           _history;
        int                         _captureCount[2] = {0, 0};

        int countRay(Position position, Position direction, Piece player, int maxSteps) const {
            int count = 0;
            for (int step = 1; step <= maxSteps; ++step) {
                Position next = {position.x + step * direction.x, position.y + step * direction.y};
                if (isOutOfBounds(next) || getPiece(next) != player)
                    break;
                ++count;
            }
            return count;
        }

        bool hasFiveInARow(Position position, Piece player) const {
            if (getPiece(position) != player)
                return false;

            for (auto& d : AXES) {
                int count = 1
                    + countRay(position, {d[0], d[1]}, player, 4)
                    + countRay(position, {-d[0], -d[1]}, player, 4);
                if (count >= 5)
                    return true;
            }
            return false;
        }

        std::vector<Position> getWinningStones(Position loc, Piece player) const {
            std::vector<Position> stones;
            if (getPiece(loc) != player)
                return stones;

            for (auto& d : AXES) {
                int fwd = countRay(loc, {d[0], d[1]}, player, BOARD_SIZE);
                int bwd = countRay(loc, {-d[0], -d[1]}, player, BOARD_SIZE);
                if (1 + fwd + bwd < 5)
                    continue;
                stones.push_back(loc);
                for (int step = 1; step <= fwd; ++step) stones.push_back({loc.x + step * d[0], loc.y + step * d[1]});
                for (int step = 1; step <= bwd; ++step) stones.push_back({loc.x - step * d[0], loc.y - step * d[1]});
            }
            return stones;
        }

        bool isInStones(const std::vector<Position>& stones, Position pos) const {
            for (const auto& s : stones)
                if (s.x == pos.x && s.y == pos.y)
                    return true;
            return false;
        }

        bool canOpponentBreakFive(Position loc, Piece player) const {
            const Piece opp = (player == BLACK) ? WHITE : BLACK;
            const auto stones = getWinningStones(loc, player);

            for (const auto& stone : stones) {
                for (auto& d : DIRS) {
                    const Position empties[2] = {
                        {stone.x - d[0],     stone.y - d[1]},
                        {stone.x - 2 * d[0], stone.y - 2 * d[1]}
                    };

                    for (auto& empty : empties) {
                        if (isOutOfBounds(empty) || getPiece(empty) != EMPTY)
                            continue;

                        Move m{empty, opp};
                        CaptureInfo info = findCaptures(m, player);
                        if (info.capturedCount == 0)
                            continue;

                        bool hitsFive = false;
                        for (const auto& pos : info.removedPositions) {
                            if (isInStones(stones, pos)) {
                                hitsFive = true;
                                break;
                            }
                        }
                        if (!hitsFive || Move::isIllegalMove(*this, m))
                            continue;

                        Board tmp = *this;
                        tmp.play(m);
                        tmp.applyCaptures(info);
                        if (!tmp.hasFiveInARow(loc, player))
                            return true;
                    }
                }
            }
            return false;
        }

    public:
        Board() {
            for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++)
                    setPiece({i, j}, EMPTY);
        };
        
        ~Board() {};

		auto	getBoard() {return (_board);}

        void play(Move move) {
            setPiece(move.getPosition(), move.getPiece());
            _history.push_back(move);
        };

        bool isWin(Piece player) {
            if (getCaptureCount(player) >= WIN_CAPTURES)
                return true;

            Move last = getLastMove();
            if (last.getPiece() == EMPTY)
                return false;

            if (last.getPiece() == player) {
                if (!hasFiveInARow(last.getPosition(), player))
                    return false;
                return !canOpponentBreakFive(last.getPosition(), player);
            }

            if (_history.size() < 2)
                return false;
            Move pending = _history[_history.size() - 2];
            if (pending.getPiece() != player)
                return false;
            return hasFiveInARow(pending.getPosition(), player);
        }

        void undo() {
            if (_history.empty())
                return;
            Move last_move = getLastMove();

            if (last_move.getType() == CAPTURE) {
                for (auto& pos : last_move.getRemovedPositions())
                    setPiece(pos, last_move.getPiece() == BLACK ? WHITE : BLACK);
                incrementCaptureCount(last_move.getPiece(), -last_move.getRemovedPositions().size());
            }

            setPiece(last_move.getPosition(), EMPTY);
            _history.pop_back();
        }

        void setPiece(Position position, Piece piece) { _board[position.x][position.y] = piece; }
        void setLastMove(Move move) { _history.back() = move; }

        bool isOutOfBounds(Position position) const {
            return position.x < 0
                || position.x >= BOARD_SIZE
                || position.y < 0
                || position.y >= BOARD_SIZE;
        }

        Move getLastMove() const {
            if (_history.empty())
                return {{BOARD_SIZE, BOARD_SIZE}, EMPTY};
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

        Piece getPiece(Position position) const { return _board[position.x][position.y]; }

        CaptureInfo findCaptures(const Move& m, Piece opponent) const {
            const Piece me = m.getPiece();
            CaptureInfo captureInfo;
            const int x = m.getPosition().x;
            const int y = m.getPosition().y;
            for (auto& d : DIRS) {
                Position pos1 = {x + d[0], y + d[1]};
                Position pos2 = {x + 2 * d[0], y + 2 * d[1]};
                Position pos3 = {x + 3 * d[0], y + 3 * d[1]};
                if (isOutOfBounds(pos1) || isOutOfBounds(pos2) || isOutOfBounds(pos3))
                    continue;
                if (getPiece(pos1) == opponent &&
                    getPiece(pos2) == opponent &&
                    getPiece(pos3) == me) {
                    captureInfo.removedPositions.push_back(pos1);
                    captureInfo.removedPositions.push_back(pos2);
                    captureInfo.capturedCount += 1;
                }
            }
            return captureInfo;
        }

        void applyCaptures(CaptureInfo captureInfo) {
            for (auto& pos : captureInfo.removedPositions)
                _board[pos.x][pos.y] = EMPTY;
        }
};

#endif
