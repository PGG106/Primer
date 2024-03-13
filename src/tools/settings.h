#pragma once

struct parser_settings{
    bool filter_checks = true;
    bool filter_captures = true;
    bool filter_score = false;
    int max_score;
    bool filter_win = false;
    int win_filter_score;
    bool filter_loss = false;
    int loss_filter_score;
    bool filter_ply = false;
    int min_ply;
    bool position_limit = false;
    std::size_t max_pos_count;
};