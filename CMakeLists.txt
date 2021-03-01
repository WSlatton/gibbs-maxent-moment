cmake_minimum_required(VERSION 3.17)
project(gibbs_maxent_moment C)

set(CMAKE_C_STANDARD 11)

add_executable(gibbs_maxent_moment src/cli.c src/gibbs_maxent_moment.c src/bv.c src/ll.c src/ll.h)
target_link_libraries(gibbs_maxent_moment m)
