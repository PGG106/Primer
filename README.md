## Overview
Primer is a WIP Binpack to Bulletformat parser based on the Stockfish parsing code.

## How to convert a file.
Currently Primer will automatically filter positions in check and where the best move is a capture, additionally it can optionally only filter positions based on a max score.
An example of a command that uses all the options is 
`./primer convert binpackname.binpack desiredbinname.bin --maxscore 10000`
