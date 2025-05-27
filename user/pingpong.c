#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int fd_parent_2_child[2];
    int fd_child_2_parent[2];
    pipe(fd_parent_2_child);
    pipe(fd_child_2_parent);

    if (fork() == 0) {
        // 子进程
        close(fd_parent_2_child[1]); // 关闭父进程到子进程的写端
        close(fd_child_2_parent[0]); // 关闭子进程到父进程的读端

        char buf[2];
        read(fd_parent_2_child[0], buf, 1); // 从父进程读取数据
        fprintf(1, "%d: received ping\n", getpid());
        write(fd_child_2_parent[1], "P", 1); // 向父进程发送数据
        exit(0);

    } else {
        // 父进程
        close(fd_parent_2_child[0]); // 关闭父进程到子进程的读端
        close(fd_child_2_parent[1]); // 关闭子进程到父进程的写端
        write(fd_parent_2_child[1], "P", 1); // 向子进程发送数据
        char buf[2];
        read(fd_child_2_parent[0], buf, 1); // 从子进程读取数据
        fprintf(1, "%d: received pong\n", getpid());
        exit(0);
    }
}
