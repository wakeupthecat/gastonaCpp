mkdir outgtk
cd sqlite
gcc -c -o ../outgtk/shellmain.o shellmain.c
gcc -c -o ../outgtk/sqlite3.o sqlite3.c
cd ..

cd lua-5.3.4/src
gcc -c -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c
gcc -c -std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c
ar rcs ../../outgtk/liblua.a  lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o
rm *.o
cd ../..

ls -la outgtk
