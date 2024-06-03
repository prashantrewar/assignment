# Problem Statement: Drop TCP port

## Step 1: Compile

Compile ebpf source for dropping the packet:

```bash
clang -O2 -g -Wall -target bpf -c xdp_drop.c -o xdp_drop.o
```

This command creates a xdp_drop.o name binary file.

## Step 2: Load

Load a BPF object

```bash
sudo xdp-loader load -m skb -s xdp_drop_tcp lo xdp_drop.o
```

## Step 3: Verify

Verify the loaded bytecode:

```bash
ip link show dev lo
    ... [check the loaded ebpf-xdp code]
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 xdpgeneric qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        prog/xdp id 232 tag ----
```

## Step 4: Pin the eBPF Map:

Mount the BPF filesystem if not already mounted:

```bash
sudo mount -t bpf bpf /sys/fs/bpf/
```

Pin the map to the BPF filesystem

```bash
$ sudo bpftool map pin name port_map /sys/fs/bpf/port_map
```

## Step 5: Compile the Userspace Program:

```bash
gcc -o set_port set_port.c -lbpf
```

This command creates a set_port.o name binary file.

## Step 6: Update the Port Number:

Use the userspace program to set the port dynamically.

```bash
sudo ./set_port < port_num >
```

## Step 7: Testing:

Use netcat to test if TCP packets to port <port_num> are being dropped.
Start a netcat server:

```bash
nc -l < port_num >
```

Try to send data to the server:

```bash
echo hello | nc 127.0.0.1 < port_num >
```

### Outcome

If the eBPF program is working correctly, the hello message should not appear on the server.


## Step 8: Unload the program

```bash
sudo ip link set lo xdpgeneric off
```