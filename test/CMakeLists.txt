add_executable(conff_test test.cpp)

target_include_directories(conff_test PRIVATE ../include)

target_link_libraries(conff_test conff)
