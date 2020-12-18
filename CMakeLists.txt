cmake_minimum_required(VERSION 3.0)
project(glPBR)

if(MSVC)
  add_definitions(/MP)
endif()


include_directories(
    contrib/msvc15-windows-x86/include
    contrib/common
)

link_directories(
    ${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/lib
)



add_definitions(-DSDL_MAIN_HANDLED)

add_executable(glPBR
	src/main.cpp
	src/Camera.h
	src/Camera.cpp
	src/Shader.h
	src/Shader.cpp
	src/Texture.h
	src/Texture.cpp
	src/Time.h
	src/Time.cpp
	src/Mesh.h
	src/Mesh.cpp
	src/Model.h
	src/Model.cpp
	src/FrameBuffer.h
	src/FrameBuffer.cpp
	src/Skybox.h
	src/Skybox.cpp
)

target_link_libraries(glPBR
    SDL2 glew32 opengl32 assimp-vc141-mtd
)

add_custom_command(TARGET glPBR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/SDL2.dll"
      "$<TARGET_FILE_DIR:glPBR>"
)
add_custom_command(TARGET glPBR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/assimp-vc141-mtd.dll"
      "$<TARGET_FILE_DIR:glPBR>"
)

add_custom_command(TARGET glPBR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/bin/glew32.dll"
      "$<TARGET_FILE_DIR:glPBR>"
)


