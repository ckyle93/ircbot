#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // Const read only memory!!!
    char *str = ":wolfe.freenode.net 376 Chris12222323 :End of /MOTD command.";
    char *str2 = strdup(str);
    for(int i = 0; i < 4; i++) {
        char *stripped = strsep(&str2, " ");
        printf("%s\n", stripped);
    }
}
