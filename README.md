agos - algorithm search
An exhaustive search for algorithms by way of simulating each algorithm. This is closely related to superoptimization for instructions.

Given a set of inputs and corresponding outputs, this software searches the space of programs, written in a custom instruction set, which generate these outputs for their inputs. The search is conducted by iterating over every possible algorithm and simulating it, then checking if it produces the required output for each input.

This is a feasibility test and a guide for evaluating how long an exhaustive algorithm search for a particular algorithm would take, e.g. it can be used to evaluate how long it would take to find the known polynomial time algorithm for primality tests (estimation not done here).
The result is, as you would expect, that exhaustive search only works for very simply programs with about up to 5 instructions, because the search space grows exponentially with number of instructions. For these, the output is the superoptimized code, i.e. the shortest code possible that proudes the required output.

This software includes a JIT compiler for the custom instruction set to x86-64 instruction set for faster simulation of the algorithms. In the end, however, it turns out that simulating the custom instruction set is almost as fast as compiling it and running it natively, because one algorithm test is a very short living thing - most algorithms are excluded based on the first input-output-pair or quickly enter an infinite loop which is also detected fairly quickly, so that the speed increase of the native execution does not justify the extra effort of the compilation.
This is probably a different situation when superoptimizing an algorithm in a machine's native instruction set language, because then the native execution comes at a lower cost.
