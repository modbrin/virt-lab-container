#define _GNU_SOURCE
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <syscall.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <algorithm>

/*
 * Checklist
 * 
 * [x] network
 * [x] pid (namespaces)
 * [x] fs save
 * [ ] cgroups
 * [ ] benchmarks
 */

// sudo apt update
// sudo apt install net-tools
// sudo apt install sysbench

static char child_stack[1048576];

// call shell command and return it's output
// based on https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
std::string exec(std::string cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
}

// concatenate multiple strings into single string
template<class... Args>
static std::string concat(Args ...strs)
{
    std::ostringstream oss;
    (void)(int[]){0, ((void)(oss << strs), 0)... };
    return oss.str();
}

// trim string whitespace characters (only at the end)
static inline void trim_right(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// create zeroed `image_file`, setup it as loop device and mount to `mount_folder`
std::string fs_save(std::string mount_folder, std::string image_file = "image.img") {
    // create image file
    system(concat("dd if=/dev/zero of=", image_file, " bs=1M count=64").c_str());
    // setup filesystem for created file
    system(concat("mkfs.ext4 ", image_file).c_str());
    // find(or implicitly create) free loop device
    std::string free_loop = exec("sudo -S losetup -f");
    trim_right(free_loop);
    // connect image file to loop device
    system(concat("sudo -S losetup ", free_loop, " ", image_file).c_str());
    // create directory for mounting
    system(concat("sudo -S mkdir -p ", mount_folder).c_str());
    // mount loop device
    system(concat("sudo -S mount -t ext4 ", free_loop, " ", mount_folder).c_str());
    return free_loop;
}  

void clean_fs_save(std::string free_loop, std::string mount_folder, std::string image_file = "image.img")
{
    // cleanup
    system(concat("umount ", mount_folder).c_str());
    system(concat("sudo losetup -d ", free_loop).c_str());
}

void benchmark()
{
    system("sysbench --test=cpu --cpu-max-prime=20000 run");
    
    // mode seqwr, seqrd, rndwr, rndrd
    // system("sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=300 --max-requests=0 prepare");
    // system("sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=300 --max-requests=0 run");
    // system("sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=300 --max-requests=0 cleanup");

    // system("sysbench memory --memory-block-size=1M --memory-total-size=10G run");
    // system("sysbench memory --memory-block-size=1K --memory-total-size=8G run");

    // system("sysbench threads --threads=128 run");

}

static int child_fn(void* arg) {
    // root config
    unshare(CLONE_NEWNET);
    system("mount --make-private -o remount /");
    system("mount -t proc proc /proc --make-private");
    
    // network config
    system("ifconfig veth1 10.1.1.2/24 up");

    // save filesystem as file
    std::string mount_folder("/home/virtual_user");
    std::string loop_device = fs_save(mount_folder);
    // create test file
    system(concat("echo \"hello container!\" > ", mount_folder, "/hello.txt").c_str());
    std::cout << "File In container: " << std::flush;
    system(concat("sudo -S cat ", mount_folder, "/hello.txt").c_str());

    benchmark();

    //system("mount -t proc proc /proc --make-private");
    printf("New `net` Namespace:\n");
    system("ip link");
    system("ping 10.1.1.1 -c 4");
    printf("\n\n");

    // cleanup
    clean_fs_save(loop_device, mount_folder);
    // should produce file not found error:
    std::cout<< "File Outside container (should be error): " << std::flush;
    system(concat("sudo -S cat ", mount_folder, "/hello.txt").c_str());
    
    system("sudo -S umount /proc"); return 0;
}

int main() {
    printf("Original `net` Namespace:\n");
    system("ip link");
    //system("ps aux");
    printf("\n\n");
    
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWNS | CLONE_NEWPID| CLONE_NEWNET | SIGCHLD, NULL);
    system(concat("ip link add name veth0 type veth peer name veth1 netns ", child_pid).c_str());
    system("ifconfig veth0 10.1.1.1/24 up");

    waitpid(child_pid, NULL, 0);
    _exit(EXIT_SUCCESS);
}