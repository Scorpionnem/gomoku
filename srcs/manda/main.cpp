#include "Game.hpp"

std::string PieceToString(Piece piece) {
    switch (piece) {
        case BLACK:
            return "BLACK";
        case WHITE:
            return "WHITE";
        default:
            return "EMPTY";
    }
}

int main() {
    Game game;
    Move move;

    game.setCurrentPlayer(BLACK);
    game.getBoard().play({9, 8, WHITE});
    game.getBoard().play({9, 7, WHITE});
    game.getBoard().play({8, 6, WHITE});
    game.getBoard().play({7, 6, WHITE});
    game.getBoard().printBoard();
    std::vector<Move> legal_moves = move.getLegalMoves(game.getBoard(), game.getCurrentPlayer());
    for (Move& legal_move : legal_moves) {
        legal_move.setPiece(BLACK);
        game.getBoard().play(legal_move);
    }
    game.getBoard().printBoard();
    return 0;
}