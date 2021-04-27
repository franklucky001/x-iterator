//
// Created by frank guo on 2021/4/2.
//

#ifndef FP_ITERATOR_ITERATOR_TRAITS_HPP
#define FP_ITERATOR_ITERATOR_TRAITS_HPP
#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <string>

template <typename Container>
struct is_container : std::false_type { };

template <typename T, typename S>
struct is_container<std::pair<T, S>> : std::false_type {};

template<typename ...Args>
struct is_container<std::tuple<Args...>> : std::false_type {};

template<typename T>
struct is_container<std::vector<T>> : std::true_type {
};

template<typename T, size_t n>
struct is_container<std::array<T, n>> : std::true_type {
};

template<typename T>
struct is_container<std::basic_string<T>> : std::true_type {
};
#endif //FP_ITERATOR_ITERATOR_TRAITS_HPP
