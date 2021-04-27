//
// Created by frank guo on 2021/4/1.
//

#ifndef FP_ITERATOR_ITERATOR_WRAPPER_HPP
#define FP_ITERATOR_ITERATOR_WRAPPER_HPP
#include <iostream>
#include <vector>
#include <optional>
#include "flat_fwd.hpp"

template<typename Container> class IteratorWrapper;
template<typename T, typename F> class MapIterator;
template<typename T, typename P> class FilterIterator;
template<typename B, typename T, typename F> class ScanIterator;
template<typename T> class Enumerate;
template<typename T, typename B> class Zip;
template<typename T, typename Container> class ZipContainer;
template<typename T> class Chain;
template<typename T, typename Container> class ChainContainer;

template<typename T>
class Iterator{
    using Item = T;
public:
    virtual ~Iterator() = default;
    virtual bool has_next() = 0;
    virtual std::optional<Item> next() = 0;
    virtual bool has_prev() = 0;
    virtual std::optional<Item> prev() = 0;
    Enumerate<Item> enumerate(){
        return Enumerate<Item>(*this);
    }

    template<class B>
    Zip<Item, B> zip(Iterator<B> & iter){
        return Zip<Item, B>(*this, iter);
    }

    template<class Container>
    ZipContainer<Item, Container> zip(Container & container){
        return ZipContainer<Item, Container>(*this, container);
    }
    template<typename B>
    Chain<Item> chain(Iterator<B> & iter){
        static_assert(std::is_same<Item, B>::value, "chain only support same value type");
        return Chain<Item>(*this, iter);
    }
    template<typename Container>
    ChainContainer<Item, Container> chain(Container & container){
        static_assert(std::is_same<Item, typename Container::value_type>::value, "chain only support same value type");
        return ChainContainer<Item, Container>(*this, container);
    }
    template<typename F>
    auto map(F && transform)->MapIterator<typename std::result_of<F(Item)>::type, decltype(transform)>{
        // TODO fix return type as template parameter
        using Return = typename std::result_of<F(Item)>::type;
        return MapIterator<Return , decltype(transform)>(*this, std::forward<decltype(transform)>(transform));
    }

    Flatten<Item> flatten(){
        return Flatten<Item>(*this);
    }

    template<typename F>
    auto flat_map(F && transform)->FlatMapIterator<Item, decltype(transform)>{
        return FlatMapIterator<Item, decltype(transform)>(*this, std::forward<decltype(transform)>(transform));
    }
    template<typename P>
    auto filter(P && predicate) ->FilterIterator<T, decltype(predicate)>{
        return FilterIterator<T, decltype(predicate)>(*this, std::forward<decltype(predicate)>(predicate));
    }

    template<class B, class F>
    B fold(B && _init, F && op){
        while (has_next()){
            std::optional<Item> opt = next();
            if(opt.has_value())
                _init = op(_init, opt.value());
        }
        return _init;
    }

    template<class B, class F>
    B fold_left(B && _init, F && op){
        return fold(std::forward<B>(_init), std::forward<F>(op));
    }
    template<class B, class F>
    B fold_right(B && _init, F && op){
        while (has_prev()){
            std::optional<Item> opt = prev();
            if(opt.has_value())
                _init = op(_init, opt.value());
        }
       return _init;
    }
    template<class F>
    auto reduce(F && op) ->std::optional<typename std::result_of<F(T, T)>::type>{
        if(!has_next())
            return std::nullopt;
        std::optional<Item> first = next();
        if(!first.has_value())
            return std::nullopt;
        Item _init = first.value();
        return fold(_init, std::forward<F>(op));
    }

    template<class F>
    auto reduce_left(F && op) ->std::optional<typename std::result_of<F(T, T)>::type>{
        return reduce(std::forward<F>(op));
    }

    template<class F>
    auto reduce_right(F && op) ->std::optional<typename std::result_of<F(T, T)>::type>{
        if(!has_prev())
            return std::nullopt;
        std::optional<Item> first = prev();
        if(!first.has_value())
            return std::nullopt;
        Item _init = first.value();
        return fold_right(_init, std::forward<F>(op));
    }
    template<class B, class F>
    auto scan(B && _init, F && accum) ->ScanIterator<B, Item, decltype(accum)>{
        return ScanIterator<B, Item, decltype(accum)>(std::forward<B>(_init), *this, std::forward<decltype(accum)>(accum));
    }

