all: mmap_cp2

mmap_cp2: mmap_cp2.c
	gcc -g -pthread mmap_cp2.c -o mmap_cp2

clean: mmap_cp2
	rm mmap_cp2