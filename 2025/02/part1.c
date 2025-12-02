#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
is_invalid_id(long long num)
{
    char str[32];
    sprintf(str, "%lld", num);
    int len = strlen(str);
    
    // Must have even length to be a repeated pattern
    if (len % 2 != 0) return 0;
    
    int half = len / 2;
    
    for (int i = 0; i < half; i++) {
        if (str[i] != str[half + i]) {
            return 0;
        }
    }
    
    return 1;
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
