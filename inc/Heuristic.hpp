#ifndef MANDA_HEURISTIC_HPP
# define MANDA_HEURISTIC_HPP

# include "Board.hpp"

class Heuristic {
    public:
        static int evaluate(const Board& board, Piece player);
};

#endif
