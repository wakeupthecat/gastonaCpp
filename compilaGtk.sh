mkdir outgtk
g++ -std=c++11 -fpermissive -static -Ibase -Igtk -Ilua-5.3.4\src -o outgtk/gastonaW gtk/mainGtk.cpp outgtk/liblua.a outgtk/shellmain.o outgtk/sqlite3.o -lm -lstdc++
