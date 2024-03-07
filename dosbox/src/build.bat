cd f:\language\turbocpp

tcc -If:include -Lf:lib -O -a -G -mc savefil.c
savefil.exe
move /Y savefil.dat zcndfil.dat ..\bin
del savefil.exe

tcc -If:include -Lf:lib -O -a -G -mc -c prgdat.c objrtn.c fldrtn.c
tcc -If:include -Lf:lib -O -B -a -G -mc -c ibmspc.c
tcc -If:include -Lf:lib -O -a -G -mc f:lib\graphics.lib rgntest.c prgdat.obj objrtn.obj fldrtn.obj ibmspc.obj
move /Y rgntest.exe ..\bin

tcc -If:include -Lf:lib -O -a -G -mc f:lib\graphics.lib testspc.c ibmspc.obj prgdat.obj
move /Y testspc.exe ..\bin

del *.obj
