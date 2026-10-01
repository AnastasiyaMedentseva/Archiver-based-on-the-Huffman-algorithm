# Архиватор на базе алгоритма Хаффмана

Программа-архиватор на языке C, использующая алгоритм Хаффмана для сжатия и распаковки файлов.

## Описание проекта

Проект состоит из нескольких модулей:

- Модуль анализа частот символов во входном файле
- Модуль построения дерева Хаффмана
- Модуль построения таблицы кодов символов
- Модуль работы с битовым потоком (чтение и запись)
- Модуль компрессии (сжатия)
- Модуль декомпрессии (распаковки)

Помимо этого в проекте есть модульные и интеграционные тесты, с помощью которых можно проверить работу как отдельных компонентов, так и всей программы целиком.
Также настроены автоматические проверки качества кода.

## Структура проекта

```text
.
├── README.md
├── LICENSE
├── CMakeLists.txt
├── .clang-format
├── .clang-tidy
├── .github/
│   └── workflows/
│       └── ci.yml
├── src/
│   ├── bitstream/
│   ├── code_table/
│   ├── compressor/
│   ├── decompressor/
│   ├── frequency_table/
│   ├── tree/
│   ├── cli/
│   └── main.c
├── test/
│   ├── unit/
│   ├── integration/
│   └── test_data/
└── experiments/
    ├── data/
    ├── results/
    └── experiments.c
```

## Требования

Для сборки проекта необходимы:

- CMake 3.16 или новее
- компилятор C с поддержкой C11
- CMocka — для тестов

Для дополнительных проверок используются:

- clang-format
- clang-tidy
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)
- Valgrind

## Сборка

Сборка проекта выполняется с помощью CMake:

```bash
cmake -S . -B build
cmake --build build
```

## Тесты

Тесты реализованы при помощи библиотеки CMocka.

Для запуска тестов её необходимо предварительно установить:

```bash
sudo apt update
sudo apt install libcmocka-dev
```

Для дополнительных проверок качества кода нужно установить инструменты:

```bash
sudo apt install clang-format clang-tidy valgrind
```

Тесты включены по умолчанию.

Запуск всех тестов:

```bash
ctest --test-dir build --output-on-failure
```

### Модульные тесты

Модульные тесты проверяют отдельные компоненты проекта:

- таблицу частот
- дерево Хаффмана
- таблицу кодов
- битовый поток

### Интеграционные тесты

Интеграционные тесты проверяют полный цикл:

```text
исходный файл
     ↓
   compress
     ↓
архивированный файл
     ↓
  decompress
     ↓
восстановленный файл
```

После восстановления содержимое сравнивается с исходным файлом.

### Тестовые данные

Тестовые файлы находятся в:

```text
test/test_data/
```

В тестах используются пустой файл, файл с одним символом, обычный текстовый файл, файл с набором символов и простой тестовый файл.

Интеграционные тесты сравнивают исходные и восстановленные файлы в бинарно-безопасном режиме.

## Дополнительные проверки

### clang-format

Проверка форматирования:

```bash
cmake -S . -B build-format -DENABLE_CLANG_FORMAT=ON
cmake --build build-format --target format-check
```

Конфигурация форматирования находится в `.clang-format`.

### clang-tidy

Запуск статического анализа:

```bash
cmake -S . -B build-tidy -DENABLE_CLANG_TIDY=ON
cmake --build build-tidy --parallel
```

Настройки находятся в `.clang-tidy`.

### AddressSanitizer и UBSan

```bash
cmake -S . -B build-sanitize \
    -DENABLE_ASAN=ON \
    -DENABLE_UBSAN=ON

cmake --build build-sanitize --parallel
ctest --test-dir build-sanitize --output-on-failure
```

### Valgrind

```bash
cmake -S . -B build-valgrind -DENABLE_VALGRIND=ON
cmake --build build-valgrind --parallel
ctest --test-dir build-valgrind --output-on-failure
```

## Опции CMake

Проект предоставляет следующие опции:

| Опция | По умолчанию | Назначение |
|---|---:|---|
| `BUILD_TESTS` | `ON` | Сборка тестов |
| `ENABLE_ASAN` | `OFF` | AddressSanitizer |
| `ENABLE_UBSAN` | `OFF` | UndefinedBehaviorSanitizer |
| `ENABLE_VALGRIND` | `OFF` | Запуск тестов через Valgrind |
| `ENABLE_CLANG_TIDY` | `OFF` | Статический анализ clang-tidy |
| `ENABLE_CLANG_FORMAT` | `OFF` | Проверка форматирования clang-format |

Пример:

```bash
cmake -S . -B build \
    -DBUILD_TESTS=ON \
    -DENABLE_ASAN=ON \
    -DENABLE_UBSAN=ON
```

## CI

В проекте настроен GitHub Actions workflow:

```text
.github/workflows/ci.yml
```

CI запускается для Pull Request в ветку `main`.

Проверяются:

- сборка проекта
- модульные и интеграционные тесты
- clang-format
- clang-tidy
- AddressSanitizer + UBSan
- Valgrind

## Эксперименты

Проект поддерживает проведение экспериментов.

Исходные файлы и результаты замеров производительности находятся в директории `experiments`.

В качестве входных данных созданы файлы следующих типов:

- Текстовые данные
- Повторяющиеся текстовые данные
- Случайные данные
- Сжатые архиватором данные

Для каждого типа доступны файлы размером 100 KB, 1 MB и 10 MB.

Всего выполняется по 10 прогонов на каждый файл.

В ходе экспериментов вычисляются:

- коэффициент сжатия 
- время сжатия
- время расжатия
- среднее значение степени сжатия
- среднее значение времени выполнения программы
- среднеквадратичное отклонение степени сжатия 
- среднеквадратичное отклонение времени выполнения программы

Результаты сохраняются в директории `results`.

Запуск экспериментов:

```bash
cmake -S . -B build-experiment
cmake --build build-experiment
./build-experiment/experiments
```
