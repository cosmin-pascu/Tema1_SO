build: so-cpp.obj func_define.obj parseArguments.obj myHashMap.obj
	cl /Zi /MD /Wall /Feso-cpp.exe so-cpp.obj func_define.obj parseArguments.obj myHashMap.obj
	
so-cpp.obj: so-cpp.c
	cl /Zi /MD /c so-cpp.c
	
func_define.obj: func_define.c
	cl /Zi /MD /c func_define.c

parseArguments.obj: parseArguments.c
	cl /Zi /MD /c parseArguments.c

myHashMap.obj: myHashMap.c
	cl /Zi /MD /c myHashMap.c

clean:
	del *.obj so-cpp.exe