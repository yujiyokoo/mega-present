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
|   |   |-- hal_psoc5lp
|   |   `-- hal_user_reserved
|   
|-- your_src
|   |-- hal
|   |   |-- hal.c
|   |   `-- hal.h
|   |
```

Make symlinks to your `hal.c` and `hal.h` :

```
your_project % ln -s ../../../your_src/hal/hal.c mrubyc/src/hal_user_reserved/hal.c
your_project % ln -s ../../../your_src/hal/hal.h mrubyc/src/hal_user_reserved/hal.h
```

They will look like:

```
.
|-- mrubyc
|   |-- doc
|   |-- mrblib
|   |-- sample_c
|   |-- src
|   |   |-- hal_esp32
|   |   |-- hal_pic24
|   |   |-- hal_posix
|   |   |-- hal_psoc5lp
|   |   `-- hal_user_reserved
|   |       `-- hal.c -> ../../../your_src/hal/hal.c  # symlink
|   |       `-- hal.c -> ../../../your_src/hal/hal.h  # symlink
|   
|-- your_src
|   |-- hal
|   |   |-- hal.c
|   |   `-- hal.h
|   |
```

Now you can make libmrubyc.a which uses your own HAL:

```
your_project/mrubyc $ CFLAGS=-DMRBC_USE_HAL_USER_RESERVED make
```

