add_executable(day01 day01.cpp)
target_compile_features(day01 PRIVATE cxx_std_14)

add_executable(day02 day02.cpp)
target_compile_features(day02 PRIVATE cxx_std_14)

add_executable(day03 day03.cpp)
target_compile_features(day03 PRIVATE cxx_std_14)

add_executable(day04 day04.cpp)
target_compile_features(day04 PRIVATE cxx_std_14)

add_executable(day05 day05.cpp)
target_compile_features(day05 PRIVATE cxx_std_14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(day05 PRIVATE Threads::Threads)

add_executable(day06 day06.cpp)
target_compile_features(day06 PRIVATE cxx_std_14)

add_executable(day07 day07.cpp)
target_compile_features(day07 PRIVATE cxx_std_14)

add_executable(day08 day08.cpp)
target_compile_features(day08 PRIVATE cxx_std_14)

add_executable(day09 day09.cpp)
target_compile_features(day09 PRIVATE cxx_std_14)

add_executable(day10 day10.cpp)
target_compile_features(day10 PRIVATE cxx_std_14)

add_executable(day11 day11.cpp)
target_compile_features(day11 PRIVATE cxx_std_14)

add_executable(day12 day12.cpp)
target_compile_features(day12 PRIVATE cxx_std_14)
