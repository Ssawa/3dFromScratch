WIN_LIBRARIES=-l user32 -l gdi32
WINDOWS_LIB=-L "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64" ${WIN_LIBRARIES}

lib:
	gcc src/3dtest.c -g -std=c99 -o 3dTest.exe

win:
	gcc src/win64_layer.c -g -std=c99 -o win64_test.exe ${WINDOWS_LIB}