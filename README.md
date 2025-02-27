**Этот же документ - [версия на русском языке, идти сюда!](README_RU.md)**

# ContestChecker
ContestChecker is free (both "as free beer" and "as free speech") tool or engine for running simple algorithm solutions.  
It is intended to smooth edges in education, olympiad training etc.  
It features codegeneration to remove boilerplate and repetioion when dealing writing IO and tests for such solutions.

# License
Project source code is licensed under [Creative Commons Zero v1.0 Universal](LICENSE).

# Tools requirements
1. C++20 capable compiler (MSVC 2019, MSVC 2022, GCC 14.2 was tested)
2. CMake 3.20 +
3. Git (recommended)

# How to build project
1. Clone repository or download zip/tar source code
2. Run CMake with default options
3. Run build via make or ninja

# How to add new Problem and Solutions
You can figure details by your own, looking at examples in Problems and Solution folders, but overall order is:  
1. Create new Problems/ subfolder, for Example, /Problems/ArraySum
2. Create new header, like ArraySum/ArraySum.h
3. ArraySum.h must contain separate namespace and typedefs "Input" and "Output" in it.
4. (recommended) create ArraySum/ArraySum_tests.h and declare a method reterning vector of Input+Output structs for easy testing.
5. Create new Solution file inside /Solutions folder, it can be placed in any subfolder
6. Header file must be in format "Solution{ProblemName}_{author}_{impl}", e.g. "SolutionArraySum_smith_naive.h"
7. this header file must contain function named "solution", taking single "Output" and returning "Input" types. It is recommended to include problem header.

After you done adding code, run Cmake again so it can generate integration code in build directory.

# How to run
Run ContestChecker without arguments to run all tests for all problems.  
You can filter specific problem via  
```
ContestChecker --problem ArraySum
```
You can run benchmarks providing task parameter like this:  

```
ContestChecker --problem ArraySum --task Benchmark
```
Note: in benchmark mode, checking output is ignored.  
If you wast to print program output instead of checking it, you can use this task option:  
```
ContestChecker --problem ArraySum --task PrintOutput
```
Also you can customize check input files (other test data will be ignored) or set print/logging to different streamm:  
```
ContestChecker --problem ArraySum --test-input /path/to/input_0.txt --test-output /path/to/output_0.txt
ContestChecker --problem ArraySum --test-input stdin --test-output stdin
ContestChecker --print-to /path/to/output.txt --log-to /path/to/log.txt
```