CC=gcc
% :: %.c
	$(CC) -g -Wall -Wvla -Werror $< -o $@

all: encode decode test_read test_write

test_read : test_read.c read_ppm.c write_ppm.c
	$(CC) -g -Wall -Wvla -Werror test_read.c read_ppm.c write_ppm.c -o $@ -lm

test_write : test_write.c read_ppm.c write_ppm.c
	$(CC) -g -Wall -Wvla -Werror test_write.c read_ppm.c write_ppm.c -o $@ -lm

decode : decode.c read_ppm.c write_ppm.c
	$(CC) -g -Wall -Wvla -Werror decode.c read_ppm.c write_ppm.c -o $@ -lm

encode : encode.c read_ppm.c write_ppm.c
	$(CC) -g -Wall -Wvla -Werror encode.c read_ppm.c write_ppm.c -o $@ -lm
