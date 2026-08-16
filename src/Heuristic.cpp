#include "Heuristic.hpp"
#include "Game.hpp"

#define WIN_WEIGHT 200000

#include <cmath>

static int evalPattern(int length, int openEnds)
{
    if (length >= 5)
        return (WIN_WEIGHT);
    if (length == 4)
        return (openEnds == 2 ? 50000 : openEnds == 1 ? 5000 : 0);
    if (length == 3)
        return (openEnds == 2 ? 1000 : openEnds == 1 ? 100 : 0);
    if (length == 2)
        return (openEnds == 2 ? 50 : openEnds == 1 ? 10 : 0);
    if (length == 1)
        return (openEnds == 2 ? 5 : 0);
    return (0);
}

static bool isVulnerable(const Board& board, Position pos, Piece player)
{
    Piece opponent = Game::opponent(player);

    for (auto& d : DIRS)
    {
        Position mate = {pos.x + d[0], pos.y + d[1]};
        Position behind = {pos.x - d[0], pos.y - d[1]};
        Position ahead = {pos.x + 2 * d[0], pos.y + 2 * d[1]};

        if (board.isOutOfBounds(mate) || board.isOutOfBounds(behind) || board.isOutOfBounds(ahead))
            continue;
        if (board.getPiece(mate) == player && board.getPiece(behind) == opponent && board.getPiece(ahead) == EMPTY)
            return true;
    }
    return false;
}

static int alignmentScore(const Board& board, Piece player)
{
    int score = 0;

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            Position pos = {x, y};
            if (board.getPiece(pos) != player)
                continue;

            for (auto& d : AXES)
            {
                Position dir = {d[0], d[1]};
                Position back = {pos.x - dir.x, pos.y - dir.y};
                if (!board.isOutOfBounds(back) && board.getPiece(back) == player)
                    continue;

                int length = 1 + board.countRay(pos, dir, player, BOARD_SIZE);
                Position front = {pos.x + length * dir.x, pos.y + length * dir.y};
                int openEnds = board.isEmpty(back) + board.isEmpty(front);

                int patternScore = evalPattern(length, openEnds);
                if (patternScore > 0)
                {
                    Position cur = pos;
                    for (int step = 0; step < length; ++step)
                    {
                        if (isVulnerable(board, cur, player))
                        {
                            patternScore /= 4;
                            break;
                        }
                        cur = {cur.x + dir.x, cur.y + dir.y};
                    }
                }
                score += patternScore;
            }
        }
    }
    return score;
}

static int playerScore(const Board& board, Piece player)
{
    int score = 0;
    score += std::ceil((std::exp(board.getCaptureCount(player)) - 1) * 1356.73);
    score += alignmentScore(board, player);
    score += std::ceil((std::exp(board.countCaptureThreats(player)) - 1) * 1356.73) / 2;
    return score;
}

int Heuristic::evaluate(const Board& board, Piece player)
{
    return playerScore(board, player) - playerScore(board, Game::opponent(player));
}
