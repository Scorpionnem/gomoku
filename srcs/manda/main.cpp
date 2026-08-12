#include "Game.hpp"

#include <readline/readline.h>

int main() {
    Game game;

    char *input;
    int round = 1;
    (void)round;
    while ((input = readline("> ")) != nullptr) {

        std::string text = input;
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
        round++;

        // Switch player
        if (round % 2 == 0) game.setCurrentPlayer(WHITE);
        else game.setCurrentPlayer(BLACK);

        // DEBUG: Print the board and the illegal moves
        game.getBoard().printBoard();
        Move::printIllegalMoves(game.getBoard(), game.getCurrentPlayer());
        
        free(input);
    }
    return 0;
}