execute_process(
    COMMAND ${CMAKE_COMMAND}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/Platform/YuniK210Maix.cmake
    -GNinja
    ${CMAKE_CURRENT_LIST_DIR})
