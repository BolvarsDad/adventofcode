/* 
 *  Big thanks to CordlessCoder for informing me about this approach.
 *  https://github.com/CordlessCoder/AoC2025/blob/main/src/bin/02.rs
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CANDIDATES 100000

static uint64_t pow10_u(int n) {
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

double
get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000.0 + ts.tv_nsec;
}

int
compare_uint64(const void *a, const void *b)
{
    uint64_t ua = *(const uint64_t *)a;
    uint64_t ub = *(const uint64_t *)b;

    if (ua < ub) return -1;
    if (ua > ub) return 1;

    return 0;
}

void
collect_invalid_in_range(uint64_t start, uint64_t end, uint64_t *candidates, int *count)
{
    int min_exp = (int)log10(start);
    int max_exp = (int)log10(end);

    for (int exp = min_exp; exp <= max_exp; exp++) {
        int digits = exp + 1;

        for (int rep = 2; rep <= digits; rep++) {
            if (digits % rep != 0)
                continue;

            int per_rep = digits / rep;
            uint64_t base = pow10_u(per_rep);

            uint64_t factor = 1;
            for (int i = 1; i < rep; i++) {
                factor = factor * base + 1;
            }

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
                    candidates[(*count)++] = candidate;
                }
            }
        }
    }
}


int main(int argc, char **argv) {
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

    uint64_t candidates[MAX_CANDIDATES];
    int cand_count = 0;

    char *token = strtok(line, ",");

    while (token != NULL) {
        uint64_t start, end;

        if (sscanf(token, "%llu-%llu", &start, &end) == 2) {
            collect_invalid_in_range(start, end, candidates, &cand_count);
        }

        token = strtok(NULL, ",");
    }

    free(line);

    double collect_time = get_time_ns();

    // Sort all candidates
    qsort(candidates, cand_count, sizeof(uint64_t), compare_uint64);

    double sort_time = get_time_ns();

    // Sum while deduplicating
    uint64_t total = 0;
    for (int i = 0; i < cand_count; i++) {
        if (i == 0 || candidates[i] != candidates[i-1]) {
            total += candidates[i];
        }
    }

    double end_time = get_time_ns();

    printf("Sum of invalid IDs: %llu\n", total);
    printf("\nTiming breakdown:\n");
    printf("  File I/O:        %.0f ns (%.3f µs)\n",
           parse_time - start_time, (parse_time - start_time) / 1000.0);
    printf("  Collection:      %.0f ns (%.3f µs)\n",
           collect_time - parse_time, (collect_time - parse_time) / 1000.0);
    printf("  Sort:            %.0f ns (%.3f µs)\n",
           sort_time - collect_time, (sort_time - collect_time) / 1000.0);
    printf("  Dedup & Sum:     %.0f ns (%.3f µs)\n",
           end_time - sort_time, (end_time - sort_time) / 1000.0);
    printf("  Total:           %.0f ns (%.3f µs)\n",
           end_time - start_time, (end_time - start_time) / 1000.0);

    return 0;
}
