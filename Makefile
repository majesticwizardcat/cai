SOURCE = source/game/*.cpp source/ai/*.cpp source/tools/*.cpp
INCLUDES = -Isource -Ilibraries
NAME = cai
OPTIONS = -O3 -pthread -std=c++17 -Wno-narrowing
DEBUG_OPTIONS = -pthread -std=c++17 -Wno-narrowing
MAIN = source/cai.cpp
AI_TEST = ai-test
FEN_TEST = fen-test
GAME_AI_TEST = game-ai-test

default: clean
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} ${MAIN} -o ${NAME}

clean:
	rm -rf ${NAME}

debug: clean
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} ${MAIN} -o ${NAME}

clean-fen-test:
	rm -rf ${FEN_TEST}

fen-test: clean-fen-test
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} tests/fen-test.cpp -o ${FEN_TEST}

clean-ai-test:
	rm -rf ${AI_TEST}

ai-test: clean-ai-test
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} tests/ai-test.cpp -o ${AI_TEST}

clean-game-ai:
	rm -rf ${GAME_AI_TEST}

game-ai: clean-game-ai
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} tests/game-ai.cpp -o ${GAME_AI_TEST}

