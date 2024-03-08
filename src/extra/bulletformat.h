#include <stdint.h>

struct ChessBoard
{
    uint64_t occupancy; // 8 byte
    uint8_t pieces[16]; // 16 bytes
    int16_t score; // 2 bytes
    uint8_t result; // 1 byte
    uint8_t king_square; // 1 byte
    uint8_t opp_king_square; // 1 byte
    uint8_t padding[3]; // 3 bytes padding
};