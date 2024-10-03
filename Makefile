# Makefile

# Target to compile Myfile.c
Myfile: Myfile.c
	gcc Myfile.c -D_SVID_SOURCE -D_GNU_SOURCE -Wall -std=c99 -Werror=cpp -pedantic -o Myfile

# Target to compile shm_processes.c
shm_proc: shm_processes.c
	gcc shm_processes.c -D_SVID_SOURCE -D_GNU_SOURCE -Wall -std=c99 -Werror=cpp -pedantic -o shm_proc

# Clean target to remove generated files
clean:
	rm -f Myfile shm_proc
