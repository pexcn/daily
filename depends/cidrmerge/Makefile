#Compiler command
CC=gcc

#Usual gcc flags
CC_FLAGS=-O3 -fPIC -g -c -Wall -W
#Enforce ansi compatibility
CC_FLAGS+= -ansi -pedantic
#Pack struct: this will reduce significantly memory usage (- 30%), but some platform cannot handle not aligned to word integers (SIGBUS will be generated in this case). It can cause incompatibility with code compiled without this flag too and program to be slower in some cases, it needs test on target system.
#CC_FLAGS+=-fpack-struct
#cpu dependant, configure accordingly to final running machine. It can increase significantly performances
#CC_FLAGS+=-march=native
#CC_FLAGS+=-march=nocona
#Profiling
#CC_FLAGS+=-pg

#standard link flags
LINK_FLAGS=
#Linker profiling flags
#LINK_FLAGS+=-pg

#Standard defines
STD_DEFINES=-D OPTIMIZED_SORT -D INLINE -D CHAR_PREFIX
#timestamp.out generation
#STD_DEFINES+=-D TIMESTAMP
#I/O definition: UNIX_IO defined, read/write used. If not, fread/fwrite is used
#STD_DEFINES+=-D UNIX_IO

# debug defines. For program debugging only.
#DEBUG_DEFINES= -D LIBRARY_DEBUG_FULL -D LIBRARY_DEBUG -D INPUT_DEBUG -D OUTPUT_VERBOSITY
#DEBUG_DEFINES+=-D LIBRARY_DEBUG
#DEBUG_DEFINES+=-D LIBRARY_DEBUG_FULL
#DEBUG_DEFINES+=-D OUTPUT_VERBOSITY
#DEBUG_DEFINES+=-D INPUT_DEBUG

all :   cidrmerge

lib:			liboptimize.so.1.5.3

cidrmerge :		cidrmerge.o cidrmerge-lib.o
			${CC} ${LINK_FLAGS} -o cidrmerge cidrmerge.o cidrmerge-lib.o

cidrmerge.o :	cidrmerge.c cidrmerge.h
			${CC} ${STD_DEFINES} ${DEBUG_DEFINES} ${CC_FLAGS} -c cidrmerge.c

cidrmerge-lib.o : 	cidrmerge-lib.c cidrmerge.h
			${CC} ${STD_DEFINES} ${DEBUG_DEFINES} ${CC_FLAGS} -c cidrmerge-lib.c

liboptimize.so.1.5.3 :	cidrmerge-lib.o
			${CC} -shared -Wl,-soname,liboptimize.so.1 -o liboptimize.so.1.5.3 cidrmerge-lib.o -lc -ldl
			ln -fs liboptimize.so.1.5.3 liboptimize.so.1
			ln -fs liboptimize.so.1.5.3 liboptimize.a

clean :
	rm -f *.o core
	rm -f cidrmerge liboptimize.so.1.5.3 liboptimize.so.1 liboptimize.a
	rm -f timestamp.out gmon.out

