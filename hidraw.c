#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 64

int main(int argc, char *argv[]) {
    const char *dev = NULL;
    char cmd[] =
    // bash script to enumerate hidraw devices. 1025:174B is the turbo/predator button(?)/funciton key ec on the PHN16-73, can be substituted if necessary or on different hardware.
    "for f in /sys/class/hidraw/hidraw*; do "
    "echo -n \"${f##*/}: \"; "
    "cat \"$f/device/uevent\" | grep HID_NAME | cut -d '=' -f2; "
    "done | grep \"1025174B:00 1025:174B\" | cut -c 1-7";

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen");
        return 1;
    }

    static char devbuf[64];
    if (fgets(devbuf, sizeof(devbuf), fp) != NULL) {
        devbuf[strcspn(devbuf, "\n")] = 0;

        static char path[80];
        snprintf(path, sizeof(path), "/dev/%s", devbuf);
        dev = path;
    }

    pclose(fp);

    if (!dev) {
        // yrou'e button is: damn error
        fprintf(stderr, "failed to find hidraw device\n");
        return 1;
    }

    int fd = open(dev, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned char buf[BUF_SIZE];

    while (1) {
        ssize_t n = read(fd, buf, BUF_SIZE);

        if (n > 0) {

            // print received bytes as hex. debugging or something.
            for (ssize_t i = 0; i < n; i++) {
                printf("%02X", buf[i]);
            }
            printf("\n");
            fflush(stdout);

            // check for exact match: 04 85 FF. bytecode for button on the infamous machine of doom. need this so it doesn't fire from function keys.
            if (n >= 3 &&
                buf[0] == 0x04 &&
                buf[1] == 0x85 &&
                buf[2] == 0xFF) {

                system("/opt/keyboard/cycle.sh"); // or wherever else you put cycle.sh or whatever script you're using.
                }

        } else if (n < 0) {
            perror("read");
            break;
        }

        usleep(50000); // consider changing this to 5ms if you have issues with slow button reset.
    }

    close(fd);
    return 0;
}
