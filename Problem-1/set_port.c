#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#define MAP_PATH "/sys/fs/bpf/port_map"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int map_fd = bpf_obj_get(MAP_PATH);
    if (map_fd < 0) {
        perror("bpf_obj_get");
        return 1;
    }

    int key = 0;
    if (bpf_map_update_elem(map_fd, &key, &port, BPF_ANY) != 0) {
        perror("bpf_map_update_elem");
        close(map_fd);
        return 1;
    }

    close(map_fd);
    return 0;
}
