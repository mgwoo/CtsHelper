cmake_minimum_required(VERSION 3.1)

project(CtsHelper)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For Multithread
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(DEFLIB_HOME
  ${PROJECT_SOURCE_DIR}/module/def
)
set(LEFLIB_HOME
  ${PROJECT_SOURCE_DIR}/module/lef
)

add_subdirectory(${DEFLIB_HOME})
add_subdirectory(${LEFLIB_HOME})

set(ENV{TYPE_CALL} library)


## configure settings
find_package(ZLIB REQUIRED)
find_package(BISON REQUIRED)

# for cimg

set (CTSHELP_SRC
  src/main.cpp
  src/lefParser.cpp
  src/defParser.cpp
  src/lefdefIO.cpp
  )


set (CTSHELP_HEADER
  src/lefdefIO.h
)


############################################################
# include directories 
############################################################
include_directories(

  ${DEFLIB_HOME}/def
  ${DEFLIB_HOME}/defzlib
  ${DEFLIB_HOME}/cdef
  ${DEFLIB_HOME}/cdefzlib

  ${LEFLIB_HOME}/lef
  ${LEFLIB_HOME}/lefzlib
  ${LEFLIB_HOME}/clef
  ${LEFLIB_HOME}/clefzlib

  )

############################################################
# Create an executable
############################################################
add_executable( CtsHelper
  ${CTSHELP_SRC}
)

set_target_properties( CtsHelper 
  PROPERTIES OUTPUT_NAME "lefdef2cts"
)

add_dependencies( CtsHelper def )
add_dependencies( CtsHelper lef )

target_link_libraries( CtsHelper
  PUBLIC
  def
  defzlib
  cdef
  cdefzlib
  lef
  lefzlib
  clef
  clefzlib

  ${ZLIB_LIBRARIES}
  -static-libstdc++ 
  )


############################################################
# Install  
############################################################

install(FILES ${CTSHELP_HEADER} DESTINATION include)
install(TARGETS CtsHelper DESTINATION bin)


