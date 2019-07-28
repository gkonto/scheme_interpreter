cmake_minimum_required(VERSION 2.8.7)
PROJECT(scheme)

add_definitions("-Wall -std=c++0x")

cmake_policy(SET CMP0002 OLD)
cmake_policy(SET CMP0000 OLD)


set(HEADERS
   )

set(SOURCES 
	scheme.c
    )

ADD_EXECUTABLE(scheme_repl ${HEADERS} ${SOURCES})
