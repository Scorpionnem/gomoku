#include "Game.hpp"

#include <readline/readline.h>

int main() {
    Game game;
    Move move;

    char *input;
    int round = 1;
    while ((input = readline("> ")) != nullptr) {
        if (std::string(input) == "exit") {
            free(input);
            break;
        }
        std::string text = input;
        int x = std::stoi(text.substr(0, text.find(',')));
        int y = std::stoi(text.substr(text.find(',') + 1));
        
        if (round % 2 == 0) game.setCurrentPlayer(WHITE);
        else game.setCurrentPlayer(BLACK);


        game.getBoard().play({x, y, game.getCurrentPlayer()});
        game.getBoard().printBoard();
        round++;
        free(input);
    }
    game.getBoard().printBoard();
    return 0;
}