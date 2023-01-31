# Mega-present
This is a presentation tool for Sega Mega Drive. It is a fork (of a fork) of mruby/c.

This is still work in progress, and experimental.

## Demo

This is me presenting this app at RubyKaigi 2022, on a Sega Mega Drive unit.

<iframe width="560" height="315" src="https://www.youtube.com/embed/JuKYJ-G8heU" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

## Build

NOTE: You will get this warning message even when the build is successful:

```
/opt/gendev/sgdk/bin/nm: liblto_plugin-0.dll: cannot open shared object file: No such file or directory
```

### Using Docker

You can use the command:

```
docker run -i -t --rm --platform linux/amd64 -v $(pwd):/mnt -w /mnt yujiyokoo/gendev_mrubyc bash -c 'mrbc -B mrbsrc src/game.rb && make -f /opt/gendev/sgdk/mkfiles/Makefile.rom clean all'
```

You can also use the local Dockerfile to build instead of pulling with the above command:
```
docker build -t yujiyokoo/gendev_mrubyc --platform linux/amd64 .
```

### Using local environment

UPDATE: I no longer use this method. The Docker method above may work better.

1. Ensure gendev is installed
I have used [Gendev 0.71](https://github.com/kubilus1/gendev/releases/tag/0.7.1) on a Debian sid in 2022. The .deb package installs under /opt/gendev.

2. Patch gendev files (use the patch file included in this repo)
Change the command accordingly, but in my case, I patched gendev dir with:

```
> sudo cp gendev-mega-mrubyc.patch /opt
> cd /opt
> sudo patch -p1 < ./gendev-mega-mrubyc.patch
```

3. Run build command
```
export GENDEV=/opt/gendev
mrbc -B mrbsrc src/game.rb && make -f $GENDEV/sgdk/mkfiles/Makefile.rom clean all
```

## Execute
After the above building step, you should end up with `out/rom.bin`, which you can use with most emulators.
If you have a way of runnig your own code on the real Mega Drive unit, it should work there too. I use Mega EverDrive X7 and it works for me.


## License

This fork of mruby/c is released under the same licence as the orginal - Revised BSD License (aka 3-clause license).

The RubyKaigi logo and Ninjas have been imported and edited from https://rubykaigi.org/2022/novelties/

