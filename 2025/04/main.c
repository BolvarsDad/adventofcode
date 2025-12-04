#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 200

int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

int
count_adjacent_rolls(char grid[][MAX_SIZE], int i, int j, int rows, int cols) {
    int count = 0;
    
    for (int d = 0; d < 8; d++) {
        int nextrow = i + dx[d];
        int nextcol = j + dy[d];
        
        if (nextrow >= 0 && nextrow < rows && nextcol >= 0 && nextcol < cols) {
            if (grid[nextrow][nextcol] == '@') {
                count++;
            }
        }
    }
    
    return count;
}

int
part1(char grid[][MAX_SIZE], int rows, int cols)
{
    int accessible = 0;
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] != '@')
                continue;
            
            int adjacent = count_adjacent_rolls(grid, i, j, rows, cols);
            if (adjacent < 4) {
                accessible++;
            }
        }
    }
    
    return accessible;
}

int
part2(char grid[][MAX_SIZE], int rows, int cols)
{
    int total_removed = 0;
    bool changed = true;
    
    while (changed) {
        changed = false;
        
        int to_remove[MAX_SIZE][MAX_SIZE] = {false};
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] != '@') continue;
                
                int adjacent = count_adjacent_rolls(grid, i, j, rows, cols);
                if (adjacent < 4) {
                    to_remove[i][j] = true;
                    changed = true;
                }
            }
        }
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (to_remove[i][j]) {
                    grid[i][j] = '.';
                    total_removed++;
                }
            }
        }
    }
    
    return total_removed;
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        fclose(fp);
        return 1;
    }
    
    char grid[MAX_SIZE][MAX_SIZE];
    char grid_copy[MAX_SIZE][MAX_SIZE];
    int rows = 0, cols = 0;
    
    while (fgets(grid[rows], MAX_SIZE, fp)) {
        grid[rows][strcspn(grid[rows], "\n")] = 0;
        
        if (cols == 0)
            cols = strlen(grid[rows]);
        
        rows++;
        
        if (rows >= MAX_SIZE)
            break;
    }
    
    fclose(fp);
    
    memcpy(grid_copy, grid, sizeof(grid));
    
    int result1 = part1(grid, rows, cols);
    printf("Part 1: %d\n", result1);
    
    int result2 = part2(grid_copy, rows, cols);
    printf("Part 2: %d\n", result2);
    
    return 0;
}
