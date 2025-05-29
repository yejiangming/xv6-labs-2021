#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int tick = uptime();
    fprintf(1, "tick: %d\n", tick);
    exit(0);
}