#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
is_invalid_id(long long num)
{
    char str[32];
    sprintf(str, "%lld", num);
    int len = strlen(str);

    for (int pattern_len = 1; pattern_len <= len / 2; pattern_len++) {
        if (len % pattern_len != 0) continue;

        int repeats = len / pattern_len;
        if (repeats < 2) continue;

        int is_pattern = 1;
        for (int i = pattern_len; i < len; i++) {
            if (str[i] != str[i % pattern_len]) {
                is_pattern = 0;
                break;
            }
        }

        if (is_pattern) return 1;
    }

    return 0;
}


int
main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file.");
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
    
    long long total = 0;
    char *token = strtok(line, ",");
    
    while (token != NULL) {
        printf("Token: %s\n", token);
        long long start, end;
        if (sscanf(token, "%lld-%lld", &start, &end) == 2) {
            for (long long num = start; num <= end; num++) {
                if (is_invalid_id(num)) {
                    total += num;
                }
            }
        }
        token = strtok(NULL, ",");
    }
    
    free(line);
    
    printf("Sum of invalid IDs: %lld\n", total);
    
    return 0;
}
