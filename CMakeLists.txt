cmake_minimum_required(VERSION 3.17)
project(fp_iterator)

set(CMAKE_CXX_STANDARD 17)

add_executable(fp_iterator main.cpp collection/iterator_wrapper.hpp collection/iterator_traits.hpp collection/flat_fwd.hpp)