    template<class B, class F>
    auto scan_left(B && _init, F && accum) ->ScanIterator<B, Item, decltype(accum)>{
        return ScanIterator<B, Item, decltype(accum)>(std::forward<B>(_init), *this, std::forward<decltype(accum)>(accum));
    }
    template<class B, class F>
    auto scan_right(B && _init, F && accum) ->ScanIterator<B, Item, decltype(accum)>{
        return ScanIterator<B, Item, decltype(accum)>(std::forward<B>(_init), *this, std::forward<decltype(accum)>(accum), true);
    }

    std::vector<Item> collect(){
        std::vector<Item> result;
        while (has_next()){
            auto opt = this->next();
            if(opt.has_value())
                result.push_back(opt.value());
        }
        return result;
    };

    template<class F>
    void for_each(F && f){
        while (has_next()){
            auto opt = this->next();
            if(opt.has_value())
                f(opt.value());
        }
    }
    template<class P>
    bool any(P && predicate){
        bool flag = false;
        while (has_next()){
            auto opt = this->next();
            if(opt.has_value() && predicate(opt.value())){
                flag = true;
                break;
            }
        }
        return flag;
    }
    template<class P>
    bool all(P && predicate){
        bool flag = true;
        while (has_next()){
            auto opt = this->next();
            if(opt.has_value() && !predicate(opt.value())){
                flag = false;
                break;
            }
        }
        return flag;
    }
};

template<typename T, typename F>
class MapIterator : public Iterator<T>{

public:
    using Item = T;
    using Self = MapIterator<T, F>;
    explicit MapIterator(Iterator<T> & iter, F  && f):_iter(iter), _transform(std::forward<F>(f)){}
    MapIterator(const Self & other):_iter(other._iter), _transform(other._transform){}
    MapIterator(Self && other) noexcept :_iter(other._iter), _transform(other._transform){}
    bool has_next(){
        return this->_iter.has_next();
    }
    bool has_prev(){
        return this->_iter.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto opt = this->_iter.next();
        // last filter no match
        if(opt.has_value())
            return this->_transform(opt.value());
        else
            return std::nullopt;
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto opt = this->_iter.prev();
        // last filter no match
        if(opt.has_value())
            return this->_transform(opt.value());
        else
            return std::nullopt;
    }
private:
    Iterator<T> & _iter;
    F _transform;
};
template<typename B, typename T, typename F>
class ScanIterator : public Iterator<B>{
public:
    using Item = B;
    using Self = ScanIterator<B, T, F>;
    explicit ScanIterator(B && init, Iterator<T>& iter, F && f, bool is_reverse=false)
        :_init(init),_iter(iter),_accumulation(std::forward<F>(f)), _is_reverse(is_reverse){
    }
    ScanIterator(const Self & other)
        :_init(other._init),_iter(other._iter),_accumulation(other._accumulation),_is_reverse(other.is_reverse){}
    ScanIterator(Self && other)noexcept
        :_init(other._init),_iter(other._iter),_accumulation(other._accumulation),_is_reverse(other.is_reverse){}
    bool has_next(){
        if(_is_reverse)
            return has_prev();
        return this->_iter.has_next();
    }
    bool has_prev(){
        return this->_iter.has_prev();
    }
    std::optional<B> next(){
        if(_is_reverse)
            return prev();
        if(!has_next())
            return std::nullopt;
        auto opt = this->_iter.next();
        // last filter no match
        if(opt.has_value())
            _init = this->_accumulation(_init, opt.value());
        return _init;
    }
    std::optional<B> prev(){
        if(!has_prev())
            return std::nullopt;
        auto opt = this->_iter.prev();
        // last filter no match
        if(opt.has_value())
            _init = this->_accumulation(_init, opt.value());
        return _init;
    }
private:
    Item _init;
    bool _is_reverse;
    Iterator<T> & _iter;
    F _accumulation;
};
template<typename T, typename P>
class FilterIterator : public Iterator<T>{
public:
    using Item = T;
    using Self = FilterIterator<T, P>;
    explicit FilterIterator(Iterator<T>& iter, P && predicate):_iter(iter), _predicate(std::forward<P>(predicate)){}
    FilterIterator(const Self & other):_iter(other._iter), _predicate(other._predicate){}
    FilterIterator(Self && other) noexcept :_iter(other._iter), _predicate(other._predicate){}
    bool has_next(){
        return this->_iter.has_next();
    }
    bool has_prev(){
        return this->_iter.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto opt = this->_iter.next();
        while (has_next() && !_predicate(opt.value()))
            opt = this->_iter.next();
        if(opt.has_value() && _predicate(opt.value()))
            return opt.value();
        else
            return std::nullopt;
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto opt = this->_iter.prev();
        while (has_prev() && !_predicate(opt.value()))
            opt = this->_iter.prev();
        if(opt.has_value() && _predicate(opt.value()))
            return opt.value();
        else
            return std::nullopt;
    }
private:
    Iterator<T>& _iter;
    P _predicate;
};

