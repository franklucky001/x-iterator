//
// Created by frank guo on 2021/4/6.
//

#ifndef FP_ITERATOR_FLAT_FWD_HPP
#define FP_ITERATOR_FLAT_FWD_HPP
#include "iterator_wrapper.hpp"
#include "iterator_traits.hpp"

template<typename T> class  Iterator;
template <typename Container> class IteratorWrapper;

template<typename T, typename Enable=void>
struct Flatten{
    static_assert(is_container<T>::value, "template T of flatten Iterator must be a container, vector|list|array|string");
};
template<typename Ts>
struct Flatten<Ts, typename std::enable_if_t<is_container<Ts>::value>>
        : public Iterator<typename Ts::value_type>{
public:
    using Self = Flatten<Ts, typename std::enable_if_t<is_container<Ts>::value>>;
    using Item = typename Ts::value_type;
    explicit Flatten(Iterator<Ts> & iter):_iter(iter), _inner_ptr(nullptr), _inner_idx(0){}
    Flatten(const Self & other):_iter(other._iter),_inner_ptr(other._inner_ptr),_inner_idx(other._inner_idx){}
    Flatten(Self && other) noexcept :_iter(other._iter),_inner_ptr(other._inner_ptr),_inner_idx(other._inner_idx){
        other._inner_ptr = nullptr;
    }
    ~Flatten(){
        if(_inner_ptr != nullptr){
            delete _inner_ptr;
            _inner_ptr = nullptr;
        }
    };
    bool has_next(){
        if(_inner_ptr != nullptr)
            return true;
        return _iter.has_next();
    }
    bool has_prev(){
        if(_inner_ptr != nullptr)
            return true;
        return _iter.has_prev();
    }
    std::optional<Item> next(){
        if(_inner_ptr == nullptr){
            if(!_iter.has_next())
                return std::nullopt;
            auto opt = _iter.next();
            //local variable
            _inner_ptr = new Ts(opt.value());
            return (*_inner_ptr)[_inner_idx++];
        }else{
            if(_inner_idx == _inner_ptr->size()){
                delete _inner_ptr;
                _inner_ptr = nullptr;
                _inner_idx = 0;
                if(!_iter.has_next())
                    return std::nullopt;
                else{
                    auto opt = _iter.next();
                    //last filter not match
                    if(opt.has_value()) {
                        _inner_ptr = new Ts(opt.value());
                        return (*_inner_ptr)[_inner_idx++];
                    }else{
                        return std::nullopt;
                    }

                }
            }else{
                return (*_inner_ptr)[_inner_idx++];;
            }
        }
    }
    std::optional<Item> prev(){
        if(_inner_ptr == nullptr){
            if(!_iter.has_prev())
                return std::nullopt;
            auto opt = _iter.prev();
            _inner_ptr = new Ts(opt.value());
            return prev();
        }else{
            if(_inner_idx == _inner_ptr->size()){
                delete _inner_ptr;
                _inner_ptr = nullptr;
                _inner_idx = 0;
                if(!_iter.has_prev())
                    return std::nullopt;
                else{
                    auto opt = _iter.prev();
                    //last filter not match
                    if(opt.has_value()) {
                        _inner_ptr = new Ts(opt.value());
                        return prev();
                    }else{
                        return std::nullopt;
                    }
                }
            }else{
                return (*_inner_ptr)[_inner_idx++];
            }
        }
    }
private:
    Iterator<Ts> & _iter;
    Ts * _inner_ptr;
    size_t _inner_idx;
};
template<typename T, typename F, typename Enable=void>
struct FlatMapIterator{
    static_assert(is_container<T>::value, "template T of flat_map Iterator must be a container, vector|list|array|string");
};
template<typename Ts,  typename F>
struct FlatMapIterator<Ts, F, typename std::enable_if_t<is_container<Ts>::value>>
: public Iterator<typename std::result_of<F(typename Ts::value_type)>::type>{
public:
    using Self = FlatMapIterator<Ts, F, typename std::enable_if_t<is_container<Ts>::value>>;
    using Item = typename std::result_of<F(typename Ts::value_type)>::type;
    explicit FlatMapIterator(Iterator<Ts> & iter, F && f):_iter(iter),_flatten_ptr(nullptr), _transform(std::forward<F>(f)){}
    FlatMapIterator(const Self & other):_iter(other._iter),_flatten_ptr(other._flatten_ptr), _transform(other._transform){}
    FlatMapIterator(Self && other) noexcept :_iter(other._iter),_flatten_ptr(other._flatten_ptr), _transform(other._transform){}
    virtual ~FlatMapIterator(){
        if(_flatten_ptr){
            delete _flatten_ptr;
            _flatten_ptr = nullptr;
        }
    }
    bool has_next(){
        return flatten_iter().has_next();
    }
    bool has_prev(){
        return flatten_iter().has_prev();
    }
    Flatten<Ts> & flatten_iter(){
        if(nullptr == _flatten_ptr){
            _flatten_ptr = new Flatten<Ts>(_iter);
        }
        return *_flatten_ptr;
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto opt = flatten_iter().next();
        //last filter no match
        //TODO is require ?
        if(!opt.has_value())
            return std::nullopt;
        else
            return _transform(opt.value());

    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto opt = flatten_iter().prev();
        if(!opt.has_value())
            return std::nullopt;
        else
            return _transform(opt.value());
    }
private:
    Iterator<Ts> & _iter;
    Flatten<Ts> * _flatten_ptr;
    F _transform;
};
#endif //FP_ITERATOR_FLAT_FWD_HPP
