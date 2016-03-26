#ifndef ARDUINO_H
#define ARDUINO_H

#include <cstddef>

class Color {
public:
    unsigned char r, g, b;
    Color(unsigned int r_, unsigned int g_, unsigned int b_);
};

class Arduino {
private:
    bool device_verified;
    int arduino; // file descriptor for device
    bool send_bytes(int fd, unsigned char bytes[], std::size_t nbytes);
    bool read_bytes(int fd, unsigned char bytes[], std::size_t nbytes);
    bool verify(const char* device, int &fd);
public:
    Arduino();
    bool init();
    bool set_color(Color color);
};

#endif