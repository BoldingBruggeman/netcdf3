#!/bin/sh
# This shell script runs the ncdump tests.
# $Id: run_tests.sh,v 1.16 2008/04/23 16:31:27 ed Exp $

set -e
echo ""
echo "*** Testing ncgen and ncdump using some test CDL files."
echo "*** creating tst_small.nc from ref_tst_small.cdl..."
../ncgen/ncgen -o tst_small.nc $srcdir/ref_tst_small.cdl
echo "*** creating tst_small.cdl from tst_small.nc..."
./ncdump tst_small.nc > tst_small.cdl
diff -w tst_small.cdl $srcdir/ref_tst_small.cdl

echo "*** creating test0.nc from test0.cdl..."
../ncgen/ncgen -b $srcdir/test0.cdl
echo "*** creating test1.cdl from test0.nc..."
./ncdump -n test1 test0.nc > test1.cdl
echo "*** creating test1.nc from test1.cdl..."
../ncgen/ncgen -b test1.cdl
echo "*** creating test2.cdl from test1.nc..."
./ncdump test1.nc > test2.cdl
echo "*** checking that test1.cdl and test2.nc are the same..."
diff -w test1.cdl test2.cdl
echo "*** All tests of ncgen and ncdump using test0.cdl passed!"
exit 0
