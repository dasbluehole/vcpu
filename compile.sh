gcc -g -c cpu.c
gcc -g -c main.c
gcc cpu.o main.o -o vcpu
rm -f *.o
