gcc -Wall -g  -c main.c -o main.o
gcc -o main.exe main.o  -O2  -lopengl32 -lglu32 -lgdi32
main.exe