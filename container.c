#define _GNU_SOURCE
#include <string.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <syscall.h>


static char child_stack[1048576];


static int child_fn(void* arg) {
    // config
    unshare(CLONE_NEWNET);
    system("mount --make-private -o remount /");
    system("mount -t proc proc /proc --make-private");
    system("ifconfig veth1 10.1.1.2/24 up");
    
    system("dd bs=100M seek=1 of=image.img count=0");
    system("losetup /dev/loop0 image.img");
    system("mkfs.ext4 /dev/loop0");
    system("mkdir /home/virtual_user");
    system("mount -t ext4 /dev/loop0 /home/virtual_user");
    //system("bash");

    // container info
    printf("New `net` Namespace:\n");
    system("ip link");
    system("ps aux");
    system("ping 10.1.1.1 -c 4");
    system("It's a me Mario! > test.txt");
    system("ls");
    //system("ls /sys/fs/cgroup/");

    // losetup free
    
    // cleanup
    printf("\n\n");
    system("sudo -S umount /home/virtual_user");
    system("sudo -S losetup -d /dev/loop0");
    system("sudo -S umount /proc");
    return 0;
}


int main() {
    // host info
    printf("Original `net` Namespace:\n");
    system("ip link");
    system("ps aux");
    //system("ls /sys/fs/cgroup/");
    printf("\n\n");
    
    // container creation
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWNS | CLONE_NEWPID| CLONE_NEWNET | SIGCHLD, NULL);

    char create_net_interface_command[75];
    char* create_net_interface_template = "ip link add name veth0 type veth peer name veth1 netns %d";
    sprintf(create_net_interface_command, create_net_interface_template, child_pid);

    system(create_net_interface_command);
    system("ifconfig veth0 10.1.1.1/24 up");
    
    waitpid(child_pid, NULL, 0);
    _exit(EXIT_SUCCESS);
}