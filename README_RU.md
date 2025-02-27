**This document is available [in English version, follow this link!](README.md)**

# ContestChecker
ContestChecker свободная и бесплатная утилита-движок для запуска несложных решений алгоритмических задач.  
Её назначение - сгладить углы для обучения программированию, подготовки к олимпиадам и т.д.  
Возможности включают в себя кодогенерацию для сокращения повторяющегося кода и предотвращения "копипасты", например ввода-вывода и оформления тестов.  

# License
Исходный код поставляется под лицензией [Creative Commons Zero v1.0 Universal](LICENSE).

# Требования к инструментам
1. C++20 компилятор (MSVC 2019, MSVC 2022, GCC 14.2 проверялись) 
2. CMake 3.20 +
3. Git (рекомендуется)

# Как собрать
1. Получите исходный код через git clone или скачиванием архива с исходниками с gihub
2. Запустить CMake с окружением вашего компилятора
3. Запустить сборку через make или ninja

# Как добавлять Проблемы (Problems) и Решения (Solutions)
Для начала можете попробовать посмотреть готовые примеры в директориях Problems и Solution; если не получилось разобраться, вот правила:  
1. Создать поддиректорию в Problems/ - например, /Problems/ArraySum
2. Создать header-файл, например ArraySum/ArraySum.h
3. ArraySum.h должен содержать уникальный namespace (например ArraySumProblemDetails) и typedef-ы "Input" и "Output" внутри namespace.
4. (рекомендуется) создайте ArraySum/ArraySum_tests.h и реализуйте getTests() метод возвращающий массив структур Input+Output.
5. Создайте новый файл Solution внутри /Solutions - структура директорий здесь не важна, можете создавать сколько угодно поддиректорий для удобства.
6. При этом имя файла должно иметь фиксированный формат "Solution{ProblemName}_{author}_{impl}", т.е. "SolutionArraySum_ivanov_naive.h"
7. В этом header-файле обязана быть свободная функция "solution", принимающая "Output" и возвращающая "Input" - рекомендуется подключать соответствующий проблеме header.
После добавления файлов с кодом, перезапустите CMake чтобы он смог сгенерировать необходимый код.

# Как запускать
Запуск ContestChecker без аргументов приведет к запуску всех доступных тестов.  
Вы можете задать фильтр проблемы с помощью аргумента problem:  
```
ContestChecker --problem ArraySum
```
Если вместо запуска тестов необходимо запустить бенчмарк, вы можете это сделать используя параметр task:  
```
ContestChecker --problem ArraySum --task Benchmark
```
примечание: в режиме бенчмарка выходные данные решения игнорируются.  
Если же необходимо вместо проверки выхода просто вывести его в консоль, можно воспользоваться  
```
ContestChecker --problem ArraySum --task PrintOutput
```
Так же есть возможность задать пользовательские файлы для ввода и вывода (в т.ч. использовать stdin/stderr/stdout):  
```
ContestChecker --problem ArraySum --test-input /path/to/input_0.txt --test-output /path/to/output_0.txt
ContestChecker --problem ArraySum --test-input stdin --test-output stdin
ContestChecker --print-to /path/to/output.txt --log-to /path/to/log.txt
```