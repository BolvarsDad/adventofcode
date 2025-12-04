#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

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

// hash set for deduplication
#define MAX_CANDIDATES 10000

struct candidate_set
{
    uint64_t values[MAX_CANDIDATES];
    int count;
};

int
contains(struct candidate_set *set, uint64_t value) {
    for (int i = 0; i < set->count; i++) {
        if (set->values[i] == value) return 1;
    }
    return 0;
}

void
add(struct candidate_set *set, uint64_t value) {
    if (!contains(set, value) && set->count < MAX_CANDIDATES) {
        set->values[set->count++] = value;
    }
}


uint64_t
sum_invalid_in_range(uint64_t start, uint64_t end)
{
    struct candidate_set set = {0};

    printf("  Range %llu-%llu:\n", start, end);

    int min_exp = (int)log10(start);
    int max_exp = (int)log10(end);

    for (int exp = min_exp; exp <= max_exp; exp++) {
        int digits = exp + 1;

        for (int rep = 2; rep <= digits; rep++) {
            if (digits % rep != 0)
                continue;

            int per_rep = digits / rep;
            uint64_t base = pow10_u(per_rep);

            // build factor = 1 + base + base^2 + ...
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

            printf("    digits=%d, rep=%d, per_rep=%d, factor=%llu, seq_range=%llu..%llu\n",
                   digits, rep, per_rep, factor, min_seq, max_seq);

            for (uint64_t seq = min_seq; seq <= max_seq; seq++) {
                uint64_t candidate = seq * factor;
                if (candidate >= start && candidate <= end) {
                    printf("      Found: %llu (seq %llu * factor %llu)%s\n",
                           candidate, seq, factor,
                           contains(&set, candidate) ? " [DUPLICATE]" : "");
                    add(&set, candidate);
                }
            }
        }
    }

    uint64_t total = 0;
    for (int i = 0; i < set.count; i++) {
        total += set.values[i];
    }

    return total;
}

int main(int argc, char **argv) {
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

    uint64_t total = 0;
    char *token = strtok(line, ",");

    while (token != NULL) {
        uint64_t start, end;
        if (sscanf(token, "%llu-%llu", &start, &end) == 2) {
            uint64_t range_sum = sum_invalid_in_range(start, end);
            printf("  Subtotal for this range: %llu\n\n", range_sum);
            total += range_sum;
        }
        token = strtok(NULL, ",");
    }

    free(line);

    printf("Sum of invalid IDs: %llu\n", total);
    printf("Expected: 15704845910\n");
    printf("Difference: %lld\n", (int64_t)total - 15704845910);
    return 0;
}
