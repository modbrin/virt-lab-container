# Container Lab

### Dependencies

**Ubuntu**
```shell
sudo apt update
sudo apt install gcc
sudo apt install net-tools
sudo apt install sysbench
```

**Arch**
```shell
sudo pacman -S gcc
sudo pacman -S net-tools
sudo pacman -S sysbench
```

Tested on gcc v9.2.1

### Build
```shell
make
```

### Run
```shell
sudo ./container
```

> By default container uses cgroups setup and skips fileio benchmarking due to taking lots of space, if you want you can disable cgroups demo by setting ENABLE_CGROUPS_DEMO flag to false and adjusting `benchmark()` function such that fileio tests will take as much space as you can provide (default is 18 Gigs).

> You can view demo output [here](./console_output_container.md)

> Also there is [lab report document](./MSurkov_L1_ContainerReport.pdf)

### Benchmarking (Used Arch Linux 5.5.10-arch1-1)

**LXC Config**
```shell
$ yay -S lxc
$ // config NAT bridge (https://wiki.archlinux.org/index.php/Linux_Containers#Using_a_NAT_bridge)
$ sudo lxc-create -n demo-container -t download -- --dist archlinux --release current --arch amd64
$ sudo lxc-start -n demo-container
$ sudo lxc-attach -n demo-container
$$ pacman -S sysbench
# after usage
$$ exit
$ sudo lxc-stop -n demo-container
$ sudo lxc-destroy -n demo-container
```
**Docker Config**
> refer to https://wiki.archlinux.org/index.php/Docker
```shell
$ yay -S docker
$ sudo systemctl enable docker.service
$ sudo systemctl start docker.service
$ sudo docker info
$ sudo docker run -it archlinux:latest /bin/bash
$$ pacman -Sy sysbench
$$ exit

```

**Tools Used**
```shell
# cpu
sysbench cpu --cpu-max-prime=20000 run

# fileio 7G 5min
sysbench fileio --file-total-size=7G --file-test-mode=rndwr --time=300 --max-requests=0 prepare
sysbench fileio --file-total-size=7G --file-test-mode=rndwr --time=300 --max-requests=0 run
sysbench fileio --file-total-size=7G --file-test-mode=rndwr --time=300 --max-requests=0 cleanup

sysbench fileio --file-total-size=7G --file-test-mode=rndrd --time=300 --max-requests=0 prepare
sysbench fileio --file-total-size=7G --file-test-mode=rndrd --time=300 --max-requests=0 run
sysbench fileio --file-total-size=7G --file-test-mode=rndrd --time=300 --max-requests=0 cleanup

sysbench fileio --file-total-size=7G --file-test-mode=seqwr --time=300 --max-requests=0 prepare
sysbench fileio --file-total-size=7G --file-test-mode=seqwr --time=300 --max-requests=0 run
sysbench fileio --file-total-size=7G --file-test-mode=seqwr --time=300 --max-requests=0 cleanup

sysbench fileio --file-total-size=7G --file-test-mode=seqrd --time=300 --max-requests=0 prepare
sysbench fileio --file-total-size=7G --file-test-mode=seqrd --time=300 --max-requests=0 run
sysbench fileio --file-total-size=7G --file-test-mode=seqrd --time=300 --max-requests=0 cleanup

# fileio 18G 2min
sysbench fileio --file-total-size=18G --file-test-mode=rndwr --time=120 --max-requests=0 prepare
sysbench fileio --file-total-size=18G --file-test-mode=rndwr --time=120 --max-requests=0 run
sysbench fileio --file-total-size=18G --file-test-mode=rndwr --time=120 --max-requests=0 cleanup

sysbench fileio --file-total-size=18G --file-test-mode=rndrd --time=120 --max-requests=0 prepare
sysbench fileio --file-total-size=18G --file-test-mode=rndrd --time=120 --max-requests=0 run
sysbench fileio --file-total-size=18G --file-test-mode=rndrd --time=120 --max-requests=0 cleanup

sysbench fileio --file-total-size=18G --file-test-mode=seqwr --time=120 --max-requests=0 prepare
sysbench fileio --file-total-size=18G --file-test-mode=seqwr --time=120 --max-requests=0 run
sysbench fileio --file-total-size=18G --file-test-mode=seqwr --time=120 --max-requests=0 cleanup

sysbench fileio --file-total-size=18G --file-test-mode=seqrd --time=120 --max-requests=0 prepare
sysbench fileio --file-total-size=18G --file-test-mode=seqrd --time=120 --max-requests=0 run
sysbench fileio --file-total-size=18G --file-test-mode=seqrd --time=120 --max-requests=0 cleanup

# memory
sysbench memory --memory-block-size=1M --memory-total-size=10G run
sysbench memory --memory-block-size=1K --memory-total-size=8G run

# threads
sysbench threads --threads=512 run
```