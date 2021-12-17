# mruby/c

[![Build Status](https://travis-ci.org/mrubyc/mrubyc.svg?branch=master)](https://travis-ci.org/mrubyc/mrubyc)

[![Join the chat at https://gitter.im/mrubyc/mrubyc](https://badges.gitter.im/mrubyc/mrubyc.svg)](https://gitter.im/mrubyc/mrubyc?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

mruby/c is an another implementation of mruby.

- Small memory consumption
- Limited class libraries
- Small size rather than execution speed

### Comparison between mruby and mruby/c

||mruby/c|mruby|
|----|----|----|
|memory size| < 40KB | < 400KB |
|main target| one-chip microprocessors | general embedded software|


## Documents

[How to compile?](doc/compile.md)

[How to run tests?](doc/test.md)


## Developer team

- [Shimane IT Open-Innovation Center](http://www.s-itoc.jp/)
- [Kyushu Institute of Technology](http://www.kyutech.ac.jp/)

## License

mruby/c is released under the Revised BSD License(aka 3-clause license).

## Related work

- Device classes for mruby/c (https://github.com/mrubyc/dev)
- Some sample programs that mainly control sensors. (https://github.com/mrubyc/devkit02/tree/main/samples)
