## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This is the main automake file for netCDF. It builds the different
# netcdf directories. Not all directories are built, depending on the
# options selected during configure.

# $Id: Makefile.am,v 1.95 2008/06/27 13:15:13 ed Exp $

# These files get added to the distribution.
EXTRA_DIST = README COPYRIGHT RELEASE_NOTES INSTALL

# This will go to the man directory and build, if necessary, the
# netcdf-install.txt file, which will be copied into the main
# directory, renamed "INSTALL".
INSTALL:
	cd man; make netcdf-install.info netcdf-install.txt
	cp man/netcdf-install.txt INSTALL

# If the user wants the F77 API this will cause it to be built and
# tested.
if BUILD_F77
F77_DIR = fortran
F77_TEST = nf_test
endif

# Does the user want C API?
if BUILD_C

LIBSRC_DIR = libsrc
NC_TEST_DIR = nc_test

# Does the user want to build the V2 API?
if BUILD_V2
V2_TEST = nctest
endif

# Does the user want C++ API?
if BUILD_CXX
CXX_DIR = cxx
endif

# Does the user want to build ncgen/ncdump?
if BUILD_UTILITIES
NCGEN = ncgen
NCDUMP = ncdump
endif

# Does the user want to build the examples?
if BUILD_EXAMPLES
EXAMPLES = examples
endif

# Is the user building netCDF-4?
if USE_NETCDF4
LIBSRC4_DIR = libsrc4
NC_TEST4 = nc_test4
MAN = man4
else
MAN = man
endif

endif # BUILD_C

# Does the user want F90 API?
if BUILD_F90
F90_DIR = f90
endif

if INSTALL_DOCS
MAN = man
endif

if BUILD_CXX4
CXX4 = cxx4
endif

# if BUILD_DAP
# DAPDIR=libdap
# NCDAPDIR=libnc-dap
# endif

# This is the list of subdirs for which Makefiles will be constructed
# and run. ncgen must come before ncdump and cxx, because their tests
# depend on it. nf_test depends upon ncgen as well.
# DAP dirs must be built right after LIBSRC_DIR
# SUBDIRS = $(F90_DIR) $(F77_DIR) $(LIBSRC_DIR) \
# $(DAPDIR) $(NCDAPDIR) $(LIBSRC4_DIR)	\
# $(V2_TEST) $(NCGEN) $(NCDUMP) $(F77_TEST) ${CXX_DIR} $(CXX4)	\
# $(NC_TEST_DIR) $(NC_TEST4) $(MAN) $(EXAMPLES) win32 
SUBDIRS = $(F90_DIR) $(F77_DIR) $(LIBSRC_DIR) $(LIBSRC4_DIR)	\
$(V2_TEST) $(NCGEN) $(NCDUMP) $(F77_TEST) ${CXX_DIR} $(CXX4)	\
$(NC_TEST_DIR) $(NC_TEST4) $(MAN) $(EXAMPLES) win32

# make test should do the same as make check.
test: check

# Remove this generated file, for a distclean
DISTCLEANFILES = VERSION comps.txt

# What needs to go in the binrary dist?
BINFILES = README_BINARIES.txt
if BUILD_C
BINFILES += include/netcdf.h share/man/man3/netcdf.3 lib/libnetcdf.a
endif

if BUILD_UTILITIES
BINFILES += bin/ncgen$(EXEEXT) bin/ncdump$(EXEEXT)	\
share/man/man1/ncgen.1 share/man/man1/ncdump.1
endif

if BUILD_F77
BINFILES += include/netcdf.inc share/man/man3/netcdf_f77.3 
if BUILD_SEPARATE_FORTRAN
BINFILES += lib/libnetcdff.a
endif
endif

if BUILD_F90
if UPPER_CASE_MOD
BINFILES += include/NETCDF.mod include/TYPESIZES.mod
else
BINFILES += include/netcdf.mod include/typesizes.mod
endif
BINFILES += share/man/man3/netcdf_f90.3
endif

if BUILD_CXX
BINFILES += include/netcdf.hh include/ncvalues.h  include/netcdfcpp.h lib/libnetcdf_c++.a 
endif

