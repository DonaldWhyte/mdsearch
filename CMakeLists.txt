#------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8)

# Define project name and version
project (MultidimensionalSearch)
set (MDSEARCH_MAJOR_VERSION 0)
set (MDSEARCH_MINOR_VERSION 1)

# Give names for directories in the project
set (PROJECT_ROOT ${PROJECT_SOURCE_DIR})
set (BUILD_DIR ${PROJECT_ROOT}/build)

# Set directory all build files go to
set_property (GLOBAL PROPERTY RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set (EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})
set (LIBRARY_OUTPUT_PATH ${BUILD_DIR})

# Search for Boost library
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.41.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib/)

# Set compiler flags
set( EXTRA_CXX_COMPILER_FLAGS "-O3" )

#------------------------------------------------------------------------------

# Ensure include directory is in search path
include_directories ("include")

# Executable for testing implemented structures
add_executable(mdsearch_tests
	# Source files
	"src/test_structures.cpp"
)
target_link_libraries ( mdsearch_tests rt )

#------------------------------------------------------------------------------