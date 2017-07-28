#pragma once

#include "lambda.h"

namespace functional
{

template<typename Fn, typename T>
void scan(Fn fn, const std::vector<T>& vec)
{
    const auto size = vec.size();
    for (size_t i = 0; i < size; i++)
        fn(vec[i]);
}

template<typename Fn, typename T>
auto map(Fn fn, const std::vector<T>& vec)
{
    const auto size = vec.size();
    std::vector<T> ret(size);
    for (size_t i = 0; i < size; i++)
        ret[i] = fn(vec[i]);
    return ret;
}

template<typename Fn, typename T>
auto map(Fn fn, std::vector<T>&& vec)
{
    const auto size = vec.size();
    for (size_t i = 0; i < size; i++)
        vec[i] = fn(vec[i]);
    return std::move(vec);
}

template<typename Fn, typename T>
auto nest(Fn fn, T&& val, size_t n)
{
    using Ret = decltype(fn(std::forward<decltype(val)>(val)));
    if (n == 0)
        return Ret(fn(std::forward<decltype(val)>(val)));
    else
    {
        Ret ret = fn(val);
        for (size_t i = 1; i < n; i++)
            ret = fn(ret);
        return ret;
    }
}

template<typename Fn, typename T>
auto nest_list(Fn fn, T&& val, size_t n)
{
    using Ret = decltype(fn(std::forward<decltype(val)>(val)));
    std::vector<Ret> ret(n + 1);
    ret[0] = Ret(std::forward<decltype(val)>(val));
    for (int i = 0; i < n; i++)
        ret[i + 1] = fn(ret[i]);
    return ret;
}

template<typename Fn, typename T>
auto fold(Fn fn, T val, const std::vector<T>& vec)
{
    const auto size = vec.size();
    for (size_t i = 0; i < size; i++)
        val = fn(val, vec[i]);
    return val;
}

}
