#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OUTPUT      "E:\\tmp\\big.bin"
#define TOTAL_BYTES (6ULL * 1024 * 1024 * 1024)   /* 3 GiB */
#define CHUNK_BYTES (64ULL * 1024 * 1024)         /* 64 MiB */

static uint64_t rng_state = 0x123456789ABCDEF0ULL;

static uint64_t xorshift64(void) {
    uint64_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    rng_state = x;
    return x;
}

static void fill_random(uint8_t *buf, size_t n) {
    size_t i = 0;
    while (i + 8 <= n) {
        uint64_t r = xorshift64();
        memcpy(buf + i, &r, 8);
        i += 8;
    }
    while (i < n) {
        buf[i++] = (uint8_t)xorshift64();
    }
}

int main(void) {
    uint8_t *buf = (uint8_t *)malloc(CHUNK_BYTES);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    FILE *f = fopen(OUTPUT, "wb");
    if (!f) {
        fprintf(stderr, "fopen failed: %s\n", OUTPUT);
        free(buf);
        return 1;
    }

    clock_t start = clock();

    unsigned long long written = 0;
    while (written < TOTAL_BYTES) {
        size_t n = CHUNK_BYTES;
        if (TOTAL_BYTES - written < (unsigned long long)n) {
            n = (size_t)(TOTAL_BYTES - written);
        }
        fill_random(buf, n);
        fwrite(buf, 1, n, f);
        written += n;
    }

    fclose(f);
    free(buf);

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
    double written_mb = (double)written / (1024.0 * 1024.0);
    double mb_per_s = written_mb / elapsed;
    printf("Wrote %llu bytes to %s in %.1fs (%.1f MB/s)\n",
           written, OUTPUT, elapsed, mb_per_s);
    return 0;
}
