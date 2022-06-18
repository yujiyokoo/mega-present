# Meaa-mruby/c
This is a fork of mruby/c to port mruby/c to Sega Megadrive.

This README focuses on information specific to this fork.
For information that relates to the original mruby/c, please refer to the [original mruby/c](https://github.com/mrubyc/mrubyc) project.

## Build

1. Ensure gendev is installed and patched
I have used [Gendev 0.71](https://github.com/kubilus1/gendev/releases/tag/0.7.1) on a Debian machine. The .deb package installs under /opt/gendev. Change the command accordingly, but in my case, I patched gendev dir with:

```
sudo su
cp gendev-mega-mrubyc.patch /opt
cd /opt
patch -p1 < ./gendev-mega-mrubyc.patch
```

2. Run build command
```
export GENDEV=/opt/gendev
mrbc -B mrbsrc src/game.rb && make -f $GENDEV/sgdk/mkfiles/Makefile.rom clean all
```

## Execute
Afetr the above building step, you should end up with `out/rom.bin`, which you can use with most emulators.
If you have a way of runnig your own code on the real Megadrive unit, it should work there too. I use Mega EverDrive X7 and it works for me.


## License

This fork of mruby/c is released under the same licence as the orginal - Revised BSD License(aka 3-clause license).
