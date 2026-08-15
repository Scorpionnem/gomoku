#include "Heuristic.hpp"
#include "Game.hpp"

const int CAPTURE_WEIGHT = 2000;
const int CAPTURE_THREAT_WEIGHT = 300;

static int evalPattern(int length, int openEnds)
{
    if (length >= 5)
        return (200000);
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
                score += evalPattern(length, openEnds);
            }
        }
    }
    return score;
}

static int playerScore(const Board& board, Piece player)
{
    int score = 0;
    score += board.getCaptureCount(player) * CAPTURE_WEIGHT;
    score += alignmentScore(board, player);
    score += board.countCaptureThreats(player) * CAPTURE_THREAT_WEIGHT;
    return score;
}

int Heuristic::evaluate(const Board& board, Piece player)
{
    return playerScore(board, player) - playerScore(board, Game::opponent(player));
}
