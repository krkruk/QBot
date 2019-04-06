#ifndef TOOLS_H_
#define TOOLS_H_
#include <Arduino.h>

template<typename T>
constexpr void unused(const T &var)
{
    (void) var;
}


class SregLock
{
    SregLock(const SregLock &) = delete;
    SregLock(SregLock &&) = delete;
    SregLock& operator=(const SregLock &) = delete;
    SregLock& operator=(SregLock &&) = delete;

    uint8_t backupSreg;
public:
    SregLock()
    {
        backupSreg = SREG;
        cli();
    }

    ~SregLock()
    {
        SREG = backupSreg;
    }
};


#endif /* TOOLS_H_ */
