#include "Game.hpp"

#include <readline/readline.h>

int main() {
    Game game;

    char *input;
    while ((input = readline("> ")) != nullptr) {

        std::string text = input;

        if (text == "undo") {
            game.getBoard().undo();
            game.setCurrentPlayer(game.opponent());
            game.getBoard().printBoard();
            continue;
        }

        int x = std::stoi(text.substr(0, text.find(',')));
        int y = std::stoi(text.substr(text.find(',') + 1));
        Move playerMove = {x, y, game.getCurrentPlayer()};
        
        // Check if the move is illegal
        if (Move::isIllegalMove(game.getBoard(), playerMove)) {
            free(input);
            continue;
        }

        // Play the move
        game.getBoard().play(playerMove);

        // Apply captures
        CaptureInfo captureInfo = game.getBoard().findCaptures(
            playerMove,
            game.opponent()
        );

        if (captureInfo.capturedCount > 0) {
            std::cout << "Captures: " << captureInfo.capturedCount << std::endl;
            playerMove.setType(CAPTURE);
            playerMove.setRemovedPositions(captureInfo.removedPositions);

            game.getBoard().setLastMove(playerMove);
            game.getBoard().incrementCaptureCount(game.getCurrentPlayer(), captureInfo.capturedCount);

            game.getBoard().applyCaptures(captureInfo);
        }

        // Switch player
        game.setCurrentPlayer(game.opponent());

        // DEBUG: Print the illegal moves
        Move::printIllegalMoves(game.getBoard(), game.getCurrentPlayer());
        std::cout << "White captures: " << game.getBoard().getCaptureCount(WHITE) << std::endl;
        std::cout << "Black captures: " << game.getBoard().getCaptureCount(BLACK) << std::endl;
        
        free(input);
    }
    return 0;
}