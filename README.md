
**Этот же документ - [версия на русском языке, идти сюда!](README_RU.md)**

## ContestChecker
ContestChecker is free (both "as free beer" and "as free speech") tool or engine for running simple algorithm solutions.  
It is intended to smooth edges in education, olympiad training etc.  
It features codegeneration to remove boilerplate and repetioion when dealing writing IO and tests for such solutions.

## License
Project source code is licensed under [Creative Commons Zero v1.0 Universal](LICENSE).

## Tools requirements
1. C++20 capable compiler (MSVC 2019, MSVC 2022, GCC 14.2 was tested)
2. CMake 3.20 +
3. Git (recommended)

## How to build project
1. Clone repository or download zip/tar source code
2. Run CMake with default options
3. Run build via make or ninja

## How to add new Problem and Solutions
You can figure details by your own, looking at examples in Problems and Solution folders, but overall order is:  
1. Create new `Problems/` subfolder, for example, `Problems/ArraySum`
2. Create new header, like `Problems/ArraySum/ProblemArraySum.h`
3. `ProblemArraySum.h` must contain `inline namespace PROBLEM_NAMESPACE {` and typedefs `Input` and `Output` in it. Also typedef `TestCaseList` is recommended for adding tests.
4. (recommended) create `Problems/ArraySum/ProblemArraySum_tests.h` and implement `const TestCaseList& getTests()` inside anonymous namespace.
5. Create new Solution file inside `Solutions/` folder, it can be placed in any subfolder. Is is still recommended to create separate folder for each problem.
6. Header file must be in format `Solution{ProblemName}_{author}_{impl}`, e.g. `SolutionArraySum_smith_naive.h`
7. this header file contents must start with anonymous namespace `namespace {` after preprocessor and contain implementation of function `Output solution(const Input& input) {}`. It is recommended to include problem header `Problems/ArraySum/ProblemArraySum.h`

After you done adding code, run CMake again so it can generate integration code in build directory.

## How to run - basics
Run ContestChecker without arguments to run all tests for all problems.  
This is equivalent of running with `--task` option set to `CheckOutput`  
```
ContestChecker
ContestChecker --task CheckOutput
```
In this mode, every `Output solution(const Input& input) {}` function is executed and if any test fails, it will output failure details in stdout and terminate the program.  
Also, if no errors are found, it will output execution time of all testcases for the solution.  

You can also run a microbenchmark with `--task` option set to `Benchmark`:  
```
ContestChecker --task Benchmark
```
This mode will ignore all output of solutions and will execute each solution in a loop until certain threshold is passed (default 10 seconds).  
After it is done running, it will output iterations for each solution (more=faster, better).  

And last, you can run all solutions and just print their output without checking:  
```
ContestChecker --task PrintOutput
```
Default output is stdout. If you want to redirect output, see `print customization` section. See `Solution filtering` if you want to run just a single solution.

## Solution filtering
Each solution has 3 properties to filter on:  
1. Problem, set via `--problem` commandline parameter, corresponds to problem name (folder in `Problems`) or first part in `Solution*_.h`.
2. Author (or student), set via `--student` parameter. Author part is usually second part between underscores in Solution filename.
3. Implementation (or variant), set via `--impl` parameter. It is usually the last part in Solution filename. It should desribe approach in short and meaningful way; When in doubt, start with `naive`, `fast`.  

Every parameter can be used on its own or in combination with others:  
```
ContestChecker --student mapron
ContestChecker --task PrintOutput --student mapron --problem ArraySum --impl naive
ContestChecker --task Benchmark --problem ArraySum --impl naive
```

## Print customization
By default, ContestChecker prints its log of execution into stdout and ignores output.  
You can change log and print behaviour using `--print-to` and `--log-to` parameters.  
Values can be: full filename or one of special names:  
1. `stdin` - standard input, like `std::cin`
2. `stdout` - standard output, like `std::cout`
3. `stderr` - standard error ouput (second output channel) like `std::cerr`. Not every terminal prints out/err different (Windows cmd doesn't).
4. `null` - supress output. 

Examples:  
```
ContestChecker --problem ArraySum --print-to current_dir_output.txt --log-to /absolute/path/to/log.txt
ContestChecker --problem ArraySum --print-to stderr --log-to null
```

## Extra logging
You can add `--print-all-cases 1` to parameter list to enable printing statistics for each test case (like run time and memory usage):  
```
ContestChecker --problem ArraySum --print-all-cases 1
```
You can also add `--enable-alloc-trace 1` parameter that will trace and print useful information about memory usage:  
```
ContestChecker --problem ArraySum --print-all-cases 1 --enable-alloc-trace 1
ContestChecker --problem ArraySum --enable-alloc-trace 1
ContestChecker --problem ArraySum --print-all-cases 0 --enable-alloc-trace 1 # same as previous
```
Information tracked include:
1. Amount of new() calls;
2. Total memory allocated with all new() calls;
3. Time spent in new() calls;
4. Amount of delete() calls;
5. Time spent in delete() calls;
6. Percentage of new()+delete() calls compared to all time.  

Note: this will track all new/delete, even indirect (for example, any usage of STL container or using make_unique()).    

You can use this info to detect if you have any memory leak (new calls not equal to delete calls), estimate total memory usage (be careful as it sum all re-allocations), or decide if you algorithm most heavy part is working with allocations.

## Adding tests in text files
When dealing with large test data, C++ array may be inconvenient.  
You can add files in text format for any problem:
1. Decide how many cases you want, for example 2, and consider 0-based indexes, so you need case `0` and case `1`
2. create `input_{index}.txt` and `output_{index}.txt` for each test case inside Problem folder, for example:  
```
Problems/ArraySum/input_0.txt
Problems/ArraySum/output_0.txt
Problems/ArraySum/input_1.txt
Problems/ArraySum/output_1.txt
```
3. re-run CMake; it will generate code to run those cases automatically.

Warning: `writeTo()` and`readFrom()` for user types must not be empty.   

## Providing custom test file
If you want to run on single input+output pair, you can provide both input and output for a problem.
```
ContestChecker --problem ArraySum --test-input /path/to/input_0.txt --test-output /path/to/output_0.txt
```
Note: this will skip all other test sources.

## Writing custom Input and Output types
Most of the time your `ProblemSomething.h` will look similar to this, reusing types from `CommonTypes`:  
```
inline namespace PROBLEM_NAMESPACE {

using Input = CommonTypes::IntegralArrayIO<int>;
using Output = CommonTypes::IntegralScalar<int64_t>;
using TestCaseList = CommonTypes::TestCaseList<Input, Output>;

}
```
But what if you need to introduce new special input? This type must satisfy those requirements:  
```
struct Input {
    SomeType m_someField;

    bool operator==(const Input&) const = default; // must be equal-comparable

    void log(std::ostream& os) const { }     // this used for pretty-printing in run log output.
    void writeTo(std::ostream& os) const { } // this is used when write to text file
    void readFrom(std::istream& is) {}       // this is used when reading from text file (or stdin)
};
// same for Output if needed
```
If you are not planning to use `stdin/stdout/file` read and write you can make `writeTo()` and `readFrom()` empty.
