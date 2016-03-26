#include "arduino.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

// Color constructor
Color::Color(unsigned int r_, unsigned int g_, unsigned int b_) {
    if (r_ > 255) r_ = 255;
    if (g_ > 255) g_ = 255;
    if (b_ > 255) b_ = 255;
    r = r_;
    g = g_;
    b = b_;
}

// Send bytes to Arduino; returns success
bool Arduino::send_bytes(int fd, unsigned char bytes[], size_t nbytes) {
    unsigned int bytes_written, spot = 0;
    do {
        bytes_written = write(fd, &bytes[spot], nbytes - spot);
        spot += bytes_written;
    } while (spot < nbytes && bytes_written > 0);
    if (spot == nbytes) {
        printf("Successful write.\n");
        return true;
    } else {
        printf("Unsuccessful write.\n");
        return false;
    }
}

// Read nbytes from Arduino; returns success
bool Arduino::read_bytes(int fd, unsigned char bytes[], size_t nbytes) {
    unsigned int bytes_read, spot = 0;
    do {
        bytes_read = read(fd, &bytes[spot], nbytes - spot);
        spot += bytes_read;
    } while(spot < nbytes && bytes_read > 0);
    if (spot == nbytes) {
        printf("Successful read.\n");
        return true;
    } else {
        printf("Unsuccessful read.\n");
        return false;
    }
}

// Initialize and verify whether a device is the Arduino
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

    // Port settings
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

    // Verify device as the Arduino
    unsigned char rand_bytes[3] = {
        (unsigned char)rand(),
        (unsigned char)rand(),
        (unsigned char)rand()
    };
    unsigned char response[3] = {'\0', '\0', '\0'};
    usleep(2000000);
    if (!send_bytes(fd, rand_bytes, 3)) return false;
    usleep(2000000);
    if (!read_bytes(fd, response, 3)) return false;
    for (unsigned int i = 0; i < 3; i++) {
        if (rand_bytes[i] != response[i]) return false;
    }
    return true;
}

// Arduino constructor
Arduino::Arduino() : device_verified(false) {};

// Initialize Arduino class; find location of the Arduino and set file descriptor
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
            printf("Verification successful.\n");
            return true;
        } else {
            printf("Verification failed.\n");
        }
    }
    return false;
}

// Send desired light strip color to Arduino
bool Arduino::set_color(Color color) {
    // Check that device is verified
    if (!device_verified) {
        printf("Device has not yet been verified.\n");
        return false;
    }

    // Send color and check response
    unsigned char rgb[3] = {color.r, color.g, color.b};
    unsigned char response[3] = {'\0', '\0', '\0'};
    if (!send_bytes(arduino, rgb, 3)) return false;
    usleep(1000000);
    if (!read_bytes(arduino, response, 3)) return false;
    for (unsigned int i = 0; i < 3; i++) {
        if (rgb[i] != response[i]) return false;
    }
    return true;
}

int main() {

    // Example of use
    Arduino arduino;
    int r, g, b;
    if (arduino.init()) {
        printf("Arduino found.\n");
        while (1) {
            printf("r = ");
            cin >> r;
            printf("g = ");
            cin >> g;
            printf("b = ");
            cin >> b;
            if (arduino.set_color(Color(r, g, b))) {
                printf("Color set successfully.\n");
            } else {
                printf("Setting color failed.\n");
            }
        }
    } else {
        printf("Arduino not found.\n");
    }

    return 0;
}