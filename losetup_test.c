#include <stdio.h>

int main () {
    system("dd if=/dev/zero of=image.img bs=1M count=64");    
    system("mkfs.ext4 image.img");
    
    FILE *fp;
    char res[100];
    fp = popen("losetup -f", "r");
    // Read one line from output - free loop device
    fgets(res, sizeof(res), fp);
    pclose(fp);

    char losetup_command[75];
    memset(losetup_command, '\0', 75);
    printf(res);
    char* losetup_template = "losetup %s image.img";
    sprintf(losetup_command, losetup_template, res);

    system(losetup_command);
    printf("CALLING: %s", losetup_command);
    system("mkdir /home/virtual_user");

    char mount_command[75];
    char* mount_template = "mount -t ext4 %s /home/virtual_user";
    sprintf(mount_command, mount_template, res);

    system(mount_command);
    printf("CALLING: %s", mount_command);


    system("umount /home/virtual_user");

    system("\"hello container!\" > /home/virtual_user/hello.txt");
    system("sudo -S cat /home/virtual_user/hello.txt");

    char unmount_command[75];
    char* unmount_template = "losetup -d %s";
    sprintf(unmount_command, unmount_template, res);
    system(unmount_command);
    system("sudo -S cat /home/virtual_user/hello.txt");
}