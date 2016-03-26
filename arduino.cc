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

// Send byte to Arduino; returns success
bool Arduino::send_byte(int fd, unsigned char byte) {
    if (write(fd, &byte, 1) == 1) {
        printf("Successful write.\n");
        return true;
    } else {
        printf("Unsuccessful write.\n");
        return false;
    }
}

// Read byte from Arduino; returns byte read
unsigned char Arduino::read_byte(int fd) {
    unsigned char byte = '\0';
    if (read(fd, &byte, 1) == 1) {
        printf("Successful read.\n");
    } else {
        printf("Unsuccessful read.\n");
    }
    return byte;
}

// Initialize and verify whether a device is our Arduino
// On success, returns true and sets fd; otherwise returns false
bool Arduino::verify(const char* device, int &fd) {
    // Initialize file descriptor
    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1) {
        printf("Unable to open %s.\n", device);
        return false;
    }
    // Construct port settings
    struct termios port_settings;
    if (tcgetattr(fd, &port_settings) != 0 ) {
        printf("Error configuring port settings.\n");
        return false;
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
        return false;
    }

    // Verify device as correct Arduino
    unsigned char rand_byte = (unsigned char)rand();
    usleep(2000000);
    if (!send_byte(fd, rand_byte)) {
        return false;
    }
    usleep(2000000);
    unsigned char response = read_byte(fd);
    if (rand_byte == response) {
        return true;
    } else {
        return false;
    }
}

// Default constructor
Arduino::Arduino() : device_verified(false){ };

// Initialize Arduino class
bool Arduino::init() {
    // Construct list of candidate devices; Todo
    vector<string> devices;
    devices.push_back("/dev/ttyACM2");
    devices.push_back("/dev/ttyACM1");
    devices.push_back("/dev/ttyACM0");

    // Verify and set file descriptor
    for (unsigned int i = 0; i < devices.size(); i++) {
        printf("Attempting to verify device %s.\n", devices[i].c_str());
        if (verify(devices[i].c_str(), arduino)) {
            device_verified = true;
            printf("Arduino found.\n");
            return true;
        } else {
            printf("Verification failed.\n");
        }
    }
    printf("Arduino not found.\n");
    return false;
}

// Set emotion on Arduino
bool Arduino::set_rgb(unsigned char r, unsigned char g, unsigned char b) {
    if (!device_verified) {
        printf("Device has not yet been verified.\n");
        return false;
    }
    if (!send_byte(arduino, emot)) {
        printf("Setting emotion failed.\n");
        return false;
    }
    usleep(1000000);
    unsigned char response = read_byte(arduino);
    if (emot == response) {
        printf("Emotion set successfully.\n");
        return true;
    } else {
        printf("Setting emotion failed.\n");
        return false;
    }
}

int main() {

    Arduino arduino;
    arduino.init();
    arduino.set_rgb(1, 2, 3);

    return 0;
}