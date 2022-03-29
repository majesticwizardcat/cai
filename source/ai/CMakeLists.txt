add_library(AI
	ai-player.cpp
	ai-trainer.cpp
	cai-population.cpp
)

target_link_libraries(AI Tools)
