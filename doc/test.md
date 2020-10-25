# Test

mruby/c adopts mrubyc-test for unit testing.

Refer to https://github.com/mrubyc/mrubyc-test for further details.

## Prerequisite

You need Docker installed.
Information about Docker on https://docs.docker.com/get-docker/

## Setup

```
make setup_test
```

This target will build a Docker container image.
It may take a while.

## Test!

```
make test
```

You can specify a test file in this way:
```
make test file=test/array_test.rb
```

You can override environment variables for example:
```
CFLAGS=-DMRBC_USE_MATH=0 make test
```
