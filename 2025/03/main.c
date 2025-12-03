#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
read_line(FILE *fname, char *buffer, size_t bufsz)
{
    if (fgets(buffer, bufsz, fname) == NULL)
        return NULL;

    size_t len = strlen(buffer);

    if (len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = '\0';

    return buffer;
}

unsigned long long
find_maximum_subsequence(const char *digits, unsigned long long *total, int subseq_len)
{
    size_t n = strlen(digits);
    if (n < subseq_len)
        return 0;

    char chosen[128];
    if (subseq_len >= (int)sizeof(chosen))
        subseq_len = sizeof(chosen) - 1;

    int next_search_start = 0;
    int out_index = 0;

    for (int picked = 0; picked < subseq_len; picked++) {

        int still_needed = subseq_len - picked;
        int last_allowed_pos = n - still_needed;  // inclusive

        char best_digit = '0' - 1;
        int best_digit_pos = next_search_start;

        for (int pos = next_search_start; pos <= last_allowed_pos; pos++) {
            if (digits[pos] > best_digit) {
                best_digit = digits[pos];
                best_digit_pos = pos;
                if (best_digit == '9')   // can't do better
                    break;
            }
        }

        chosen[out_index++] = best_digit;
        next_search_start = best_digit_pos + 1;
    }

    chosen[out_index] = '\0';

    unsigned long long value = 0;
    for (int i = 0; i < out_index; i++)
        value = value * 10 + (chosen[i] - '0');

    if (total)
        *total += value;

    return value;
}

int
main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file.txt> <winsize>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file.");
        return 1;
    }

    char buffer[1024];
    unsigned long long total = 0;
    int k = atoi(argv[2]); // window size

    while (read_line(fp, buffer, sizeof(buffer)) != NULL) {
        unsigned long long joltage = find_maximum_subsequence(buffer, &total, k);
        printf("Line: %s -> Joltage: %llu\n", buffer, joltage);
    }

    printf("Total joltage: %llu\n", total);

    fclose(fp);

    return 0;
}
