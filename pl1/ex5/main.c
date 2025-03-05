#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

// 8 processes are created, 7 children
// Yes, due to the way the system works and there being no waits

int main(void) {
    fork();
    printf(" 1\n ");
    fork();
    printf(" 2\n ");
    fork();
    printf(" 3\n ");
    return 0;
}
