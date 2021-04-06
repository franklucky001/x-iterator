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
    explicit Flatten(Iterator<Ts> & iter):_iter(iter), _inner_ptr(nullptr){}
    Flatten(const Self & other):_iter(other._iter),_inner_ptr(other._inner_ptr){}
    Flatten(Self && other) noexcept :_iter(other._iter),_inner_ptr(other._inner_ptr){
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
            _inner_ptr = new IteratorWrapper(opt.value());
            return next();
        }else{
            if(!_inner_ptr->has_next()){
                delete _inner_ptr;
                _inner_ptr = nullptr;
                if(!_iter.has_next())
                    return std::nullopt;
                else{
                    auto opt = _iter.next();
                    //last filter not match
                    if(opt.has_value()) {
                        _inner_ptr = new IteratorWrapper(opt.value());
                        return next();
                    }else{
                        return std::nullopt;
                    }

                }
            }else{
                return _inner_ptr->next();
            }
        }
    }
    std::optional<Item> prev(){
        if(_inner_ptr == nullptr){
            if(!_iter.has_prev())
                return std::nullopt;
            auto opt = _iter.prev();
            _inner_ptr = new IteratorWrapper(opt.value());
            return prev();
        }else{
            if(!_inner_ptr->has_prev()){
                delete _inner_ptr;
                _inner_ptr = nullptr;
                if(!_iter.has_prev())
                    return std::nullopt;
                else{
                    auto opt = _iter.prev();
                    _inner_ptr = new IteratorWrapper(opt.value());
                    return prev();
                }
            }else{
                return _inner_ptr->prev();
            }
        }
    }
private:
    Iterator<Ts> & _iter;
    IteratorWrapper<Ts> * _inner_ptr;
};
template<typename T, typename R, typename F, typename Enable=void>
struct FlatMapIterator{
    static_assert(is_container<T>::value);
};
template<typename Ts, typename R, typename F>
struct FlatMapIterator<Ts, R, F, typename std::enable_if_t<is_container<Ts>::value>>
        : public Iterator<R>{
public:
    using Self = FlatMapIterator<Ts, R, F, typename std::enable_if_t<is_container<Ts>::value>>;
    using Item = R;
    explicit FlatMapIterator(Iterator<Ts> & iter, F && f):_iter(iter), _transform(std::forward<F>(f)){}
    FlatMapIterator(const Self & other):_iter(other._iter), _transform(other._transform){}
    FlatMapIterator(Self && other) noexcept :_iter(other._iter), _transform(other._transform){}
    bool has_next(){
        return _iter.has_next();
    }
    bool has_prev(){
        return _iter.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto opt = _iter.next();
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
        auto opt = _iter.prev();
        if(!opt.has_value())
            return std::nullopt;
        else
            return _transform(opt.value());
    }
private:
    Flatten<Ts> & _iter;
    F _transform;
};
#endif //FP_ITERATOR_FLAT_FWD_HPP
