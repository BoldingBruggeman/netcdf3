## This is a automake file, part of Unidata's netCDF package.
# Copyright 2006, see the COPYRIGHT file for more information.  

# This file builds F90 examples.

# $Id: Makefile.am,v 1.22 2008/04/04 20:23:49 dmh Exp $

# Tell the compiler where to find the .mod files. The .mod file is
# built, and therefore found in the build directory, not the source
# directory.
AM_FFLAGS = -I$(top_srcdir)/fortran -I$(top_srcdir)/f90	\
@MOD_FLAG@$(top_builddir)/f90
AM_FCFLAGS = -I$(top_srcdir)/fortran -I$(top_srcdir)/f90	\
@MOD_FLAG@$(top_builddir)/f90
LDADD = ../../fortran/libnetcdff.la

# Some fortran compilers think your files should not have an .f90
# extension! The value of FCFLAGS_f90 is set in the configure script,
# based on the fortran compiler.
AM_FCFLAGS += $(FCFLAGS_f90)
AM_F90FLAGS = $(FCFLAGS_f90)

# These are the example programs.
TESTPROGRAMS = simple_xy_wr simple_xy_rd sfc_pres_temp_wr	\
sfc_pres_temp_rd pres_temp_4D_wr pres_temp_4D_rd

TESTSCRIPTS = do_comps.sh

# Here's the source code for the programs.
simple_xy_wr_SOURCES = simple_xy_wr.f90
simple_xy_rd_SOURCES = simple_xy_rd.f90
sfc_pres_temp_wr_SOURCES = sfc_pres_temp_wr.f90
sfc_pres_temp_rd_SOURCES = sfc_pres_temp_rd.f90
pres_temp_4D_wr_SOURCES = pres_temp_4D_wr.f90
pres_temp_4D_rd_SOURCES = pres_temp_4D_rd.f90

# Cleaning up files created during the process.
CLEANFILES = simple_xy.nc sfc_pres_temp.nc pres_temp_4D.nc 
MAINTAINERCLEANFILES = Makefile.in

# Is the user building netCDF-4?
if USE_NETCDF4

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_FFLAGS += -I@HDF5DIR@/include
AM_FCFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS = -L@HDF5DIR@/lib
endif

# Since netCDF-4 is being built, link to the netcdf library in
# ../../libsrc4, also link to HDF5 and HDF5 high-level libraries, and
# find netcdf.h in ../../libsrc4.
AM_FFLAGS += -I$(top_srcdir)/libsrc4 
AM_FCFLAGS += -I$(top_srcdir)/libsrc4 
LDADD += ../../libsrc4/libnetcdf.la -lhdf5_hl -lhdf5

if EXTRA_EXAMPLE_TESTS
# This will cause netCDF-4 versions of the example programs to be
# built.
BUILT_SOURCES = nc4_simple_xy_wr.f90 nc4_sfc_pres_temp_wr.f90	\
nc4_pres_temp_4D_wr.f90

# How to build the example.
nc4_simple_xy_wr_SOURCES = nc4_simple_xy_wr.f90
nc4_sfc_pres_temp_wr_SOURCES = nc4_sfc_pres_temp_wr.f90
nc4_pres_temp_4D_wr_SOURCES = nc4_pres_temp_4D_wr.f90

# Add example to the tests run.
TESTPROGRAMS += nc4_simple_xy_wr nc4_sfc_pres_temp_wr	\
nc4_pres_temp_4D_wr

# Add this test script.
TESTSCRIPTS += run_nc4_comps.sh

# Make sure all the files created by our netcdf-4 tests get cleaned.
CLEANFILES += nc4_simple_xy_wr.f90 simple_xy.cdl			\
nc4_sfc_pres_temp_wr.f90 sfc_pres_temp.cdl nc4_pres_temp_4D_wr.f90	\
pres_temp_4D.cdl nc4_simple_xy.nc nc4_pres_temp_4D.nc			\
nc4_sfc_pres_temp.nc

endif #EXTRA_EXAMPLE_TESTS

TESTPROGRAMS += simple_xy_nc4_wr simple_xy_nc4_rd
simple_xy_nc4_wr_SOURCES = simple_xy_nc4_wr.f90
simple_xy_nc4_rd_SOURCES = simple_xy_nc4_rd.f90
CLEANFILES += simple_xy_nc4.nc

else # not USE_NETCDF4

# Without netcdf-4, go to ../../libsrc for the library and header.
AM_FFLAGS += -I$(top_srcdir)/libsrc
AM_FCFLAGS += -I$(top_srcdir)/libsrc

if BUILD_DAP
LDADD += ../../libnc-dap/libnc-dap.la 
else
LDADD += ../../libsrc/libnetcdf.la 
endif

endif # USE_NETCDF4

# This target tells how to build one of the netCDF-4 examples.
# Note old version used sed -i, but -i is not universally recognized
nc4_simple_xy_wr.f90:
	sed -e 's/nf90_clobber/nf90_hdf5/' simple_xy_wr.f90 \
	| sed -e 's/simple_xy/nc4_simple_xy/' > nc4_simple_xy_wr.f90

# This target tells how to build one of the netCDF-4 examples.
nc4_sfc_pres_temp_wr.f90:
	sed -e 's/nf90_clobber/nf90_hdf5/' sfc_pres_temp_wr.f90 \
	| sed -e 's/sfc_pres_temp/nc4_sfc_pres_temp/' > nc4_sfc_pres_temp_wr.f90

# This target tells how to build one of the netCDF-4 examples.
nc4_pres_temp_4D_wr.f90:
	sed -e 's/nf90_clobber/nf90_hdf5/' pres_temp_4D_wr.f90 \
	| sed -e 's/pres_temp_4D/nc4_pres_temp_4D/' > nc4_pres_temp_4D_wr.f90

# Build these test programs.
check_PROGRAMS = $(TESTPROGRAMS)

# Make sure the script which compares file outputs is included in the
# dist.
EXTRA_DIST = do_comps.sh run_nc4_comps.sh

# Run all test programs, plus the do_comps.sh script, which checks
# that all the output files are the same as the C example output
# files.
TESTS = $(TESTPROGRAMS) $(TESTSCRIPTS)

# There is a linking conflict here. Do we link with C++ and add
# the necessary fortran library or do we link with fortran and
# add the necessary C++ library.  The latter is likely to be
# more standard so we do that.
if BUILD_DAP
# Do not use the nodist_ dummy mechanism
LDADD += -lstdc++
endif
