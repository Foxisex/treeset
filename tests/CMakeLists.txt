set(target_name_test treeset_test)

add_executable(${target_name_test})

include(CompileOptions)
set_compile_options(${target_name_test})

target_sources(
  ${target_name_test}
  PRIVATE
    tests/treeset.test.cpp
)

target_link_libraries(
  ${target_name_test}
  PRIVATE
    treeset
    gtest
)
