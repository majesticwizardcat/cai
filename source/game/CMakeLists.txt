add_library(Game
	game.cpp
	player.cpp
	chess-board.cpp
)

target_link_libraries(Game NNAI)
