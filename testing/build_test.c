// File by Chris Kyle

#include <stdlib.h>
#include <stdio.h>

int main() {
    int i;
    char *line_pos = "123456789012345678901234567890123456789012345678901234567890\n";

    FILE* new_file = fopen("line_pos", "w");
    for(i = 0; i < 100; ++i) {
       fprintf(new_file, line_pos);
    }
    fclose(new_file);
}
