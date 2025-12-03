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
find_maximum_subsequence(const char *line, unsigned long long *total, int k)
{
    size_t len = strlen(line);
    if (len < k)
        return 0;

    char res[128];
    if (k >= (int)sizeof(res))
        k = sizeof(res) - 1;
    
    int start = 0;
    int outpos = 0;

    for (int pick = 0; pick < k; ++pick) {
        int remaining = k - pick;
        int search_end = len - remaining;

        char best_digit = '0' - 1;
        int best_pos = start;

        for (int i = start; i <= search_end; ++i) {
            if (line[i] > best_digit) {
                best_digit = line[i];
                best_pos = i;

                if (best_digit == '9')
                    break;
            }
        }

        res[outpos++] = best_digit;
        start = best_pos + 1;
    }

    res[outpos] = '\0';

    unsigned long long value = 0;
    for (int i = 0; i < outpos; ++i)
        value = value * 10 + (res[i] - '0');

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
