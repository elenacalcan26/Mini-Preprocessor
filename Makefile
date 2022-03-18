CFLAGS = /D_CRT_SECURE_NO_DEPRECATE /W4 /EHsc /Za /MD

build: main.obj hashmap.obj
	cl.exe $(CFLAGS) /Feso-cpp.exe main.obj hashmap.obj

main.obj: main.c
	cl.exe $(CFLAGS) /Fomain.obj /c main.c

hashmap.obj: hashmap.c
	cl.exe $(CFLAGS) /Fohashmap.obj /c hashmap.c

clean:
	del *.obj *.exe