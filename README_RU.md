
**This document is available [in English version, follow this link!](README.md)**

## ContestChecker
ContestChecker свободная и бесплатная утилита-движок для запуска несложных решений алгоритмических задач.  
Её назначение - сгладить углы для обучения программированию, подготовки к олимпиадам и т.д.  
Возможности включают в себя кодогенерацию для сокращения повторяющегося кода и предотвращения "копипасты", например ввода-вывода и оформления тестов.  

## License
Исходный код поставляется под лицензией [Creative Commons Zero v1.0 Universal](LICENSE).

## Требования к инструментам
1. C++20 компилятор (MSVC 2019, MSVC 2022, GCC 14.2 проверялись) 
2. CMake 3.20 +
3. Git (рекомендуется)

## Как собрать
1. Получите исходный код через git clone или скачиванием архива с исходниками с gihub
2. Запустить CMake с окружением вашего компилятора
3. Запустить сборку через make или ninja

## Как добавлять Проблемы (Problems) и Решения (Solutions)
Для начала можете попробовать посмотреть готовые примеры в директориях Problems и Solution; если не получилось разобраться, вот правила:  
1. Создать поддиректорию в `Problems/` - например, `Problems/ArraySum`
2. Создать header-файл, например `Problems/ArraySum/ProblemArraySum.h`
3. `ProblemArraySum.h` должен содержать namespace `inline namespace PROBLEM_NAMESPACE {` и typedef-ы `Input` и `Output` внутри namespace. Рекомендуется так же определить тип `TestCaseList` для следующего шага.
4. (рекомендуется) создайте `Problems/ArraySum/ProblemArraySum_tests.h` и реализуйте в нем `const TestCaseList& getTests()` в анонимном namespace.
5. Создайте новый файл Solution внутри `Solutions/` - структура директорий здесь не важна, можете создавать сколько угодно поддиректорий для удобства. Всё же рекомендуется создавать директории для каждой проблемы.
6. При этом имя файла должно иметь фиксированный формат `Solution{ProblemName}_{author}_{impl}`, т.е. `SolutionArraySum_ivanov_naive.h`
7. Содержимое этого header-файла обязан начинаться  с анонимного namespace - `namespace {`  (после препроцессора); внутри него должна быть реализована функция `Output solution(const Input& input) {}` ; рекомендуется подключать соответствующий проблеме header `Problems/ArraySum/ProblemArraySum.h`
После добавления файлов с кодом, перезапустите CMake, чтобы он смог сгенерировать необходимые файлы для запуска решений.

## Запуск - основы
Запустите ContestChecker без аргументов, чтобы запустить все тесты для всех проблем.   
Это эквивалентно запуску с опцией `--task`, установленной на `CheckOutput`
```
ContestChecker
ContestChecker --task CheckOutput
```
В этом режиме каждая функция  `Output solution(const Input& input) {}` выполняется, и если один из тестов провалится, будет выведена информация об ошибке и выполнение прервано.  
Если все тесты успешно прошли, то в выводе будет лог с временем выполнения каждого решения.   

Вы можете запустить микробенчмарк решения используя `--task` опцию равную `Benchmark`:  
```
ContestChecker --task Benchmark
```
В этом режиме все выходы решений игнорируются. Каждое решение запускается в цикле пока не пройдет достаточно времени (по умолчанию 10 секунд).
После окончания замеров, будет выведено количество пройденных итераций - `iterations: ` (больше=лучше, быстрее)

Наконец, вы можете просто запустить все решения и вывести их выход в консоль:  
```
ContestChecker --task PrintOutput
```
Вывод по умолчанию в stdout. Если вам нужно другое поведение, можете посмотреть секцию `Настройка вывода` . См `Фильтр решений`  если необходимо вывести только одно решение.

## Фильтр решений
Каждое решение имеет три параметра доступных для фильтрации:  
1. Проблема, выставляется опцией `--problem` соответствует имени директории проблемы (в `Problems`) либо первой части в `Solution*_.h`.
2. Автор (или ученик, студент), выставляется опцией `--student` . Автор обычно вторая часть в имени header-файла решения между подчеркиваниями. 
3. Реализация (или вариант), выставляется опцией  `--impl` . Обычно является последней частью имени header-файла. Решение обычно лаконично выражает идею решения. Если сомневаетесь, используйте имена `naive`, `fast`.  

Каждый параметр может использоваться как самостоятельно, так и в комбинации с другими:  
```
ContestChecker --student mapron
ContestChecker --task PrintOutput --student mapron --problem ArraySum --impl naive
ContestChecker --task Benchmark --problem ArraySum --impl naive
```

