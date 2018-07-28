mkdir outgtk
gcc -std=c++11 -Ibase -Igtk -Ilua-5.3.4/src -o outgtk/gastonaGtk base/EvaLayoutManager.cpp gtk/mainGtk.cpp outgtk/liblua.a outgtk/shellmain.o outgtk/sqlite3.o `pkg-config --cflags gtk+-2.0 --libs glib-2.0`  -lstdc++ -lm -ldl
