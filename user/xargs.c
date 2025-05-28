#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    char buf[512];
    while (1) {
        gets(buf, 512);
        if (buf[0] == '\0') {
            break;
        }
        if (fork() == 0) {
            char *args[MAXARG+2];
            // 将 argv 中的全部参数复制到 args 中
            int i = 0;
            while( i < MAXARG && argv[i] != 0) {
                args[i] = argv[i];
                i++;
            }
            args[i] = buf;
            args[i + 1] = 0; // 确保 args 以 NULL 结尾
            exec(argv[1], args);
        } else {
            wait(0);
        }
    }
    exit(0);

}