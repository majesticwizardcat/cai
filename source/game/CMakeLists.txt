add_library(Game
	board.cpp
	game.cpp
	player.cpp
)

target_link_libraries(Game AI)
