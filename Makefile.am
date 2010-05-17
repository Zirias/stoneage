CLEANFILES = autoscan.log configure.scan
MAINTAINERCLEANFILES = Makefile.in configure

vcs-clean: maintainer-clean
	rm -fr autom4te.cache
	rm -fr doxy
	rm -f aclocal.m4 compile depcomp install-sh missing
.PHONY: vcs-clean

SUBDIRS = src gfx