# At Unidata, package up binaries.
ftpbin: install
	echo "Getting binaries from ${prefix}"
	ls ${prefix}
	echo "These are the binaries for netCDF @PACKAGE_VERSION@." > ${prefix}/README_BINARIES.txt
	echo "For this build: CC=$(CC) CXX=$(CXX) FC=$(FC) F77=$(F77) $F90=$(F90)" >> ${prefix}/README_BINARIES.txt
	echo "CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS)" >> ${prefix}/README_BINARIES.txt
	echo "FCFLAGS=$(FCFLAGS) F77FLAGS=$(F77FLAGS) $F90FLAGS=$(F90FLAGS)" >> ${prefix}/README_BINARIES.txt
	tar cf @BINFILE_NAME@ -C ${prefix} ${BINFILES}
	gzip -f @BINFILE_NAME@

# This target checks that the install got all the files it was
# supposed to. NetCDF users don't have to run this target, but it is
# run nightly by the automatic netCDF testing at Unidata, to ensure
# that all the expected output was installed. It's ugly, but very
# helpful as a final test!
check_install: install
if BUILD_C
	[ -f ${includedir}/netcdf.h ] || (echo "Missing!" && exit 1)
	[ -f ${mandir}/man3/netcdf.3 ] || (echo "Missing!" && exit 1)
	[ -f ${libdir}/libnetcdf.a ] || (echo "Missing!" && exit 1)
if BUILD_SHARED
	[ -f ${libdir}/libnetcdf.@SHAREEXT@ ] || (echo "Missing!" && exit 1)
endif
endif
if BUILD_UTILITIES
	[ -f ${bindir}/ncgen ] || (echo "Missing!" && exit 1)
	[ -f ${bindir}/ncdump ] || (echo "Missing!" && exit 1)
	[ -f ${mandir}/man1/ncgen.1 ] || (echo "Missing!" && exit 1)
	[ -f ${mandir}/man1/ncdump.1 ] || (echo "Missing!" && exit 1)
endif
if BUILD_F77
	[ -f ${includedir}/netcdf.inc ] || (echo "Missing!" && exit 1)
	[ -f ${mandir}/man3/netcdf_f77.3 ] || (echo "Missing!" && exit 1)
if BUILD_SEPARATE_FORTRAN
	[ -f ${libdir}/libnetcdff.a ] || (echo "Missing!" && exit 1)
if BUILD_SHARED
	[ -f ${libdir}/libnetcdff.@SHAREEXT@ ] || (echo "Missing!" && exit 1)
endif
endif
endif
if BUILD_F90
if UPPER_CASE_MOD
	[ -f ${includedir}/NETCDF.mod ] || (echo "Missing!" && exit 1)
	[ -f ${includedir}/TYPESIZES.mod ] || (echo "Missing!" && exit 1)
else
	[ -f ${includedir}/netcdf.mod ] || (echo "Missing!" && exit 1)
	[ -f ${includedir}/typesizes.mod ] || (echo "Missing!" && exit 1)
endif
	[ -f ${mandir}/man3/netcdf_f90.3 ] || (echo "Missing!" && exit 1)
endif
if BUILD_CXX
	[ -f ${includedir}/netcdf.hh ] || (echo "Missing!" && exit 1)
	[ -f ${includedir}/ncvalues.h ] || (echo "Missing!" && exit 1)
	[ -f ${includedir}/netcdfcpp.h ] || (echo "Missing!" && exit 1)
	[ -f ${libdir}/libnetcdf_c++.a ] || (echo "Missing!" && exit 1)
if BUILD_SHARED
	[ -f ${libdir}/libnetcdf_c++.@SHAREEXT@ ] || (echo "Missing!" && exit 1)
endif
endif
if INSTALL_DOCS
	[ -f ${infodir}/netcdf.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf.txt ] || (echo "Missing!" && exit 1)
	[ -f ${infodir}/netcdf-tutorial.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-tutorial.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-tutorial.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-tutorial.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-tutorial.txt ] || (echo "Missing!" && exit 1)
	[ -f ${infodir}/netcdf-install.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-install.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-install.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-install.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-install.txt ] || (echo "Missing!" && exit 1)
if BUILD_C
	[ -f ${infodir}/netcdf-c.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-c.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-c.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-c.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-c.txt ] || (echo "Missing!" && exit 1)
endif
if BUILD_F77
	[ -f ${infodir}/netcdf-f77.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f77.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f77.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f77.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f77.txt ] || (echo "Missing!" && exit 1)
endif
if BUILD_F90
	[ -f ${infodir}/netcdf-f90.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f90.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f90.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f90.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-f90.txt ] || (echo "Missing!" && exit 1)
