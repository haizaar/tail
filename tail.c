#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#define LINES_TO_PRINT 10
#define SEEK_CHUNK 4096

int tail(char* file) {
	FILE *f;
	long curpos = 0;
	char chunk[SEEK_CHUNK];
	int bytes_read = 0;
	int linecount = 0;
	int i;
    // to print 10 lines we need to search up to  11th '\n' in file
    int lines_to_print = LINES_TO_PRINT + 1;

	f = fopen(file, "r");
	if (f == NULL) {
		fprintf(stderr, "Failed to open %s for reading: %s\n", file, strerror(errno));
		return EXIT_FAILURE;
	}	

	// fast forward
	if (fseek(f, 0, SEEK_END)) {
		fprintf(stderr, "Failed to seek file %s: %s. Is it a file??\n", file, strerror(errno));
		return EXIT_FAILURE;
	}
	
	curpos = ftell(f);

	while ((linecount < lines_to_print) && (curpos > 0)) {
		if (!fseek(f, -SEEK_CHUNK, SEEK_CUR)) {
            bytes_read = fread(chunk, 1, SEEK_CHUNK,  f);
        } else {
            // Begning of file reached
            rewind(f);
            bytes_read = fread(chunk, 1, curpos,  f);
        }
		for (i = bytes_read-1; i >= 0; i--) {
			if (chunk[i] == '\n') {
				linecount++;
				if (linecount == lines_to_print) {
					break;
				}
			}
            curpos--;
		}
	}
		
	// print file
    fseek(f, curpos, SEEK_SET);
	while ( (bytes_read = fread(chunk, 1, SEEK_CHUNK,  f)) ) {
		fwrite(chunk, 1, bytes_read, stdout);
	}
	
	return EXIT_SUCCESS;
}
	

int main(int argc, char* argv[]) {
	char *file;
    struct stat st;

	if (!(argc > 1)) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	file = argv[1];
	if (lstat(file, &st)) {
		fprintf(stderr,"Failed to stat %s: %s\n", file, strerror(errno));
		return EXIT_FAILURE;
	}	
	
	if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s is not a regular file\n", file);
        return EXIT_FAILURE;
    }
	
	return tail(file);
}