template<typename  T>
class Enumerate : public Iterator<std::pair<size_t, T>>{
public:
    using Self = Enumerate<T>;
    using Item = std::pair<size_t, T>;
    explicit Enumerate(Iterator<T> & iter):_iter(iter), _count(0){};
    Enumerate(const Self & other):_iter(other._iter), _count(0){};
    Enumerate(Self && other) noexcept :_iter(other._iter), _count(0){};
    bool has_next(){
        return this->_iter.has_next();
    }
    bool has_prev(){
        return this->_iter.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        // last filter no match
        auto opt = this->_iter.next();
        if(opt.has_value())
            return std::make_pair<size_t, T>(_count++, std::forward<T>(opt.value()));
        else
            return std::nullopt;
    }
    std::optional<Item> prev(){
        if(!has_next())
            return std::nullopt;
        // last filter no match
        auto opt = this->_iter.prev();
        if(opt.has_value())
            return std::make_pair<size_t, T>(_count++, std::forward<T>(opt.value()));
        else
            return std::nullopt;
    }
private:
    Iterator<T> & _iter;
    size_t _count;
};
template<typename T, typename B>
class Zip : public Iterator<std::pair<T, B>>{
public:
    using Self = Zip<T, B>;
    using Item = std::pair<T,B>;
    explicit Zip(Iterator<T> & iter, Iterator<B> &  back):_iter(iter), _back(back){}
    Zip(const Self & other): _iter(other._iter), _back(other._back){}
    Zip(Self && other) noexcept : _iter(other._iter), _back(other._back){}
    bool has_next(){
        return _iter.has_next() && _back.has_next();
    }
    bool has_prev(){
        return _iter.has_prev() && _back.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto front = _iter.next();
        auto back = _back.next();
        // last filter no match
        if(front.has_value() && back.has_value())
            return std::make_pair<T, B>(std::forward<T>(front.value()), std::forward<B>(back.value()));
        else
            return std::nullopt;
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto front = _iter.prev();
        auto back = _back.prev();
        // last filter no match
        if(front.has_value() && back.has_value())
            return std::make_pair<T, B>(std::forward<T>(front.value()), std::forward<B>(back.value()));
        else
            return std::nullopt;
    }
private:
    Iterator<T> & _iter;
    Iterator<B> & _back;
};

template<typename T, typename Container>
class ZipContainer : public Iterator<std::pair<T, typename Container::value_type>>{
public:
    using Self = ZipContainer<T, Container>;
    using B = typename Container::value_type;
    using Item = std::pair<T, typename Container::value_type>;
    explicit ZipContainer(Iterator<T> & iter, Container & container):_iter(iter), _container(container),_offset(0){}
    ZipContainer(const Self & other):_iter(other._iter), _container(other._container), _offset(other._offset){}
    ZipContainer(Self && other) noexcept :_iter(other._iter), _container(other._container), _offset(other._offset){}
    bool has_next() {
        auto back_beg = std::begin(_container);
        auto back_end = std::end(_container);
        return _iter.has_next() && (back_beg + _offset != back_end);
    }
    bool has_prev(){
        auto back_beg = std::rbegin(_container);
        auto back_end = std::rend(_container);
        return _iter.has_prev() && (back_beg + _offset != back_end);
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto front = _iter.next();
        // last filter no match
        if(front.has_value()) {
            auto back_beg = std::begin(_container);
            auto back_value = *(back_beg + _offset++);
            return std::make_pair<T, B>(std::forward<T>(front.value()), std::forward<B>(back_value));
        }
        else
            return std::nullopt;
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto front = _iter.prev();
        // last filter no match
        if(front.has_value()) {
            auto back_beg = std::rbegin(_container);
            auto back_value = *(back_beg + _offset++);
            return std::make_pair<T, B>(std::forward<T>(front.value()), std::forward<B>(back_value));
        }
        else
            return std::nullopt;
    }
private:
    Iterator<T> & _iter;
    Container & _container;
    size_t _offset;
};

