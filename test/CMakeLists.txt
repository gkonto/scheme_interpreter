cmake_minimum_required(VERSION 2.8.7)
PROJECT(scheme_test)
add_definitions("-g -Wall -std=c++0x")
cmake_policy(SET CMP0002 OLD)
cmake_policy(SET CMP0000 OLD)


set(HEADERS
	../node.hpp
	../parser.hpp
	../symboltable.hpp
   )

set(SOURCES 
	main.cpp
	../parser.cpp
	../node.cpp
	../symboltable.cpp
    )

ADD_EXECUTABLE(scheme_test ${HEADERS} ${SOURCES})
