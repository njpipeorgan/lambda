#pragma once
#ifndef LAMBDA_H
#define LAMBDA_H

#include <tuple>

namespace lambda
{
#pragma push_macro("FWD")
#define FWD(x) std::forward<decltype(x)>(x)

#define DEFINE_UNARY_OPERATOR(name, expr)                                   \
struct name                                                                 \
{                                                                           \
    template<typename X>                                                    \
    inline auto operator()(X&& x) const -> decltype(expr)                   \
    { return (expr); }                                                      \
};
#define DEFINE_BINARY_OPERATOR(name, expr)                                  \
struct name                                                                 \
{                                                                           \
    template<typename X, typename Y>                                        \
    inline auto operator()(X&& x, Y&& y) const -> decltype(expr)            \
    { return (expr); }                                                      \
};

#define CONSTRUCT_UNARY_EXPR(oper, name)                                    \
template<typename X, typename = std::enable_if_t<_any_expr<X>::value>>      \
inline auto operator oper(X&& x)                                            \
{ return _make_oper_expr(op::name{}, FWD(x)); }
#define CONSTRUCT_BINARY_EXPR(oper, name)                                   \
template<typename X, typename Y,                                            \
         typename = std::enable_if_t<_any_expr<X, Y>::value>>               \
inline auto operator oper(X&& x, Y&& y)                                     \
{ return _make_oper_expr(op::name{}, FWD(x), FWD(y)); }

#define CONSTRUCT_MEMBER_UNARY_EXPR(oper, name, args)                       \
inline auto operator oper(args) const                                       \
{ return _make_oper_expr(op::name{}, FWD(*this)); }
#define CONSTRUCT_MEMBER_BINARY_EXPR(oper, name)                            \
template<typename Y>                                                        \
inline auto operator oper(Y&& y) const                                      \
{ return _make_oper_expr(op::name{}, FWD(*this), FWD(y)); }


// operators
namespace op
{

// non-member operators
DEFINE_UNARY_OPERATOR (minus,      -FWD(x))
DEFINE_UNARY_OPERATOR (address,    &FWD(x))
DEFINE_UNARY_OPERATOR (deref,      *FWD(x))
DEFINE_UNARY_OPERATOR (bit_not,    ~FWD(x))
DEFINE_UNARY_OPERATOR (logic_not,  !FWD(x))
DEFINE_BINARY_OPERATOR(plus,       FWD(x) + FWD(y))
DEFINE_BINARY_OPERATOR(subtract,   FWD(x) - FWD(y))
DEFINE_BINARY_OPERATOR(times,      FWD(x) * FWD(y))
DEFINE_BINARY_OPERATOR(divide,     FWD(x) / FWD(y))
DEFINE_BINARY_OPERATOR(mod,        FWD(x) % FWD(y))
DEFINE_BINARY_OPERATOR(bit_xor,    FWD(x) ^ FWD(y))
DEFINE_BINARY_OPERATOR(bit_and,    FWD(x) & FWD(y))
DEFINE_BINARY_OPERATOR(bit_or,     FWD(x) | FWD(y))
DEFINE_BINARY_OPERATOR(greater,    FWD(x) > FWD(y))
DEFINE_BINARY_OPERATOR(less,       FWD(x) < FWD(y))
DEFINE_BINARY_OPERATOR(shl,        FWD(x) << FWD(y))
DEFINE_BINARY_OPERATOR(shr,        FWD(x) >> FWD(y))
DEFINE_BINARY_OPERATOR(equal,      FWD(x) == FWD(y))
DEFINE_BINARY_OPERATOR(unequal,    FWD(x) != FWD(y))
DEFINE_BINARY_OPERATOR(less_eq,    FWD(x) <= FWD(y))
DEFINE_BINARY_OPERATOR(greater_eq, FWD(x) >= FWD(y))
DEFINE_BINARY_OPERATOR(logic_and,  FWD(x) && FWD(y))
DEFINE_BINARY_OPERATOR(logic_or,   FWD(x) || FWD(y))

// member operators
DEFINE_UNARY_OPERATOR (pre_inc,    ++FWD(x))
DEFINE_UNARY_OPERATOR (pos_inc,    FWD(x)++)
DEFINE_UNARY_OPERATOR (pre_dec,    --FWD(x))
DEFINE_UNARY_OPERATOR (pos_dec,    FWD(x)--)
DEFINE_UNARY_OPERATOR (int_,       int(FWD(x)))
DEFINE_UNARY_OPERATOR (float_,     float(FWD(x)))
DEFINE_UNARY_OPERATOR (double_,    double(FWD(x)))
DEFINE_BINARY_OPERATOR(set,        FWD(x) = FWD(y))
DEFINE_BINARY_OPERATOR(add_to,     FWD(x) += FWD(y))
DEFINE_BINARY_OPERATOR(sub_from,   FWD(x) -= FWD(y))
DEFINE_BINARY_OPERATOR(times_by,   FWD(x) *= FWD(y))
DEFINE_BINARY_OPERATOR(divide_by,  FWD(x) /= FWD(y))
DEFINE_BINARY_OPERATOR(mod_by,     FWD(x) %= FWD(y))
DEFINE_BINARY_OPERATOR(xor_by,     FWD(x) ^= FWD(y))
DEFINE_BINARY_OPERATOR(and_by,     FWD(x) &= FWD(y))
DEFINE_BINARY_OPERATOR(or_by,      FWD(x) |= FWD(y))
DEFINE_BINARY_OPERATOR(shl_by,     FWD(x) <<= FWD(y))
DEFINE_BINARY_OPERATOR(shr_by,     FWD(x) >>= FWD(y))
DEFINE_BINARY_OPERATOR(ptr_to_mem, FWD(x) ->* FWD(y))
DEFINE_BINARY_OPERATOR(subscript,  FWD(x)[FWD(y)])
//DEFINE_BINARY_OPERATOR(mem_access, FWD(x) -> FWD(y))

// function call operator
struct fun_call
{
    template<typename X, typename... Any>
    inline auto operator()(X&& x, Any&&... any) const
    { return FWD(x)(FWD(any)...); }
};

}


template<typename Arg>
struct _value_expr;

template<size_t I>
struct _slot_expr;

template<typename Operator, typename... Args>
struct _oper_expr;


template<typename Any>
using _clean_t = std::remove_cv_t<std::remove_reference_t<Any>>;

template<typename... Any>
struct _is_clean;
template<>
struct _is_clean<> : std::true_type {};
template<typename First, typename... Rest>
struct _is_clean<First, Rest...> : 
    std::integral_constant<bool, std::is_same<First, _clean_t<First>>::value && 
                                 _is_clean<Rest...>::value> {};

template<typename Any>
struct _remove_rref { using type = Any; };
template<typename Any>
struct _remove_rref<Any&&> { using type = Any; };

template<typename Any>
using _remove_rref_t = typename _remove_rref<Any>::type;


template<typename Any>
struct _is_expr : std::false_type {};
template<typename Any>
struct _is_expr<Any&> : _is_expr<Any> {};
template<typename Any>
struct _is_expr<Any&&> : _is_expr<Any> {};
template<typename Any>
struct _is_expr<const Any> : _is_expr<Any> {};
template<typename Arg>
struct _is_expr<_value_expr<Arg>> : std::true_type {};
template<size_t I>
struct _is_expr<_slot_expr<I>> : std::true_type {};
template<typename Operator, typename... Args>
struct _is_expr<_oper_expr<Operator, Args...>> : std::true_type {};

template<typename... All>
struct _any_expr;
template<typename First, typename... Rest>
struct _any_expr<First, Rest...> : 
    std::integral_constant<bool, _is_expr<First>::value || _any_expr<Rest...>::value> {};
template<>
struct _any_expr<> : std::false_type {};

// remove constness and reference for expression,
// wrap _value_expr for non-expression.
template<typename Any>
using _expr_t = std::conditional_t<_is_expr<Any>::value, _clean_t<Any>, _value_expr<_remove_rref_t<Any>>>;


template<typename Target, typename Value, typename = std::enable_if_t<!_is_expr<Value>::value>>
inline auto _expr_cast(Value&& arg) -> Target
{
    return Target(FWD(arg));
}
template<typename Target, typename Expr, typename = std::enable_if_t<_is_expr<Expr>::value>>
inline auto _expr_cast(Expr&& expr) -> decltype(expr)
{
    return FWD(expr);
}
template<typename Arg>
inline auto by_val(Arg&& arg)
{
    return _expr_cast<_value_expr<_clean_t<Arg>>>(FWD(arg));
}
template<typename Arg>
inline auto by_ref(Arg&& arg)
{
    return _expr_cast<_value_expr<Arg&&>>(FWD(arg));
}
template<typename Function>
inline auto by_unique_fn(Function&& f)
{
    return [f = FWD(f)](auto&&... args) { return _make_oper_expr(f, FWD(args)...); };
}
#define AUTO_OVERLOAD(f) [&](auto&&... args) { return f(std::forward<decltype(args)>(args)...); }
#define by_fn(f) by_unique_fn(AUTO_OVERLOAD(f))


// evaluation when pass by tuple
template<typename Function, typename Args, typename Fills, size_t... I>
inline auto _apply_eval_impl(
    Function f, Args args, Fills fills, std::integer_sequence<size_t, I...>) ->
    decltype(f(std::get<I>(args).eval(fills)...))
{
    return f(std::get<I>(args).eval(fills)...);
}
template<typename Function, typename Args, typename Fills, 
    typename Indices = std::make_integer_sequence<size_t, std::tuple_size<Args>::value>>
inline auto _apply_eval(Function f, Args args, Fills fills) ->
    decltype(_apply_eval_impl(f, args, fills, Indices{}))
{
    return _apply_eval_impl(f, args, fills, Indices{});
}
// evaluation when pass by parameter pack
template<typename Function, typename Args, typename... Fills, size_t... I>
inline auto _discrete_eval_impl(
    Function f, Args args, std::integer_sequence<size_t, I...>, Fills&&... fills) ->
    decltype(f(std::get<I>(args)(FWD(fills)...)...))
{
    return f(std::get<I>(args)(FWD(fills)...)...);
}
template<typename Function, typename Args, typename... Fills, 
    typename Indices = std::make_integer_sequence<size_t, std::tuple_size<Args>::value>>
inline auto _discrete_eval(Function f, Args args, Fills&&... fills) ->
    decltype(_discrete_eval_impl(f, args, Indices{}, FWD(fills)...))
{
    return _discrete_eval_impl(f, args, Indices{}, FWD(fills)...);
}



#define DEFINE_MEMBER_OPERATORS() \
CONSTRUCT_MEMBER_UNARY_EXPR (++,  pre_inc,)     \
CONSTRUCT_MEMBER_UNARY_EXPR (++,  pos_inc, int) \
CONSTRUCT_MEMBER_UNARY_EXPR (--,  pre_dec,)     \
CONSTRUCT_MEMBER_UNARY_EXPR (--,  pos_dec, int) \
CONSTRUCT_MEMBER_BINARY_EXPR(=,   set)          \
CONSTRUCT_MEMBER_BINARY_EXPR(+=,  add_to)       \
CONSTRUCT_MEMBER_BINARY_EXPR(-=,  sub_from)     \
CONSTRUCT_MEMBER_BINARY_EXPR(*=,  times_by)     \
CONSTRUCT_MEMBER_BINARY_EXPR(/=,  divide_by)    \
CONSTRUCT_MEMBER_BINARY_EXPR(%=,  mod_by)       \
CONSTRUCT_MEMBER_BINARY_EXPR(^=,  xor_by)       \
CONSTRUCT_MEMBER_BINARY_EXPR(&=,  and_by)       \
CONSTRUCT_MEMBER_BINARY_EXPR(|=,  or_by)        \
CONSTRUCT_MEMBER_BINARY_EXPR(<<=, shl_by)       \
CONSTRUCT_MEMBER_BINARY_EXPR(>>=, shr_by)       \
CONSTRUCT_MEMBER_BINARY_EXPR(->*, ptr_to_mem)   \
CONSTRUCT_MEMBER_BINARY_EXPR([],  subscript)    \
//CONSTRUCT_MEMBER_BINARY_EXPR(->,  mem_access)


template<typename Operator, typename... Args>
struct _oper_expr
{
    const Operator op_;
    const std::tuple<Args...> args_;
    
