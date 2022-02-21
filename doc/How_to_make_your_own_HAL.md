## How to make your own HAL

For example, if your project looks like this:

```
your_project % tree -d
.
|-- mrubyc
|   |-- doc
|   |-- mrblib
|   |-- sample_c
|   |-- src
|   |   |-- hal_esp32
|   |   |-- hal_pic24
|   |   |-- hal_posix
|   |   `-- hal_psoc5lp
|   
|-- your_src
|   |-- hal.h
|   |-- hal.c
|   |-- your_main.c
|   |
```

You can make libmrubyc.a which uses your own HAL:

```
cd mrubyc/src
MRBC_USE_HAL=../../your_src make
```

and you can compile your code and link it with libmrubyc.a. 

```
cd your_src
cc -I../mrubyc/src -I. your_main.c ../mrubyc/src/libmrubyc.a
```
