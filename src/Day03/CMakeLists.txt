set(APP_SOURCES "day03.cpp" "day03_helpers.cpp" "day03_helpers.hpp")

add_executable(day03 ${APP_SOURCES})
configure_file(example.txt ${CMAKE_CURRENT_BINARY_DIR}/example.txt)
configure_file(input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt)

find_package(Catch2 3 REQUIRED)
add_executable(test_day03 "test.cpp")
target_sources(test_day03 PRIVATE "day03_helpers.hpp" "day03_helpers.cpp")
target_link_libraries(test_day03 PRIVATE Catch2::Catch2WithMain)

add_test(NAME DAY_03_Test COMMAND test_day03)