    explicit _oper_expr(Operator op, Args... args) : 
        op_(op), args_(std::make_tuple(FWD(args)...)) {}
    
    template<typename... Fills>
    inline auto eval(const std::tuple<Fills...>& fills) const ->
        decltype(_apply_eval(op_, args_, fills))
    {
        return _apply_eval(op_, args_, fills);
    }
    template<typename... Fills>
    inline auto operator()(Fills&&... fills) const -> 
        decltype(_discrete_eval(op_, args_, FWD(fills)...))
    {
        return _discrete_eval(op_, args_, FWD(fills)...);
    }
    template<typename... CallArgs>
    inline auto call(CallArgs&&... call_args) const
    {
        return _make_oper_expr(op::fun_call{}, *this, FWD(call_args)...);
    }

    DEFINE_MEMBER_OPERATORS()
};

template<typename Operator, typename... Args>
inline auto _make_oper_expr(Operator&& op, Args&&... args)
{
    return _oper_expr<_clean_t<Operator&&>, _expr_t<Args&&>...>(
        FWD(op), _expr_cast<_expr_t<Args&&>>(FWD(args))...);
}

template<typename Arg>
struct _value_expr
{
    // _value_expr may store a reference

    Arg arg_;

    explicit _value_expr(Arg arg) : arg_(arg) {}

