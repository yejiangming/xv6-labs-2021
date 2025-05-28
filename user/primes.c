#include "kernel/types.h"

#include "user/user.h"

int prime_process(int fd[]) {
    close(fd[1]);
    int prime;
    if (read(fd[0], &prime, sizeof(prime)) == 0) {
        close(fd[0]);
        exit(0);
    } 
    fprintf(1, "prime %d\n", prime);

    int child_fd[2];
    pipe(child_fd);
    // 创建子进程
    if (fork() == 0) {
        prime_process(child_fd);
    } else {
        close(child_fd[0]);
        int n;
        while (read(fd[0], &n, sizeof(n)) > 0) {
            if (n % prime != 0) {
                write(child_fd[1], &n, sizeof(n));
            }
        }
        close(child_fd[1]);
        close(fd[0]);
        wait(0);
    }
    exit(0);
}

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);

    if (fork() == 0) {
        prime_process(fd);
    } else {
        close(fd[0]);
        for (int i = 2; i <= 35; i++) {
            write(fd[1], &i, sizeof(i));
        }
        close(fd[1]);
        wait(0);
    }
    exit(0);
}