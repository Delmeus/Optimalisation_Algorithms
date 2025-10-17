cmake_minimum_required(VERSION 3.27)
project(algorytmy_optymalizacji)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(algorytmy_optymalizacji ../src/main.cpp
        ../src/utils/ProblemInstance.cpp
        ../src/algorithms/Genetic.cpp
        ../src/algorithms/Greedy.cpp
        ../src/utils/Solution.cpp
        ../src/utils/common_utils.hpp
        ../src/algorithms/Random.cpp
        ../src/utils/Timer.cpp
        ../src/algorithms/TabuSearch.cpp)
