# *lambda*

*lambda* is a tiny library for writing very short anonymous functions. 

## A basic example

    #include "lambda.h"
    using namespace lambda; // to make things simple

    int main(int argc, char* argv[])
    {
        auto add7 = _ + 7;             // _ represents a slot for the argument
        std::cout << add7(4) << "\n";  // prints 11
        return 0;
    }

Compile the program with a compiler support C++14 (g++ >= 5.1, icc >= 16.0).

## Slots and operators

Slots in *lambda* are expressions evaluated to be corresponding arguments when function calls are made. They are pre-defined as `_1`, `_2`, ..., `_8`, and `_` is a synonym for `_1`. Common C++ operators are overloaded for slots so that writing lambdas is just like writing normal expressions. 

**Arithmetic**

    auto f = _1 * _2 + _3;
    f(2, 3, 4);            // gives 10

**Assignment**

    int x = 0;
    auto f = (_ = 42);
    f(x);                  // x becomes 42
    
**Function call**

    auto f = _1.call(_2, _3);
    struct { bool operator()(bool x, bool y) { return !x & y; } } andn;
    f(andn, false, true);  // gives true
    
**Multiple statements**
    
    auto f = (_1 ^= _2, _2 ^= _1, _1 ^= _2);
    int x = 1, y = 2;
    f(x, y);               // x becomes 2, y becomes 1
    
**Slot number**

    auto f = _4;
    f(1, 2, 3, 4, 5);      // gives 4, all other arguments are ignored
    
**Standard output**

    auto f = std::cout << _ << "\n";
    f("Hello, world!");    // prints Hello, world!
    
**Use with STL**

    std::vector<int> v = {1, 2, 3, 4, 5};
    std::for_each(v.begin(), v.end(), _++);

## Capture methods

### `by_val` and `by_ref`

*lambda* expressions capture variables by references by default. For example, 

    int x = 0;
    auto f = _ + x;
    x = 5;
    f(1);     // gives 6
    
You can explicitly specify how *lambda* expressions capture variables using `by_val` and `by_ref`:

    int x = 0;
    auto f = _ + by_val(x);
    x = 5;
    f(1);     // gives 1

In certain cases, `by_val` and `by_ref` are used to delay evaluations. For example,  

    auto x = 0;
    auto f = by_ref(std::cout) << "count: " << ++by_ref(x) << "\n";
    f();      // prints count: 1
    f();      // prints count: 2
    f();      // prints count: 3

Otherwise, "count: " is immediately printed, and `x` is incremented and then passed by value. 

### `by_fn`

*lambda* allows you to use functions in expressions using `by_fn` macro, even if they are not overloaded for *lambda* expressions like slots. For example, 

    auto f = 1 - 2 * by_fn(std::sin)(_);
    f(0.5);    // gives 0.0411489
    
If you prefer `printf` to `std::cout`: 

    auto f = by_fn(printf)("%s\n", _);
    f("Hello, world!");

## Comparisons

**Native C++ lambda expression**

*lambda* is relative short for many simple expressions, especially where there are a lot of operators: 

    _1 * _2 + _3;                                    // this library
    [](auto x, auto y, auto z) { return x * y + z }; // native lambda

But *lambda* do not support certain operators (`.`, `.*`, `static_cast`, `dynamic_cast`, ...), thus goes very bad with object-oriented programming. 

**Boost::lambda**

*lambda* is superior because of functionalities provided by C++11. Type inference and native lambda support make *lambda* expressions easy to write: *lambda* supports an abitrary number of slots, and automatically handles types. 

**About performance**

*lambda* expressions are in forms of expression templates, where references and values are stored during compiling. Arguments in the calls are passed through perfect forwarding to every nodes in the expression trees, and compilers should be able to optimize away unnecessary evaluations. 

In the following example, the for_each loop will be vectorized, where `_ += 1` is evaluated by `paddd`(SEE2) or `vpaddd`(AVX2) instruction if supported:

    std::vector<int> x(1000, 1);
    std::for_each(x.begin(), x.end(), _ += 1);
    std::cout << x[rand() % 2] << "\n";







