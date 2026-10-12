cmake_minimum_required(VERSION 3.16)
project(winmac_demo C)

# 共有ライブラリ: Windows=.dll / Mac=.dylib / Linux=.so
add_library(winmac_demo SHARED src/winmac_demo.c)
target_compile_definitions(winmac_demo PRIVATE WINMAC_BUILDING)
target_include_directories(winmac_demo PUBLIC src)
set_target_properties(winmac_demo PROPERTIES C_VISIBILITY_PRESET hidden)

# 読み込みテスト（ライブラリを実際に開いて関数を呼ぶ）
enable_testing()
add_executable(load_test tests/load_test.c)
if(NOT WIN32)
  target_link_libraries(load_test PRIVATE ${CMAKE_DL_LIBS})
endif()
add_test(NAME load_test COMMAND load_test $<TARGET_FILE:winmac_demo>)
