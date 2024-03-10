## Overview
Primer is a WIP Binpack to Bulletformat parser based on the Stockfish parsing code.

## How to convert a file.
Currently Primer can optionally filter positions in check, positions where the best move is a capture, and positions whose score (in absolute value) is bigger than a set max score.
An example of a command that uses all the options is 
`./primer convert binpackname.binpack desiredbinname.bin --filter-in-check --filter-captures --maxscore 10000`
