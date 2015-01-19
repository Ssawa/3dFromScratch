WIN_LIBRARIES=-l Gdi32
WINDOWS_LIB=-L "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64" ${WIN_LIBRARIES}

mat:
	gcc src/matrix.c -g -std=c99 -o matrix.exe

lib:
	gcc src/3dtest.c -g -std=c99 -o 3dTest.exe

win: main.o
	gcc output/main.o output/matrix.o -o win64_test.exe ${WINDOWS_LIB}

main.o: src/win64_layer.c matrix.o
	gcc -c src/win64_layer.c -g -std=c99 -o output/main.o

matrix.o: src/matrix.c
	gcc -c src/matrix.c -g -std=c99 -o output/matrix.o