#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256
#define DIAL_SZ 100

char *
read_line(FILE *fname, char *buf, size_t bufsz)
{
    if (fgets(buf, bufsz, fname) == NULL)
        return NULL;

    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n')
        buf[len-1] = '\0';

    return buf;
}

int
process_rotation(const char *instr, int *pos, int *zero_count)
{
    if (instr == NULL || instr[0] == '\0')
        return 0;

    char direction = instr[0];
    if (direction != 'L' && direction != 'R')
        return 0;

    int dist = atoi(&instr[1]);
    if (dist < 0)
        return 0;

    int old_pos = *pos;

    // Calculate new position
    if (direction == 'L') {
        *pos = (*pos - dist) % DIAL_SZ;
        if (*pos < 0)
            *pos += DIAL_SZ;
    } else {
        *pos = (*pos + dist) % DIAL_SZ;
    }

    // Count how many times we point at 0 during this rotation
    // Complete rotations each pass through 0 once
    *zero_count += dist / DIAL_SZ;

    // Check the partial rotation
    int partial = dist % DIAL_SZ;
    if (partial > 0 && old_pos != 0) {
        if (direction == 'R') {
            if (old_pos + partial > DIAL_SZ)
                (*zero_count)++;
        } else {
            if (old_pos < partial)
                (*zero_count)++;
        }
    }

    if (*pos == 0)
        (*zero_count)++;

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

    char line[MAX_LINE_LEN];
    int pos = 50;
    int zero_count = 0;

    printf("Starting position: %d\n\n", pos);

    while (read_line(fp, line, MAX_LINE_LEN) != NULL) {
        int old_pos = pos;

        if (process_rotation(line, &pos, &zero_count))
            printf("Rotation %s: %d -> %d\n", line, old_pos, pos);
    }

    fclose(fp);

    printf("Pass: %d\n", zero_count);

    return 0;
}
