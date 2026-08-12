#include "TerminalUI.hpp"

#include <csignal>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace TerminalUI {
namespace {

termios g_oldTerm{};
bool g_rawEnabled = false;
std::string g_status;

void restoreTerminal() {
    if (!g_rawEnabled)
        return;
    tcsetattr(STDIN_FILENO, TCSANOW, &g_oldTerm);
    g_rawEnabled = false;
    std::cout << "\033[?25h" << std::flush;
}

void setRaw(bool enable) {
    if (enable) {
        if (g_rawEnabled)
            return;
        if (tcgetattr(STDIN_FILENO, &g_oldTerm) == -1)
            return;
        termios raw = g_oldTerm;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        g_rawEnabled = true;
        std::cout << "\033[?25l" << std::flush;
    } else {
        restoreTerminal();
    }
}

void onSignal(int) {
    restoreTerminal();
    std::_Exit(1);
}

enum Key {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_UNDO,
    KEY_QUIT,
    KEY_OTHER,
};

Key readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return KEY_NONE;

    if (c == '\033') {
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return KEY_OTHER;
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return KEY_OTHER;
        if (seq[0] == '[') {
            if (seq[1] == 'A') return KEY_UP;
            if (seq[1] == 'B') return KEY_DOWN;
            if (seq[1] == 'C') return KEY_RIGHT;
            if (seq[1] == 'D') return KEY_LEFT;
        }
        return KEY_OTHER;
    }

    if (c == ' ')
        return KEY_SPACE;
    if (c == 'u' || c == 'U')
        return KEY_UNDO;
    if (c == 'q' || c == 'Q')
        return KEY_QUIT;
    return KEY_OTHER;
}

// Game logic for playing a move
void playMove(Game& game, int x, int y) {
    Move playerMove = {x, y, game.getCurrentPlayer()};

    if (Move::isIllegalMove(game.getBoard(), playerMove)) {
        g_status = "Invalid / illegal move";
        return;
    }

    game.getBoard().play(playerMove);

    CaptureInfo captureInfo = game.getBoard().findCaptures(
        playerMove,
        game.opponent()
    );

    if (captureInfo.capturedCount > 0) {
        playerMove.setType(CAPTURE);
        playerMove.setRemovedPositions(captureInfo.removedPositions);
        game.getBoard().setLastMove(playerMove);
        game.getBoard().incrementCaptureCount(
            game.getCurrentPlayer(),
            captureInfo.capturedCount
        );
        game.getBoard().applyCaptures(captureInfo);
        g_status = "Capture! +" + std::to_string(captureInfo.capturedCount);
    }
    else g_status.clear();

    if (game.getBoard().isWin(game.getCurrentPlayer())) {
        const char* winner =
            game.getCurrentPlayer() == BLACK ? "Red" : "Blue";
        g_status = std::string(winner) + " wins!";
        return;
    }

    game.setCurrentPlayer(game.opponent());
}

void refresh(Game& game, int cx, int cy) {
    Board::clearScreen();
    Move::printIllegalMoves(game.getBoard(), game.getCurrentPlayer(), cx, cy);

    const char* player =
        game.getCurrentPlayer() == BLACK ? "Red" : "Blue";

    std::cout << "Player: " << player
              << "  |  cursor: (" << cx << "," << cy << ")\n";
    std::cout << "White captures: " << game.getBoard().getCaptureCount(WHITE)
              << "  |  Black captures: " << game.getBoard().getCaptureCount(BLACK)
              << "\n";
    std::cout << "Arrows: move  |  Space: play  |  u: undo  |  q: quit\n";
    if (!g_status.empty())
        std::cout << g_status << "\n";
    std::cout << std::flush;
}

} // namespace

void run(Game& game) {
    std::atexit(restoreTerminal);
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    int cx = BOARD_SIZE / 2;
    int cy = BOARD_SIZE / 2;

    setRaw(true);
    refresh(game, cx, cy);

    while (true) {
        Key key = readKey();

        if (key == KEY_QUIT)
            break;
        if (key == KEY_UP && cx > 0)
            --cx;
        else if (key == KEY_DOWN && cx < BOARD_SIZE - 1)
            ++cx;
        else if (key == KEY_LEFT && cy > 0)
            --cy;
        else if (key == KEY_RIGHT && cy < BOARD_SIZE - 1)
            ++cy;
        else if (key == KEY_UNDO) {
            Move last = game.getBoard().getLastMove();
            if (last.getX() == BOARD_SIZE && last.getY() == BOARD_SIZE) {
                g_status = "Nothing to undo";
            } else {
                game.getBoard().undo();
                game.setCurrentPlayer(game.opponent());
                g_status = "Undo";
            }
        } else if (key == KEY_SPACE) {
            playMove(game, cx, cy);
        }

        refresh(game, cx, cy);
    }

    setRaw(false);
    Board::clearScreen();
}

} // namespace TerminalUI
