cmake_minimum_required(VERSION 3.2)
message("[INFO] Using env variable `TRAVIS_BUILD_NUMBER` = $ENV{TRAVIS_BUILD_NUMBER}")
message("[INFO] Using env variable `BOOST_ROOT` = $ENV{BOOST_ROOT}")
message("[INFO] Using env variable `BOOST_INCLUDEDIR` = $ENV{BOOST_INCLUDEDIR}")
message("[INFO] Using env variable `BOOST_LIBRARYDIR` = $ENV{BOOST_LIBRARYDIR}")
project(lisp_engine VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(SOURCES
	src/main.cpp
	)

file(GLOB_RECURSE LISP_ENGINE_HEADERS
    "src/*.h"
)

set(TEST_SOURCES
	tests/tests.cpp
	)

set(TESTS_HEADERS

	)

set(DATA_FILES
	../Readme.md

	)

option(LOGS "Enable output logs" false)
configure_file(src/configure.h.in ../build/configure.h)

add_executable(lisp_engine ${SOURCES} ${DATA_FILES} ${LISP_ENGINE_HEADERS})
#add_executable(lisp_engine-orig stub.cpp)
add_executable(tests ${TEST_SOURCES} ${TESTS_HEADERS} ${LISP_ENGINE_HEADERS})
#add_executable(tests-orig stub.cpp)
set_target_properties(lisp_engine tests PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	)

find_package(Boost 1.72 COMPONENTS unit_test_framework filesystem program_options REQUIRED)

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
message("[INFO] Using MSVC")
target_link_libraries(lisp_engine
	${Boost_FILESYSTEM_LIBRARY}
)
ELSE()
message("[INFO] Using not MSVC")
target_link_libraries(lisp_engine
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)
ENDIF()

#target_link_libraries(lisp_engine
# Disable all boost libraries link (will be concrete linking)
	#${Boost_LIBRARIES}
#	${Boost_FILESYSTEM_LIBRARY}
# options_library don't work as dynamic library
	#${Boost_PROGRAM_OPTIONS_LIBRARY}
#)

target_link_libraries(tests
	${Boost_LIBRARIES}
)

message("[INFO] Boost libs: ${Boost_LIBRARIES}")
message("[INFO] Boost inc: ${Boost_INCLUDE_DIR}")
message("[INFO] Boost inc dirs: ${Boost_INCLUDE_DIRS}")
message("[INFO] Boost lib dirs: $ENV{BOOST_LIBRARYDIR}")

IF(MSVC)
set_target_properties(lisp_engine tests PROPERTIES
	COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
	INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
	LINK_DIRECTORIES ${Boost_LIBRARY_DIRS}
)
ELSE()
set_target_properties(lisp_engine tests PROPERTIES
	COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
	INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
	# TODO Not setting by cmake (maybe old cmake) - analyze it
	#LINK_DIRECTORIES ${BOOST_LIBRARYDIR}
	LINK_DIRECTORIES $ENV{BOOST_LIBRARYDIR}
)
ENDIF()

IF(WIN32)
set_target_properties(lisp_engine tests PROPERTIES
  #COMPILE_OPTIONS "-Wall" # A lot of messages (in std libraries)
  COMPILE_OPTIONS ""
)
ELSE()
set_target_properties(lisp_engine tests PROPERTIES
	#COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-Wno-c++11-extensions"
	COMPILE_OPTIONS ""
)
ENDIF()

### Tests ###
enable_testing()
add_test(lisp_engine_tests tests)
###

### Packaging ###
install(TARGETS lisp_engine RUNTIME DESTINATION bin)
#install(TARGETS lisp_engine-orig RUNTIME DESTINATION bin)
#install(FILES build/lisp_engine-config.conf DESTINATION bin)

install(DIRECTORY build/boost_factory/usr/lib/boost_1_72_0 DESTINATION bin/boost_factory/usr/lib/)

install(DIRECTORY tests/fixtures DESTINATION lib/lisp_engine/tests)
install(TARGETS tests RUNTIME DESTINATION lib/lisp_engine/tests)
#install(TARGETS tests-orig RUNTIME DESTINATION lib/lisp_engine/tests)
#install(FILES build/tests-config.conf DESTINATION lib/lisp_engine/tests)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT linkfly@yandex.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
