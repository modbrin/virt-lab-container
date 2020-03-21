[BACK TO README](./README.md)

```shell
$ sudo ./container

Host `net` Namespace ***********************************************************************
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: enp0s20f0u5u3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP mode DEFAULT group default qlen 1000
    link/ether 00:e0:4c:68:00:74 brd ff:ff:ff:ff:ff:ff
3: wlp59s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP mode DORMANT group default qlen 1000
    link/ether 9c:b6:d0:c4:8b:bb brd ff:ff:ff:ff:ff:ff


Host process tree **************************************************************************
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.1  0.0 174336 11448 ?        Ss   15:33   0:11 /sbin/init
root           2  0.0  0.0      0     0 ?        S    15:33   0:00 [kthreadd]
root           3  0.0  0.0      0     0 ?        I<   15:33   0:00 [rcu_gp]
...<long list of other procs>


Container `net` Namespace ******************************************************************
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: veth1@if9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP mode DEFAULT group default qlen 1000
    link/ether 42:b7:18:6e:d5:6a brd ff:ff:ff:ff:ff:ff link-netnsid 0

PING 10.1.1.1 (10.1.1.1) 56(84) bytes of data.
64 bytes from 10.1.1.1: icmp_seq=1 ttl=64 time=0.038 ms
64 bytes from 10.1.1.1: icmp_seq=2 ttl=64 time=0.035 ms
64 bytes from 10.1.1.1: icmp_seq=3 ttl=64 time=0.034 ms
64 bytes from 10.1.1.1: icmp_seq=4 ttl=64 time=0.037 ms

--- 10.1.1.1 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3051ms
rtt min/avg/max/mdev = 0.034/0.036/0.038/0.001 ms


Container process tree *********************************************************************
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.0  0.0   6728  1284 pts/0    S+   17:44   0:00 ./container
root           9  0.0  0.0   9420  3248 pts/0    R+   17:44   0:00 ps aux


Container save fs into file ****************************************************************
64+0 records in
64+0 records out
67108864 bytes (67 MB, 64 MiB) copied, 0.415177 s, 162 MB/s
mke2fs 1.45.5 (07-Jan-2020)
Discarding device blocks: done                            
Creating filesystem with 65536 1k blocks and 16384 inodes
Filesystem UUID: 118291f5-ba69-476e-8969-95662e1c118a
Superblock backups stored on blocks: 
        8193, 24577, 40961, 57345

Allocating group tables: done                            
Writing inode tables: done                            
Creating journal (4096 blocks): done
Writing superblocks and filesystem accounting information: done

File In container: hello container!


Benchmarking *************************************************************************
cpu -> benchmark_results_container/cpu_bench_container.txt
fileio random write -> benchmark_results_container/fileio_bench_rndwr_18G_container.txt
fileio random read -> benchmark_results_container/fileio_bench_rndrd_18G_container.txt
fileio sequential write -> benchmark_results_container/fileio_bench_seqwr_18G_container.txt
fileio sequential read -> benchmark_results_container/fileio_bench_seqrd_18G_container.txt
memory block 1M -> benchmark_results_container/memory_bench_blk1M_container.txt
memory block 1K -> benchmark_results_container/memory_bench_blk1K_container.txt
threads -> benchmark_results_container/threads_bench_container.txt
Done benchmarking

File Outside container (should be error): cat: /home/virtual_user/hello.txt: No such file or directory
```