cmake_minimum_required(VERSION 2.6)
project(glShadows)


include_directories(
    contrib/msvc15-windows-x86/include
    contrib/common
)

link_directories(
    ${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/lib
)



add_definitions(-DSDL_MAIN_HANDLED)

add_executable(glShadows
	src/main.cpp
	src/Camera.h
	src/Entity.h
	src/Shader.h
	src/Texture.h
	src/Time.h
	src/Mesh.h
	src/Model.h

	src/Camera.cpp
	src/Entity.cpp
	src/Shader.cpp
	src/Texture.cpp
	src/Time.cpp
	src/Mesh.cpp
	src/Model.cpp

	src/basicColor.vert
	src/basicColor.frag
	src/standardShader.vert
	src/standardShader.frag
	src/lightShader.vert
	src/lightShader.frag
)

target_link_libraries(glShadows
    SDL2 glew32 opengl32 assimp-vc141-mtd
)

add_custom_command(TARGET glShadows POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/SDL2.dll"
      "$<TARGET_FILE_DIR:glShadows>"
)
add_custom_command(TARGET glShadows POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/assimp-vc141-mtd.dll"
      "$<TARGET_FILE_DIR:glShadows>"
)

add_custom_command(TARGET glShadows POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/glew32.dll"
      "$<TARGET_FILE_DIR:glShadows>"
)
