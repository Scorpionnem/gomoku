#include "Board.hpp"
#include "Move.hpp"
#include "AI.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

static void place(Board& b, int x, int y, Piece p) {
    b.setPiece({x, y}, p);
}

static void printTopMoves(int n) {
    std::vector<AI::_::MoveScore> moves = AI::v.ai_moves;
    std::sort(moves.begin(), moves.end(),
        [](const AI::_::MoveScore& a, const AI::_::MoveScore& b) { return a.score > b.score; });

    std::cout << "  top " << n << " coups evalues:\n";
    for (int i = 0; i < n && i < (int)moves.size(); ++i) {
        Position pos = moves[i].m.getPosition();
        std::cout << "    (" << pos.x << "," << pos.y << ")  score=" << moves[i].score << "\n";
    }
}

static void runScenario(const char* name, Board& board, Piece ai, int depth) {
    std::cout << "\n=== " << name << " (IA = "
              << (ai == BLACK ? "BLACK/rouge" : "WHITE/bleu") << ", depth=" << depth << ") ===\n";
    board.printBoard();

    Move best = AI::bestMove(board, ai, depth);
    Position p = best.getPosition();

    std::cout << "  -> coup choisi: (" << p.x << "," << p.y << ")\n";
    std::cout << "  temps=" << AI::v.time << "s  noeuds=" << AI::v.explored_nodes
              << "  prof_max=" << AI::v.max_depth_explored << "\n";
    printTopMoves(6);
}

int main() {
    // Scenario 1: BLACK a un quatre ouvert horizontal (5..8 en y=9). Il doit completer en 5.
    {
        Board b;
        for (int x = 5; x <= 8; ++x)
            place(b, x, 9, BLACK);
        // un peu de contexte adverse pour rester dans le "nearby"
        place(b, 5, 10, WHITE);
        runScenario("Quatre ouvert -> gagner en jouant (4,9) ou (9,9)", b, BLACK, 4);
    }

    // Scenario 2: WHITE a un quatre ouvert. BLACK (IA) doit bloquer.
    {
        Board b;
        for (int x = 5; x <= 8; ++x)
            place(b, x, 9, WHITE);
        place(b, 9, 8, BLACK);
        runScenario("Bloquer le quatre ouvert adverse", b, BLACK, 4);
    }

    // Scenario 3: position ouverte simple, BLACK a un trois, doit developper.
    {
        Board b;
        place(b, 9, 9, BLACK);
        place(b, 10, 9, BLACK);
        place(b, 9, 10, WHITE);
        runScenario("Developpement en debut de partie", b, BLACK, 10);
    }

    return 0;
}
