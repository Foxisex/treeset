set(target_name treeset)

add_library(${target_name} STATIC
	libset/treeset.cpp)
	
include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include)


target_link_libraries(
	${target_name}
	PRIVATE
	m
)
