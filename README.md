# Тестовое задание в Nokia

## Формулировка

Задан CSV-файл с заголовком, в котором перечислены названия столбцов. Строки нумеруются целыми положительными числами,необязательно в порядке возрастания. В ячейках CSV-файла могут хранится или целые
числа или выражения вида:

```
= ARG1 OP ARG2
```

где ARG1 и ARG2 – целые числа или адреса ячеек в формате Имя_колонки Номер_строки, а OP – арифметическая операция
из списка: +, -, *, /.

Например, таблица

|   | A | B | Cell |
|---|---|---|---|
| 1 | 1 | 0 | -1|
| 2 | 2 |=A1+Cell30| 0 |
| 30| 0 |=B1+A1| 5 |

Будет представлена в нашем CSV-формате следующим образом:

```
,A,B,Cell
1,1,0,-1
2,2,=A1+Cell30,0
30,0,=B1+A1,5
```
(обратите внимание на пропуск первого значения в первой строке CSV-представления, он обозначает пустую левую верхнюю
ячейку таблицы).

Требуется написать программу, которая читает произвольную CSV-форму из файла (количество строк и столбцов может быть
любым), вычисляет значения ячеек, если это необходимо, и выводит получившуюся табличку в виде CSV-представления в
консоль.

Программа должна компилироваться компилятором gcc или clang в Linux, mingw/cygwin в Windows. Рекомендуется
использование стандартной библиотеки (STL). Использование любых сторонних библиотек (не STL) запрещено. Имя CSV-
файла необходимо передавать первым аргументом командной строки. В решении требуется предоставить инструкции по
компиляции программы или использовать следующие средства сборки: make, automake, cmake, gradle.
Пример запуска программы:
```
$ csvreader.exe file.csv
```
где csvreader.exe – имя исполняемого файла (может быть любым), file.csv – имя файла с таблицей (произвольно задаётся
пользователем), $ - приглашение командного интерпретатора.

Для приведённого примера результат, напечатанный в консоль, может быть таким:
```
,A,B,Cell
1,1,0,-1
2,2,6,0
30,0,1,5
```


