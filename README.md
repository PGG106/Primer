## Overview
Primer is a WIP Binpack to Bulletformat parser based on the Stockfish parsing code.

## Filtering options
Primer can filter your data for you as it parses the binpack.
Currently it filters positions where the best move is a capture and positions where stm is in check by default.
It also offers several optional filtering options:
* #### --no-filter-captures
Disables the default filtering of positions where the best move is a capture.
* #### --no-filter-in-check
Disables the default filtering of positions where stm is in check.
* #### --max-score max_score
Filters all positions with a abs(score) \> max_score.
* #### --filter-win win_filter_score
Filters positions in a won game (from stm pov) with a score \< win_filter_score.
* #### --filter-loss loss_filter_score
Filters positions in a lost game (from stm pov) with a score \> loss_filter_score.
* #### ----limit-positions max_pos_count
Stops after having parsed max_pos_count positions, filtered positions don't count towards the total.

## How to convert a file.
Here's an example of a basic Primer command \
`./primer convert binpackname.binpack desiredbinname.bin --max-score 10000`
