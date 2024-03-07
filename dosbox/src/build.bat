REM Where Turbo C++ is installed:
REM Set DRV to the drive
REM Set PTH to the directory

set DRV=f
set PTH=%DRV%:\language\turbocpp

cd %PTH%

tcc -I%DRV%:include -L%DRV%:lib -O -a -G -mc savefil.c
savefil.exe
move /Y savefil.dat zcndfil.dat ..\bin
del savefil.exe

tcc -I%DRV%:include -L%DRV%:lib -O -a -G -mc -c prgdat.c objrtn.c fldrtn.c
tcc -I%DRV%:include -L%DRV%:lib -O -B -a -G -mc -c ibmspc.c
tcc -I%DRV%:include -L%DRV%:lib -O -a -G -mc %DRV%:lib\graphics.lib rgntest.c prgdat.obj objrtn.obj fldrtn.obj ibmspc.obj
move /Y rgntest.exe ..\bin

tcc -I%DRV%:include -L%DRV%:lib -O -a -G -mc %DRV%:lib\graphics.lib testspc.c ibmspc.obj prgdat.obj
move /Y testspc.exe ..\bin

del *.obj