    template<typename... Any>
    inline auto eval(Any&&...) const -> Arg
    {
        return arg_;
    }
    template<typename... Fills>
    inline auto operator()(Fills&&...) const -> Arg
    {
        return arg_;
    }
    template<typename... CallArgs>
    inline auto call(CallArgs&&... call_args) const
    {
        return _make_oper_expr(op::fun_call{}, *this, FWD(call_args)...);
    }
    
    DEFINE_MEMBER_OPERATORS()
};

template<size_t I>
struct _slot_expr
{
    // _slot_expr<0> is reserved for recursion
    static_assert(I != 0, "Slot 0 is not supported.");

    template<typename... Fills>
    inline auto eval(const std::tuple<Fills...>& fills) const -> 
        decltype(std::get<I - 1>(fills))
    {
        return std::get<I - 1>(fills);
    }
    template<typename... Fills>
    inline auto operator()(Fills&&... fills) const -> 
        decltype(std::get<I - 1>(std::forward_as_tuple(FWD(fills)...)))
    {
        return std::get<I - 1>(std::forward_as_tuple(FWD(fills)...));
    }
    template<typename... CallArgs>
    inline auto call(CallArgs&&... call_args) const
    {
        return _make_oper_expr(op::fun_call{}, *this, FWD(call_args)...);
    }
    
