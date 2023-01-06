add_library(NNAI
	nnai-player.cpp
	nnai-trainer.cpp
	cai-population.cpp
)

target_link_libraries(NNAI Tools)
