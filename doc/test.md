# Test

mruby/c adopts mrubyc-test for unit testing.

Refer to https://github.com/mrubyc/mrubyc-test for further details.

## Prerequisite

You need rbenv installed.
If you still don't have it, installation commands are like this (Ubuntu + Bash):

```
git clone https://github.com/rbenv/rbenv.git ~/.rbenv
echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bash_profile
echo 'eval "$(rbenv init -)"' >> ~/.bash_profile
. ~/.bash_profile
```

For more information about rbenv on https://github.com/rbenv/rbenv

## Setup

```
make setup_test
```

This target will install both CRuby and mruby which are specified in `.mrubycconfig` .
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
