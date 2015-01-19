WIN_LIBRARIES=-l Gdi32
WINDOWS_LIB=-L "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64" ${WIN_LIBRARIES}
SRC=src
OUT=output

mat:
	gcc ${SRC}/matrix.c -g -std=c99 -o matrix.exe

test: matrix.o
	gcc ${SRC}/3dtest.c ${OUT}/matrix.o -g -std=c99 -o 3dTest.exe

win: main.o
	gcc ${OUT}/main.o ${OUT}/matrix.o -g -o win64_test.exe ${WINDOWS_LIB}

main.o: matrix.o
	gcc -c ${SRC}/win64_layer.c -g -std=c99 -o ${OUT}/main.o

matrix.o:
	gcc -c ${SRC}/matrix.c -g -std=c99 -o ${OUT}/matrix.o