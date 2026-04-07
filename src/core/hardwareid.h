#ifndef HARDWAREID_H
#define HARDWAREID_H

#include <QString>

class HardwareId
{
public:
    static QString generate();
    static QString getCpuId();
    static QString getMacAddress();
    static QString getDiskSerial();

private:
    HardwareId() = default;
};

#endif // HARDWAREID_H
