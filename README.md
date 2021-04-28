# fp tools map-filter-reduce-fold-scan-enumerate-zip
rust like fp iterator implement for c++

## Examples
### map-filter
```c++
    std::vector<int> vec = {1,2,3,4,5,6, 7, 9};
    auto result = IteratorWrapper(vec)
            .filter([](int v) { return v % 2 == 0; })
            .map([](int v) { return v * 10; })
            .collect();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
```

### reduce
```c++
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
        .reduce_right([](int a, int b){return a - b;});
    if(result.has_value())
        std::cout<<result.value()<<std::endl;
```

### fold
```c++
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .fold_right(0, [](int a, int b){return a - b;});
    std::cout<<result<<std::endl;
```
### scan 
```c++
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .scan_right(30, [](int _init, int v) { return _init - v; })
            .collect();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
```

### enumerate
```c++
    std::vector<int> vec = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vec)
            .enumerate()
            .collect();
    for(auto [i, v] : result){
        std::cout<<"idx = "<< i<< ", value = "<<v<<std::endl;
    }
```

### zip
```c++
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
            .collect();
#endif
    for (auto [a, b] : result){
        std::cout<<"front = "<< a<<", back "<<b<<std::endl;
    }
```

### flatten
```c++
    std::vector<std::string> vec_str = {"abc", "def", "lmn"};
    auto result = IteratorWrapper(vec_str)
            .flatten()
            .collect();
    for(auto ch: result){
        std::cout<<"flat string item char = "<< ch<<std::endl;
    }
    std::vector<std::vector<int>> mat = {{10,20,30},{40,50,60}};
    auto li = IteratorWrapper(mat)
            .flatten()
            .collect();
    for(auto i: li){
        std::cout<<"flat matrix item = "<<i<<std::endl;
    }
```

### flat_map
```c++
    std::vector<std::string> vec_str = {"abc", "0123", "DEF"};
    auto result = IteratorWrapper(vec_str)
            .flat_map([](char c) { return static_cast<int>(c); })
            .collect();
    for (auto i : result){
        std::cout<<i<<std::endl;
    }
```

### take && take_while
```c++
    std::vector<int> vs = {1, 2, 3, 4, 5, 6, 7, 8, 9};
#if 0
    auto result = IteratorWrapper(vs)
            .take(5)
            .collect();
#else
    auto result = IteratorWrapper(vs)
            .take_while([](int val){return val % 3 != 0;})
            .collect();
#endif
    for(auto & i: result){
        std::cout<<i<<std::endl;
    }
```