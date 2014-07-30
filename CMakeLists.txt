cmake_minimum_required(VERSION 2.4.4)

project(opengv)

set(OPENGV_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_VERBOSE_MAKEFILE on)

set(OPENGV_LIBDEPS
  z
  glew32
  glfw3
  png
  opengl32
  expat
)

if(MINGW)
    set(OS_NAME "mingw")
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -O0 -g -Wall")
elseif(WIN32)
    set(OS_NAME "win32")
	ADD_DEFINITIONS(
		-D_WINDOWS 
		-DWIN32
		-DUNICODE -D_UNICODE
		-D_CRT_SECURE_NO_WARNINGS
	)
    set(OPENGV_LIBDEPS
        zlib
        glew32
        glfw3
        libpng16
        opengl32
        libexpat
    )
endif()

add_definitions(-DGLEW_STATIC)
#add_definitions(-m32)

set(OPENGV_LIBADD
    ${OPENGV_LIBDEPS}
    opengv
)

set(OPENGV_LIBDIR ${OPENGV_DIR}/lib/${OS_NAME})
set(OPENGV_BUILDDIR ${OPENGV_DIR}/build/${OS_NAME})

set(LIBRARY_OUTPUT_PATH ${OPENGV_LIBDIR})
set(EXECUTABLE_OUTPUT_PATH ${OPENGV_DIR}/bin)

include_directories(
	${OPENGV_DIR}/core
	${OPENGV_DIR}/deps/include
	${OPENGV_DIR}/deps/include/${OS_NAME}
)
link_directories(
	${OPENGV_LIBDIR}
	${OPENGV_DIR}/deps/lib/${OS_NAME}
)

add_subdirectory(core)
#add_subdirectory(test)



