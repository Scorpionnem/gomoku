#include "Heuristic.hpp"
#include "Game.hpp"

const int CAPTURE_WEIGHT = 2000;
const int CAPTURE_THREAT_WEIGHT = 300;

int alignmentLength(const Board& board, Position pos, Position dir, Piece player)
{
    int length = 0;
    Position cur = pos;
    while (!board.isOutOfBounds(cur) && board.getPiece(cur) == player)
    {
        length++;
        cur = {cur.x + dir.x, cur.y + dir.y};
    }
    return (length);
}

int evalPattern(int length, int openEnds)
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

int alignementScore(const Board& board, Piece player)
{
    int score = 0;

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            Position pos{x, y};
            if (board.getPiece(pos) != player)
                continue ;

            for (auto& d : AXES){
                Position dir = {d[0], d[1]};
                Position back = {pos.x - dir.x, pos.y - dir.y};
                if (!board.isOutOfBounds(back) && board.getPiece(back) == player)
                    continue ;

                int length = alignmentLength(board, pos, dir, player);
                Position front = {pos.x + length * dir.x, pos.y + length * dir.y};
                bool backOpen = !board.isOutOfBounds(back) && board.getPiece(back) == EMPTY;
                bool frontOpen = !board.isOutOfBounds(front) && board.getPiece(front) == EMPTY;

                score += evalPattern(length, (backOpen ? 1 : 0) + (frontOpen ? 1 : 0));
            }
        }
    }
    return score;
}

int captureThreats(const Board& board, Piece player)
{
    const Piece opponent = Game::opponent(player);
    int threats = 0;

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            Position pos = {x, y};
            if (board.getPiece(pos) != player)
                continue ;

            for (auto& d : DIRS)
            {
                Position p1 = {pos.x + d[0], pos.y + d[1]};
                Position p2 = {pos.x + 2 * d[0], pos.y + 2 * d[1]};
                Position p3 = {pos.x + 3 * d[0], pos.y + 3 * d[1]};
                if (board.isOutOfBounds(p1) || board.isOutOfBounds(p2) || board.isOutOfBounds(p3))
                    continue ;

                if (board.getPiece(p1) == opponent && board.getPiece(p2) == opponent && board.getPiece(p3) == EMPTY)
                    threats++;
            }
        }
    }
    return (threats);
}

int Heuristic::evaluate(const Board& board, Piece player)
{
    Piece   opponent = Game::opponent(player);

    int player_score = board.getCaptureCount(player) * CAPTURE_WEIGHT;
    player_score += alignementScore(board, player);
    player_score += captureThreats(board, player) * CAPTURE_THREAT_WEIGHT;

    int enemy_score = board.getCaptureCount(opponent) * CAPTURE_WEIGHT;
    enemy_score += alignementScore(board, opponent);
    enemy_score += captureThreats(board, opponent) * CAPTURE_THREAT_WEIGHT;

    return (player_score - enemy_score);
}
