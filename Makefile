#
# mruby/c  Makefile
#
# Copyright (C) 2015-2018 Kyushu Institute of Technology.
# Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

MRUBY_VERSION = $(shell grep mruby_version .mrubycconfig | sed 's/mruby_version: *//')
CRUBY_VERSION = $(shell grep cruby_version .mrubycconfig | sed 's/cruby_version: *//')

all: mrubyc_lib mrubyc_bin


mrubyc_lib:
	cd mrblib ; $(MAKE) all
	cd src ; $(MAKE) all

mrubyc_bin:
	cd sample_c ; $(MAKE) all

clean:
	cd mrblib ; $(MAKE) clean
	cd src ; $(MAKE) clean
	cd sample_c ; $(MAKE) clean

package: clean
	@LANG=C ;\
	TARGET="mruby-c_`head -n1 Version`" ;\
	if [ -n "$$MRUBYC_VERSION" ] ;\
		then TARGET="mruby-c_$$MRUBYC_VERSION" ;\
	fi ;\
	echo Making \"$$TARGET.tgz\" ;\
	mkdir -p pkg/$$TARGET ;\
	cp -Rp src doc sample_c sample_ruby auto_test README.md Makefile pkg/$$TARGET ;\
	cd pkg ;\
	tar cfz ../$$TARGET.tgz $$TARGET ;\
	cd .. ;\
	rm -Rf pkg ;\
	echo Done.

.PHONY: test setup_test
test:
	RBENV_VERSION=$(CRUBY_VERSION) \
	CFLAGS="-DMRBC_USE_MATH=1 -DMAX_SYMBOLS_COUNT=500 $(CFLAGS)" \
	bundle exec mrubyc-test test --every=100 $(file)

setup_test:
	@echo MRUBY_VERSION=$(MRUBY_VERSION)
	@echo CRUBY_VERSION=$(CRUBY_VERSION)
	rbenv install --skip-existing $(MRUBY_VERSION) ;\
	rbenv local $(MRUBY_VERSION) ;\
	rbenv install --skip-existing $(CRUBY_VERSION) ;\
	RBENV_VERSION=$(CRUBY_VERSION) gem install bundler ;\
	RBENV_VERSION=$(CRUBY_VERSION) bundle install
