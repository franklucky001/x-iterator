#include <iostream>
//#include <vector>
#include <list>
#include <queue>
#include <array>
#include "collection/iterator_wrapper.hpp"

void test_map_filter(){
    std::vector<int> vec = {1,2,3,4,5,6, 7, 9};
    auto result = IteratorWrapper(vec)
            .filter([](int  v){return v % 2 == 0;})
            .map([](int v){return v * 10;})
            .collection();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
}
void test_reduce(){
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .reduce_right([](int a, int b){return a - b;});
    if(result.has_value())
        std::cout<<result.value()<<std::endl;
}
void test_fold(){
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .fold_right(0, [](int a, int b){return a - b;});
    std::cout<<result<<std::endl;
}
void test_enumerate(){
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .enumerate()
            .collection();
    for(auto [i, v] : result){
        std::cout<<"idx = "<< i<< ", value = "<<v<<std::endl;
    }
}
void test_scan(){
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .scan_right(30, [](int _init, int v){return _init - v;})
            .collection();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
}
void test_zip(){
    std::vector<int> front = {1,2,3,4,5,6};
    std::vector<char> back = {'a', 'b', 'c', 'd'};
#if 0
    auto back_iter = IteratorWrapper(back);
    auto result = IteratorWrapper(front)
            .zip(back_iter)
            .collection();
#else
    auto result = IteratorWrapper(front)
            .zip(back)
            .collection();
#endif
    for (auto [a, b] : result){
        std::cout<<"front = "<< a<<", back "<<b<<std::endl;
    }
}
template<typename T, typename Enable> struct check;
void foo(){
    check<std::string> c = check<std::string>();
}
template<typename T, typename Enable= void >
struct check;

template<typename T>
struct check<T, typename std::enable_if_t<is_container<T>::value>>{
    using D = typename T::value_type;
};
int main() {
//    test_scan();
    check<std::string> c = check<std::string>();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
