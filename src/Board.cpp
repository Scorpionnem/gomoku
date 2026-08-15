#include "Board.hpp"
#include "Game.hpp"

int Board::countRay(Position position, Position direction, Piece player, int maxSteps) const {
    int count = 0;
    for (int step = 1; step <= maxSteps; ++step) {
        Position next = {position.x + step * direction.x, position.y + step * direction.y};
        if (isOutOfBounds(next) || getPiece(next) != player)
            break;
        ++count;
    }
    return count;
}

bool Board::hasFiveInARow(Position position, Piece player) const {
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

std::vector<Position> Board::getWinningStones(Position loc, Piece player) const {
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

bool Board::isInStones(const std::vector<Position>& stones, Position pos) const {
    for (const auto& s : stones)
        if (s.x == pos.x && s.y == pos.y)
            return true;
    return false;
}

bool Board::canOpponentBreakFive(Position loc, Piece player) const {
    const Piece opp = Game::opponent(player);
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

bool Board::isWin(Piece player) const {
    if (getCaptureCount(player) >= WIN_CAPTURES)
        return true;

    Move last = getLastMove();
    if (isEmpty(last.getPosition()))
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

void Board::play(const Move& move) {
    setPiece(move.getPosition(), move.getPiece());
    _history.push_back(move);
}

void Board::applyMove(const Move& move, Piece opponent) {
    play(move);

    CaptureInfo info = findCaptures(move, opponent);
    if (info.capturedCount == 0)
        return;

    Move recorded = move;
    recorded.setType(CAPTURE);
    recorded.setRemovedPositions(info.removedPositions);
    setLastMove(recorded);
    incrementCaptureCount(recorded.getPiece(), info.capturedCount);
    applyCaptures(info);
}

void Board::undo() {
    if (_history.empty())
        return;
    Move last_move = getLastMove();

    if (last_move.getType() == CAPTURE) {
        for (auto& pos : last_move.getRemovedPositions())
            setPiece(pos, Game::opponent(last_move.getPiece()));
        incrementCaptureCount(last_move.getPiece(), -static_cast<int>(last_move.getRemovedPositions().size() / 2));
    }

    setPiece(last_move.getPosition(), EMPTY);
    _history.pop_back();
}

CaptureInfo Board::findCaptures(const Move& m, Piece opponent) const {
    CaptureInfo captureInfo;
    const Position from = m.getPosition();
    const Piece me = m.getPiece();

    for (auto& d : DIRS) {
        Position p1, p2;
        if (!matchCaptureRay(from, {d[0], d[1]}, opponent, me, p1, p2))
            continue;
        captureInfo.removedPositions.push_back(p1);
        captureInfo.removedPositions.push_back(p2);
        captureInfo.capturedCount += 1;
    }
    return captureInfo;
}

int Board::countCaptureThreats(Piece player) const {
    const Piece opponent = Game::opponent(player);
    int threats = 0;

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            Position pos = {x, y};
            if (getPiece(pos) != player)
                continue;
            for (auto& d : DIRS) {
                Position p1, p2;
                if (matchCaptureRay(pos, {d[0], d[1]}, opponent, EMPTY, p1, p2))
                    ++threats;
            }
        }
    }
    return threats;
}

bool Board::matchCaptureRay(Position from, Position dir, Piece pair, Piece end, Position& p1, Position& p2) const {
    p1 = {from.x + dir.x, from.y + dir.y};
    p2 = {from.x + 2 * dir.x, from.y + 2 * dir.y};
    Position p3 = {from.x + 3 * dir.x, from.y + 3 * dir.y};

    if (isOutOfBounds(p1) || isOutOfBounds(p2) || isOutOfBounds(p3))
        return false;
    return getPiece(p1) == pair && getPiece(p2) == pair && getPiece(p3) == end;
}
