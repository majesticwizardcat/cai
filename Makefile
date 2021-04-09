SOURCE = source/game/*.cpp source/ai/*.cpp
INCLUDES = -Isource -Ilibraries
NAME = cai
OPTIONS = -O3 -pthread -std=c++17 -Wno-narrowing -mavx
DEBUG_OPTIONS = -pthread -std=c++17 -Wno-narrowing
MAIN = source/cai.cpp
DEPTH_TEST = depth-test
FEN_TEST = fen-test

default: clean
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} ${MAIN} -o ${NAME}

clean:
	rm -rf ${NAME}

debug: clean
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} ${MAIN} -o ${NAME}

clean-fen-test:
	rm -rf ${DEPTH_TEST}

fen-test: clean-depth-test
	g++ ${INCLUDES} ${OPTIONS} ${SOURCE} tests/fen-test.cpp -o ${FEN_TEST}

