CC=clang++

loglmake: main.cpp
	$(CC) -Wall -g -I./Externals/include -L./Externals/library ./Externals/library/libglfw.3.3.dylib main.cpp glad.c -o app -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation -Wno-deprecated
