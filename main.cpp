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

void test_flat(){
    //fixme expect a,b,c,d,e,f,l,m,n
    std::vector<std::string> vec_str = {"abc", "def", "lmn"};
    auto result = IteratorWrapper(vec_str)
            .flatten()
            .collection();
    for(auto ch: result){
        std::cout<<"flat string item char = "<< ch<<std::endl;
    }
    //fixme expect 10,20,30,40,50,60a
    std::vector<std::vector<int>> mat = {{10,20,30},{40,50,60}};
    auto li = IteratorWrapper(mat)
            .flatten()
            .collection();
    for(auto i: li){
        std::cout<<"flat matrix item = "<<i<<std::endl;
    }
}

int main() {
    test_flat();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