endif
if BUILD_CXX
	[ -f ${infodir}/netcdf-cxx.info ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-cxx.html ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-cxx.pdf ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-cxx.ps ] || (echo "Missing!" && exit 1)
	[ -f ${docdir}/netcdf-cxx.txt ] || (echo "Missing!" && exit 1)
endif
endif # INSTALL_DOCS
	echo " *** SUCCESS: all files were found!"

install-data-hook:
	@echo ''
	@echo '+-------------------------------------------------------------+'
	@echo '| Congratulations! You have successfully installed netCDF!    |'
	@echo '|                                                             |'
	@echo '| CAUTION:                                                    |'
	@echo '|                                                             |'
	@echo '| If you have not already run "make check", then we strongly  |'
	@echo '| recommend you do so. It does not take very long.            |'
	@echo '|                                                             |'
	@echo '| Before using netCDF to store important data, test your      |'
	@echo '| build with "make check".                                    |'
	@echo '|                                                             |'
	@echo '| NetCDF is tested nightly on many platforms at Unidata       |'
	@echo '| but your platform is probably different in some ways.       |'
	@echo '|                                                             |'
	@echo '| If any tests fail, please see the netCDF web site:          |'
	@echo '| http://www.unidata.ucar.edu/software/netcdf/                |'
	@echo '|                                                             |'
	@echo '| NetCDF is developed and maintained at the Unidata Program   |'
	@echo '| Center. Unidata provides a broad array of data and software |'  
	@echo '| tools for use in geoscience education and research.         |'
	@echo '| http://www.unidata.ucar.edu                                 |'
	@echo '+-------------------------------------------------------------+'
	@echo ''

# If building the dll on windows, with MingW, this extra target
# packages the results.
win32_bin:
	@echo 'This netCDF DLL was generated using MingW.' > README_DLL.txt
	@echo '' >> README_DLL.txt
	@echo './configure --enable-dll --enable-shared --disable-separate-fortram --disable-cxx --disable-f90' >> README_DLL.txt
	@echo 'To use the DLL from C, include netcdf.h and set pre-processor macro DLL_NETCDF.' >> README_DLL.txt
	@echo 'To use the DLL from Fortran, include netcdf.inc.' >> README_DLL.txt
	cd libsrc/.libs
	lib /machine:i386 /def:libnetcdf.def
	ls
	cd ../..
	cp libsrc/.libs/netcdf.lib .
	cp libsrc/netcdf.h .
	cp fortran/netcdf.inc .
	cp ncgen/.libs/ncgen.exe .
	cp ncdump/.libs/ncdump.exe .
	cp libsrc/.libs/libnetcdf-4.dll .
	cp libsrc/.libs/libnetcdf.a .
	cp libsrc/.libs/libnetcdf.dll.a .
	tar -cf win32_dll_$(VERSION).tar netcdf.h libnetcdf-4.dll README_DLL.txt netcdf.inc ncgen.exe ncdump.exe netcdf.lib
	gzip -f win32_dll_$(VERSION).tar

# If building the dll on windows, with Cygwin, using the -mno-cygwin
# option, this extra target packages the results.
win32_bin_nocygwin:
	@echo 'This netCDF DLL was generated using MingW.' > README_DLL.txt
	@echo '' >> README_DLL.txt
	@echo './configure --enable-dll --enable-shared --disable-separate-fortram --disable-cxx --disable-f90' >> README_DLL.txt
	@echo 'To use the DLL from C, include netcdf.h and set pre-processor macro DLL_NETCDF.' >> README_DLL.txt
	@echo 'To use the DLL from Fortran, include netcdf.inc.' >> README_DLL.txt
	cp libsrc/netcdf.h .
	cp fortran/netcdf.inc .
	cp ncgen/.libs/ncgen.exe .
	cp ncdump/.libs/ncdump.exe .
	cp libsrc/.libs/cygnetcdf-4.dll libnetcdf-4.dll
	cp libsrc/.libs/libnetcdf.a .
	cp libsrc/.libs/libnetcdf.dll.a .
	tar -cf win32_dll_$(VERSION).tar netcdf.h libnetcdf-4.dll README_DLL.txt netcdf.inc ncgen.exe ncdump.exe
	gzip -f win32_dll_$(VERSION).tar

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 libtool.m4 ltoptions.m4 ltversions.m4 ltsugar.m4 lt~obsolete.m4 libtool 