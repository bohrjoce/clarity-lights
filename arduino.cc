#include "arduino.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdint.h>
#include <time.h>

using namespace std;

// Initialize and return file descriptor
int init(const char* device) {
    // Open port
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1) {
        printf("Unable to open %s.\n", device);
    }

    // Construct port settings
    struct termios port_settings;
    if (tcgetattr(fd, &port_settings) != 0 ) {
        printf("Error configuring port settings.\n");
    }

    // Baud rates
    cfsetispeed(&port_settings, B9600);
    cfsetospeed(&port_settings, B9600);

    // Set settings
    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    port_settings.c_cc[VMIN] = 1;
    port_settings.c_cc[VTIME] = 5;
    port_settings.c_cflag |= CREAD | CLOCAL;
    cfmakeraw(&port_settings);
    
    // Flush port and apply the settings to the port
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &port_settings) != 0) {
        printf("Error applying port settings.\n");
    }

    return fd;
}

// Send byte to Arduino
void send_byte(int fd, unsigned char byte) {
    if (write(fd, &byte, 1) == 1) {
        printf("Successful write.\n");
    } else {
        printf("Unsuccessful write.\n");
    }
}

// Read byte from Arduino
unsigned char read_byte(int fd) {
    unsigned char byte = '\0';
    if (read(fd, &byte, 1) == 1) {
        printf("Successful read.\n");
    } else {
        printf("Unsuccessful read.\n");
    }
    return byte;
}

// Verify whether a device is our Arduino
// On success, returns true and sets fd; otherwise returns false
bool verify(string device, int &fd) {
    unsigned char rand_byte = (unsigned char)rand();
    fd = init(device.c_str());
    usleep(2000000);
    send_byte(fd, rand_byte);
    usleep(2000000);
    unsigned char response = read_byte(fd);
    if (rand_byte == response) {
        printf("Device %s verified.\n", device.c_str());
        return true;
    } else {
        printf("Device %s not verified.\n", device.c_str());
        return false;
    }
}

int main() {

    // Construct list of candidate devices
    vector<string> devices;
    devices.push_back("/dev/ttyACM2");
    devices.push_back("/dev/ttyACM1");
    devices.push_back("/dev/ttyACM0");

    // Verify and use our Arduino
    int arduino;
    for (unsigned int i = 0; i < devices.size(); i++) {
        if (verify(devices[i], arduino)) {
            unsigned char in;
            while(1) {
                cin >> in;
                usleep(1500000);
                send_byte(arduino, in);
                usleep(1500000);
                unsigned char n = read_byte(arduino);
                cout << n << endl;
            }
        }
    }

    return 0;
}