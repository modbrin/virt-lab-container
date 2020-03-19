#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <algorithm>

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

template<class... Args>
std::string concat(Args ...strs)
{
    std::ostringstream oss;
    (void)(int[]){0, ((void)(oss << strs), 0)... };
    return oss.str();
}

static void trim_right(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void save_filesystem(std::string mount_folder, std::string image_file) {
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

    // create test file
    system(concat("echo \"hello container!\" > ", mount_folder, "/hello.txt").c_str());
    std::cout << "In container: " << std::flush;
    system(concat("sudo -S cat ", mount_folder, "/hello.txt").c_str());
    
    // cleanup
    system(concat("umount ", mount_folder).c_str());
    system(concat("sudo losetup -d ", free_loop).c_str());
    
    // check after cleaning
    std::cout<< "Out of container: " << std::flush;
    system(concat("sudo -S cat ", mount_folder, "/hello.txt").c_str());
}

int main ()
{
    save_filesystem("/home/virtual_user", "image.img");
    
    return 0;
}