template<typename T>
class Chain : public Iterator<T>{
public:
    using Self = Chain<T>;
    using Item = T;
    explicit Chain(Iterator<T> & iter, Iterator<T> &  back):_iter(iter), _back(back){}
    Chain(const Self & other): _iter(other._iter), _back(other._back){}
    Chain(Self && other) noexcept : _iter(other._iter), _back(other._back){}
    bool has_next(){
        return _iter.has_next() || _back.has_next();
    }
    bool has_prev(){
        return _iter.has_prev() || _back.has_prev();
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto front = _iter.next();
        // last filter no match
        if(front.has_value())
            return front.value();
        else{
            auto back = _back.next();
            if(back.has_value())
                return back.value();
            else
                return std::nullopt;
        }
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto front = _back.prev();
        // last filter no match
        if(front.has_value())
            return front.value();
        else{
            auto back = _iter.prev();
            if(back.has_value())
                return back.value();
            else
                return std::nullopt;
        }
    }
private:
    Iterator<T> & _iter;
    Iterator<T> & _back;
};
template<typename T,typename Container>
class ChainContainer : public Iterator<T>{
public:
    using Self = ChainContainer<T, Container>;
    using Item = T;
    explicit ChainContainer(Iterator<T> & iter, Container & container):_iter(iter), _container(container),_offset(0){}
    ChainContainer(const Self & other):_iter(other._iter), _container(other._container), _offset(other._offset){}
    ChainContainer(Self && other) noexcept :_iter(other._iter), _container(other._container), _offset(other._offset){}
    bool has_next() {
//        auto back_beg = std::begin(_container);
//        auto back_end = std::end(_container);
        return _iter.has_next() || (std::begin(_container) + _offset != std::end(_container));
    }
    bool has_prev(){
//        auto back_beg = std::rbegin(_container);
//        auto back_end = std::rend(_container);
        return _iter.has_prev() || (std::rbegin(_container) + _offset != std::rend(_container));
    }
    std::optional<Item> next(){
        if(!has_next())
            return std::nullopt;
        auto front = _iter.next();
        // last filter no match
        if(front.has_value())
            return front.value();
        else{
            auto back_beg = std::begin(_container);
            auto back_value = *(back_beg + _offset++);
            return back_value;
        }
    }
    std::optional<Item> prev(){
        if(!has_prev())
            return std::nullopt;
        auto front_cur = std::rbegin(_container) + _offset++;
        // last filter no match
        if(front_cur != std::rend(_container))
            return *front_cur;
        else{
            auto back = _iter.next();
            return back.value();
        }
    }
private:
    Iterator<T> & _iter;
    Container & _container;
    size_t _offset;
};
/****
 * indexed container vector, array
 * ***/
template <typename Container>
class IteratorWrapper : public Iterator<typename Container::value_type>{
public:
    using Self = IteratorWrapper<Container>;
    using std_iterator = typename Container::iterator;
    using std_reverse_iterator = typename std::reverse_iterator<std_iterator>;
    using Item = typename Container::value_type;

    explicit IteratorWrapper(Container & container) noexcept{
        this->_first = std::begin(container);
        this->_last = std::end(container);
        this->_r_begin = std::rbegin(container);
        this->_r_end = std::rend(container);
    }
    IteratorWrapper(const Self& other) noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_r_begin = other._r_begin;
        this->_r_end = other._r_end;
    }
    IteratorWrapper(Self && other)noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_r_begin = other._r_begin;
        this->_r_end = other._r_end;
    }
    bool has_next(){
        return _first != _last;
    }
    bool has_prev(){
        return _r_begin != _r_end;
    }
    std::optional<Item> next(){
        if(has_next())
            return *(_first++);
        return std::nullopt;
    }
    std::optional<Item> prev(){
        if(has_prev())
            return *(_r_begin++);
        return std::nullopt;
    }
private:
    std_iterator _first;
    std_iterator _last;
    std_reverse_iterator _r_begin;
    std_reverse_iterator _r_end;
};
#endif //FP_ITERATOR_ITERATOR_WRAPPER_HPP