## Настройка вывода
По умолчанию, ContestChecker печатает лог выполнения в стандартный вывод (stdout) и никак не печатает выход решений.  
Это поведение для вывода и лога можно поменять с помощью параметров  `--print-to` и`--log-to`
Значения могут быть либо полными именем файла, либо специальными именами:
1. `stdin` - стандартный ввод, как `std::cin`
2. `stdout` - стандартный вывод, как `std::cout`
3. `stderr` - стандартный канал ошибок, как `std::cerr`. Не все терминалы визуально оформляют out и err выводы по-разному (Например, Windows cmd не различает).
4. `null` - Подавить вывод. 

Примеры:  
```
ContestChecker --problem ArraySum --print-to current_dir_output.txt --log-to /absolute/path/to/log.txt
ContestChecker --problem ArraySum --print-to stderr --log-to null
```

## Дополнительное логирование
Можете добавить параметр`--print-all-cases 1` для включения статистики по каждому тест кейсу (а не решению целиком), будет добавлен вывод времени выполнения и использования памяти:  
```
ContestChecker --problem ArraySum --print-all-cases 1
```
Можете добавить`--enable-alloc-trace 1` опцию, которая позволит включить замеры и логирование использования памяти:  
```
ContestChecker --problem ArraySum --print-all-cases 1 --enable-alloc-trace 1
ContestChecker --problem ArraySum --enable-alloc-trace 1
ContestChecker --problem ArraySum --print-all-cases 0 --enable-alloc-trace 1 # same as previous
```
Информация включает в себя:
1. Количество вызовов new() ;
2. Суммарно выделенная память через new();
3. Время которое потрачено на все new();
4. Количество вызовов delete();
5. Время потраченное на все delete();
6. Процент new()+delete() вызовов относительно общего времени.  

Внимание: происходит перехват ВСЕХ new/delete вызовов программы, даже неявных (например, любые использования STL контейнеров или вызов make_unique()).    

Вы можете воспользоваться данной информацией, например для определения, есть ли утечка памяти (кол-во new() должно равняться кол-ву delete()), оценить использование памяти (осторожно, т.к. в статистику попадают пере-аллокации), или определить насколько существенную долю в вашем алгоритме занимает выделение памяти. 

## Добавление тестов в виде тестовых файлов
При работе с большими входными данными, тесты в виде C++ массивов не всегда удобны.  
Вы можете добавлять тестовые файлы в виде текста для любой проблемы:
1. Определите, сколько кейсов вы хотите создать, например, 2; учитывая индексы с 0 вы должны создать кейс `0` и кейс `1`
2. Создайте файлы`input_{index}.txt` и`output_{index}.txt` для каждого кейса в директории проблемы, например:  
```
Problems/ArraySum/input_0.txt
Problems/ArraySum/output_0.txt
Problems/ArraySum/input_1.txt
Problems/ArraySum/output_1.txt
```
3. Перезапустите CMake; он создаст необходимый код для запуска этих кейсов.

Предупреждение: реализации `writeTo()` и`readFrom()` для пользовательских типов не должны быть пусты. 
## Пользовательские файлы для теста
Если вы хотите запустить тесты на отдельной паре файлов, не добавляя их в директорию проблемы, это можно сделать так:
```
ContestChecker --problem ArraySum --test-input /path/to/input_0.txt --test-output /path/to/output_0.txt
```
Примечание: в таком режиме все остальные источники тестов будут пропущены.

## Пользовательские типы Input и Output 
В большинстве случаев ваш header-файл проблемы `ProblemSomething.h`  просто использует готовые типы из `CommonTypes`:  
```
inline namespace PROBLEM_NAMESPACE {

using Input = CommonTypes::IntegralArrayIO<int>;
using Output = CommonTypes::IntegralScalar<int64_t>;
using TestCaseList = CommonTypes::TestCaseList<Input, Output>;

}
```
Что если стандартные не подходят и необходим новый тип входа? В таком случае вместо using вы объявляете новый тип, который соответствует следующим требованиям:  
```
struct Input {
    SomeType m_someField;

    bool operator==(const Input&) const = default; // Должен быть сравниваемым на равенство.

    void log(std::ostream& os) const { }     // Это необходимо для вывода в лог
    void writeTo(std::ostream& os) const { } // используется для записи в текстовый файл
    void readFrom(std::istream& is) {}       // используется для чтения из текстового формата (файл или stdin)
};
// same for Output if needed
```
Если вы точно не собираетесь использовать файлы или стандартные потоки для кейсов (и пользоваться только C++ - тестами), вы можете оставить  `writeTo()` и`readFrom()` пустыми.