    DEFINE_MEMBER_OPERATORS()
};

// pre-defined slots
static const auto _  = _slot_expr<1>{};
static const auto _1 = _slot_expr<1>{};
static const auto _2 = _slot_expr<2>{};
static const auto _3 = _slot_expr<3>{};
static const auto _4 = _slot_expr<4>{};
static const auto _5 = _slot_expr<5>{};
static const auto _6 = _slot_expr<6>{};
static const auto _7 = _slot_expr<7>{};
static const auto _8 = _slot_expr<8>{};


// wrapper class for expressions
template<typename Expr>
struct _expr_object
{
    static_assert(_is_expr<Expr>::value, "Not an expression.");
    Expr expr_;

    template<typename TypeFree>
    _expr_object(TypeFree&& expr) : expr_(FWD(expr)) {}

    template<typename... Args>
    inline auto operator()(Args&&... args) const ->
        decltype(expr_.eval(std::forward_as_tuple(FWD(args)...)))
    {
        return expr_.eval(std::forward_as_tuple(FWD(args)...));
    }
};

// _expr_object generator
template<typename Expr>
inline auto lambda(Expr&& expr)
{
    return _expr_object<_clean_t<Expr>>(FWD(expr));
}

CONSTRUCT_UNARY_EXPR (-,  minus)
CONSTRUCT_UNARY_EXPR (&,  address)
CONSTRUCT_UNARY_EXPR (*,  deref)
CONSTRUCT_UNARY_EXPR (~,  bit_not)
CONSTRUCT_UNARY_EXPR (!,  logic_not)
CONSTRUCT_BINARY_EXPR(+,  plus)
CONSTRUCT_BINARY_EXPR(-,  subtract)
CONSTRUCT_BINARY_EXPR(*,  times)
CONSTRUCT_BINARY_EXPR(/,  divide)
CONSTRUCT_BINARY_EXPR(%,  mod)
CONSTRUCT_BINARY_EXPR(^,  bit_xor)
CONSTRUCT_BINARY_EXPR(&,  bit_and)
CONSTRUCT_BINARY_EXPR(|,  bit_or)
CONSTRUCT_BINARY_EXPR(>,  greater)
CONSTRUCT_BINARY_EXPR(<,  less)
CONSTRUCT_BINARY_EXPR(<<, shl)
CONSTRUCT_BINARY_EXPR(>>, shr)
CONSTRUCT_BINARY_EXPR(==, equal)
CONSTRUCT_BINARY_EXPR(!=, unequal)
CONSTRUCT_BINARY_EXPR(<=, less_eq)
CONSTRUCT_BINARY_EXPR(>=, greater_eq)
CONSTRUCT_BINARY_EXPR(&&, logic_and)
CONSTRUCT_BINARY_EXPR(||, logic_or)


#undef DEFINE_UNARY_OPERATOR
#undef DEFINE_BINARY_OPERATOR
#undef CONSTRUCT_UNARY_EXPR
#undef CONSTRUCT_BINARY_EXPR
#undef CONSTRUCT_MEMBER_UNARY_EXPR
#undef CONSTRUCT_MEMBER_BINARY_EXPR
#undef FWD
#pragma pop_macro("FWD")

}

#endif /* _LAMBDA_H */
