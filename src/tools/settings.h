#pragma once

struct parser_settings{
    bool filter_checks = true;
    bool filter_captures = true;
    bool filter_score = false;
    int max_score;
};