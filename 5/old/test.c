#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char id[11];
    char *test = "01289";
    strncpy(id, test, 9);
    id[9] = 0;
    printf("%ld\n", strlen(id));
    for (int i = 0; i <= 10; i++) {
        if (id[i] == '\0') printf("[NUL]");
        else printf("[%c]", id[i]);
    }
    return 0;
}