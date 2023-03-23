# CAI
cai (Chess AI) is a chess engine playground for developement of chess AIs. Currently it supports 2 AIs: A fully neural network AI and a fully MinMax AI.

## Features
### Board
cai has a very minimal board and moves storage. The board only uses 4 uint64s to store the board state, 1 uint16 to store the information of the position (next player color, castles, en passant) and another uint64 to store the board hash. This makes is so the whole board fits in a single cache line, making it extremely fast. There are still improvements to be made in the future but for now it performs good enough.

### Hashing
For hashing, a table of random numbers is generated in compile time. These numbers are xored depending on the board state, creating a hash. This is called [Zobrist Hashing](https://en.wikipedia.org/wiki/Zobrist_hashing). When a move is played, the old random numbers are xored out and the new ones are xored in so the hash is not recalculated every time.

### Testing
[Perft Tests](https://www.chessprogramming.org/Perft_Results) are used to ensure the board is working properly. Due to how fast the board is, we can reach a good depth to test the board.

### Neural Networks
The NNAi has a simple implementation that works quite good for the purposes of this project. Genetic algorithms are used to incrementally find the better ai. So far unfortunately, there are no good results from this AI. Even after running it for a week, the AI is still very random. More research requires on this.

### MinMax AI
Currently WIP, the min max AI searches for all positions until a depth. As a small optimization, it uses a basic alpha-beta pruning to decrease the number of positions as well as a hash table to keep already evaluated positions.

### Multithreading
Peft tests as well min max tree expanding utilize multithreading.

## Currently WIP
Currently, I'm working on improving the Min Max AI as well as a Hybrid AI that utilizes MinMaxing and neural networks.
