add_executable(PerftTest perft-test.cpp)
target_link_libraries(PerftTest Game)

add_executable(AiPlayerTest ai-player-test.cpp)
target_link_libraries(AiPlayerTest Game)

add_executable(PerformanceTest performance-test.cpp)
target_link_libraries(PerformanceTest Game)
