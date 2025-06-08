add_library(${PROJECT_NAME} SHARED)

target_compile_definitions(${PROJECT_NAME} PRIVATE CONFF_EXPORTS=1)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

target_sources(${PROJECT_NAME} PRIVATE conff.cpp md5.cpp)

