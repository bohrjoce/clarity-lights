#ifndef ARDUINO_H
#define ARDUINO_H



class Arduino {
private:
    bool device_verified;
    int arduino; // file descriptor for device
    bool send_byte(int fd, unsigned char byte);
    unsigned char read_byte(int fd);
    bool verify(const char* device, int &fd);
public:
    Arduino();
    bool init();
    bool set_rgb(unsigned char r, unsigned char g, unsigned char b);
};

#endif