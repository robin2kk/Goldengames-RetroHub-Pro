/* Minimal PS5 payload execution probe for Goldengames RetroHub Pro. */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    static const char msg[] = "Goldengames RetroHub payload main executed successfully\n";
    int fd = open("/data/goldengames_retrohub_probe.log",
                  O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd < 0) {
        fd = open("/user/data/goldengames_retrohub_probe.log",
                  O_CREAT | O_TRUNC | O_WRONLY, 0644);
    }
    if (fd >= 0) {
        write(fd, msg, sizeof(msg) - 1);
        close(fd);
    }
    return 0;
}
