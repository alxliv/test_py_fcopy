#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define INPUT       "E:\\tmp\\big.bin"
#define OUTPUT      "E:\\tmp\\big_out.bin"
#define CHUNK_BYTES (64ULL * 1024 * 1024)   /* 64 MiB */

int main(void) {
    uint8_t *buf = (uint8_t *)malloc(CHUNK_BYTES);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    FILE *f_in = fopen(INPUT, "rb");
    if (!f_in) {
        fprintf(stderr, "fopen input failed: %s\n", INPUT);
        free(buf);
        return 1;
    }
    FILE *f_out = fopen(OUTPUT, "wb");
    if (!f_out) {
        fprintf(stderr, "fopen output failed: %s\n", OUTPUT);
        fclose(f_in);
        free(buf);
        return 1;
    }

    double read_time = 0.0;
    double write_time = 0.0;
    unsigned long long total = 0;

    clock_t start_total = clock();
    while (1) {
        clock_t t = clock();
        size_t n = fread(buf, 1, CHUNK_BYTES, f_in);
        read_time += (double)(clock() - t) / CLOCKS_PER_SEC;
        if (n == 0) {
            break;
        }

        t = clock();
        fwrite(buf, 1, n, f_out);
        write_time += (double)(clock() - t) / CLOCKS_PER_SEC;

        total += n;
    }
    double elapsed = (double)(clock() - start_total) / CLOCKS_PER_SEC;

    fclose(f_in);
    fclose(f_out);
    free(buf);

    double total_mb = (double)total / (1024.0 * 1024.0);
    printf("Copied %llu bytes in %.2fs\n", total, elapsed);
    printf("  read:  %.2fs (%.1f MB/s)\n", read_time, total_mb / read_time);
    printf("  write: %.2fs (%.1f MB/s)\n", write_time, total_mb / write_time);
    return 0;
}
