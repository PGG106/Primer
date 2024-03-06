#include <stdint.h>

struct ChessBoard
{
    uint64_t occupancy;
    uint8_t pieces[16];
    int16_t score;
    uint8_t result;
    uint8_t king_square;
    uint8_t opp_king_square;
};