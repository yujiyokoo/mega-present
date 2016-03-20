# How to compile?

To make mruby/c sample programs, just `make` in top directory. `Makefile` generates mruby/c VM library `libmruby.a` and mruby/c executables.

## libmruby.c

`libmruby.c` is generated in `/src` directory.


## mruby/c executables

Three mruby/c executables are generated in `/sample_c` directory.

`mrubyc` is a mruby/c VM for one mruby byte-code file. This program executes one mrb file.

````
mrubyc basic_sample01.mrb
````
`mrubyc_concurrent` is a mruby/c VM for multiple mruby byte-code file. This program executes several mrb files.

````
mrubyc_concurrent basic_sample01.mrb basic_sample02.mrb
````

`mrubyc_sample` is a single mruby/c executable file included sample01.c.
