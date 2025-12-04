#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define HASH_SIZE (1024)
#define HASH_MASK (HASH_SIZE - 1)

static inline uint64_t
pow10_u(int n)
{
    static const uint64_t table[20] = {
        1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL,
        100000ULL, 1000000ULL, 10000000ULL,
        100000000ULL, 1000000000ULL, 10000000000ULL,
        100000000000ULL, 1000000000000ULL,
        10000000000000ULL, 100000000000000ULL,
        1000000000000000ULL, 10000000000000000ULL,
        100000000000000000ULL, 1000000000000000000ULL,
        10000000000000000000ULL
    };
    return table[n];
}

static inline int
count_digits_u64(uint64_t x)
{
    static const uint64_t pow10_table[20] = {
        1ULL,
        10ULL,
        100ULL,
        1000ULL,
        10000ULL,
        100000ULL,
        1000000ULL,
        10000000ULL,
        100000000ULL,
        1000000000ULL,
        10000000000ULL,
        100000000000ULL,
        1000000000000ULL,
        10000000000000ULL,
        100000000000000ULL,
        1000000000000000ULL,
        10000000000000000ULL,
        100000000000000000ULL,
        1000000000000000000ULL,
        10000000000000000000ULL
    };

    int bits  = 64 - __builtin_clzll(x);
    int guess = (19 * bits) >> 6;  // ≈ floor(log2(x) * log10(2))

    return guess + (x >= pow10_table[guess]);
}

static inline double
get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

struct hash_set {
    uint64_t *keys;
    uint8_t  *occ;
    int size;
};

/* MurmurHash3 64-bit finalizer */
static inline uint64_t
hash_u64(uint64_t x)
{
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;

    return x;
}

static inline void
hash_set_init(struct hash_set *hs)
{
    hs->keys = calloc(HASH_SIZE, sizeof(uint64_t));
    hs->occ  = calloc(HASH_SIZE, sizeof(uint8_t));
    hs->size = 0;
}

static inline int
hash_set_insert(struct hash_set *hs, uint64_t key, uint64_t *running_sum)
{
    uint64_t h = hash_u64(key) & HASH_MASK;

    while (hs->occ[h]) {
        if (hs->keys[h] == key)
            return 0;

        h = (h + 1) & HASH_MASK;
    }

    hs->keys[h] = key;
    hs->occ[h] = 1;
    hs->size++;

    *running_sum += key;
    return 1;
}

static inline void
hash_set_free(struct hash_set *hs)
{
    free(hs->keys);
    free(hs->occ);
}

static void
collect_invalid_in_range(uint64_t start, uint64_t end, struct hash_set *hs, uint64_t *running_sum)
{
    int min_exp = count_digits_u64(start) - 1;
    int max_exp = count_digits_u64(end) - 1;

    for (int exp = min_exp; exp <= max_exp; exp++) {
        int digits = exp + 1;

        for (int rep = 2; rep <= digits; rep++) {
            if (digits % rep != 0)
                continue;

            int per_rep = digits / rep;
            uint64_t base = pow10_u(per_rep);

            uint64_t factor = 1;
#pragma unroll
            for (int i = 1; i < rep; i++)
                factor = factor * base + 1;

            uint64_t min_seq = start / factor;
            if (start % factor)
                min_seq++;

            uint64_t max_seq = end / factor;

            uint64_t seq_lo = pow10_u(per_rep - 1);
            uint64_t seq_hi = pow10_u(per_rep) - 1;

            if (min_seq < seq_lo)
                min_seq = seq_lo;
            if (max_seq > seq_hi)
                max_seq = seq_hi;
            if (min_seq > max_seq)
                continue;

            for (uint64_t seq = min_seq; seq <= max_seq; seq++) {
                uint64_t candidate = seq * factor;
                if (candidate >= start && candidate <= end) {
                    hash_set_insert(hs, candidate, running_sum);
                }
            }
        }
    }
}

int
main(int argc, char **argv)
{
    double start_time = get_time_ns();

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    if (getline(&line, &len, fp) == -1) {
        perror("Error reading file");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    double parse_time = get_time_ns();

    struct hash_set hs;
    hash_set_init(&hs);

    uint64_t total = 0;

    char *token = strtok(line, ",");
    while (token) {
        uint64_t start, end;
        if (sscanf(token, "%llu-%llu", &start, &end) == 2) {
            collect_invalid_in_range(start, end, &hs, &total);
        }
        token = strtok(NULL, ",");
    }

    free(line);

    double compute_time = get_time_ns();

    hash_set_free(&hs);

    double end_time = get_time_ns();

    printf("Sum of invalid IDs: %llu\n", total);

    printf("\nTiming breakdown:\n");
    printf("  File I/O:       %.0f ns (%.3f µs)\n",
           parse_time - start_time, (parse_time - start_time)/1000.0);
    printf("  Computation:    %.0f ns (%.3f µs)\n",
           compute_time - parse_time, (compute_time - parse_time)/1000.0);
    printf("  Total runtime:  %.0f ns (%.3f µs)\n",
           end_time - start_time, (end_time - start_time)/1000.0);

    return 0;
}
