
# CMakeLists.txt


cmake_minimum_required(VERSION 2.8)
project(seshat) 

string(TIMESTAMP YEAR "%Y")

set(PACKAGE "Q")
set(PACKAGE_NAME "Qabalah")
set(PACKAGE_TITLE "${PACKAGE_NAME} Language Interpreter")
set(PACKAGE_VERSION_MAJOR 0)
set(PACKAGE_VERSION_MINOR 1)
set(PACKAGE_VERSION_BUILD 0)
set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_BUILD}")
set(PACKAGE_STRING "${PACKAGE_TITLE} ${PACKAGE_VERSION}")

set(PACKAGE_MAINTAINER "Per Löwgren")
set(PACKAGE_BUGREPORT "per.lowgren@gmail.com")
set(PACKAGE_URL "https://bitbucket.org/perlowgren/qabalah-interpreter")

set(PACKAGE_TARNAME "${PACKAGE}")
set(PACKAGE_YEAR ${YEAR})

if(CMAKE_C_COMPILER_ID STREQUAL GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=gnu99")
	if(WIN32)
		set(CMAKE_C_FLAGS_RELEASE "-O2")
		set(CMAKE_C_FLAGS_DEBUG  "-Werror -O3 -gdwarf-2 -g")
	else()
		set(CMAKE_C_FLAGS_RELEASE "-O2")
		set(CMAKE_C_FLAGS_DEBUG  "-Werror -O3 -g")
	endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCLI -DREADLINE")

configure_file(
	"${PROJECT_SOURCE_DIR}/src/_config.h"
	"${PROJECT_BINARY_DIR}/src/config.h"
)

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_BINARY_DIR}/src"
)

set(q_src
	src/q.c
	src/var.c
	src/str.c
)

add_executable(q ${q_src})
target_link_libraries(q m readline)

install(
	TARGETS q
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

