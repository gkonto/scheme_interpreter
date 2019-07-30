# scheme_interpreter
<<<<<<< HEAD
Scheme Interpreter written in c++

(PROJECT UNDER DEVELOPMENT)

This is an implementation of Bootstrap Scheme interpreter, based on peter.michaux's "Scheme From Scratch".

Bootstrap Scheme is a quick and very dirty Scheme interpreter. Its only intended use is to compile a self-compiling Scheme-to-Assembly or Scheme-to-C compiler the first time. Bootstrap Scheme doesn't have many features a Scheme system usually has. It doesn't have numbers other than integers. It doesn't have vectors. It definitely doesn't have a module system, call/cc, macros, dynamic-wind or any other advanced Scheme features. Bootstrap Scheme is slow. The implementation is an abstract syntax tree node walker with no optimizations. There is no point in making a node walking interpreter any better than the absolute base necessity as the fundamental design of a node walker would never be used in production. Small, easy to read source code is far more important than anything else. Bootstrap Scheme revels in the opportunity to be very dirty Scheme.

How to compile

The project can be easily compiled with CMake. CMake is a cross-platform free and open-source software application for managing the build process of software using a compiler-independent method.

Steps:

cd
mkdir my_build
cd my_build/
cmake ../
make Job Done.

Follow the same instructions for compiling the tests folder.
=======
A basic Scheme Interpreter written in c++
>>>>>>> 2c5142fbf20809ac2f15a09516f26648da20dd37
