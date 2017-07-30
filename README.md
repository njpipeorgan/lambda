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

## Slots and operators

Slots in *lambda* are expressions evaluated to be corresponding arguments when function calls are made. They are pre-defined as `_1`, `_2`, ..., `_8`, and `_` is a synonym for `_1`. Common C++ operators are overloaded for slots so that writing lambdas is just like writing normal expressions. 

**Arithmetic**

    auto f = _1 * _2 + _3;
    f(2, 3, 4);         // gives 10

**Comparison**

    auto f = _1 < _2;
    f(3, 2);            // gives false

**Standard output**

    auto f = std::cout << _ << "\n";
    f("Hello, world!"); // prints Hello, world!

**Assignment**

    int x = 0;
    auto f = (_ = 42);
    f(x);               // x becomes 42
    
**Slot number**

    auto f = _4;
    f(1, 2, 3, 4, 5);   // gives 4, all other arguments are ignored

## Capture methods

### `by_val` and `by_ref`

*lambda* expressions captures variables by references by default. For example, 

    int x = 0;
    auto f = _ + x;
    x = 5;
    f(1);   // gives 6
    
You can explicitly specify how *lambda* captures variables using `by_val` and `by_ref`:

    int x = 0;
    auto f = _ + by_val(x);
    x = 5;
    f(1);   // gives 1

In certain cases, `by_val` and `by_val` are used to delay evaluations:

    auto x = 0;
    auto f = by_ref(std::cout) << "count: " << ++by_ref(x) << "\n";
    f();    // prints count: 0
    f();    // prints count: 1
    f();    // prints count: 2

### `by_fn`

*lambda* allows you to use functions in expressions using `by_fn` macro, even if they are not overloaded for expressions like slots. For example, 

    auto f = 1 - 2 * by_fn(std::sin)(_)



































