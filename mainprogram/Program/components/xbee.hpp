#ifndef COMPONENT_XBEE_HPP
#define COMPONENT_XBEE_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.h"
#endif

namespace component
{
class XBee
{
  public:
    explicit XBee(const int speed);
    bool has_data() const;
    int read_data() const;
    void send_data(const int data) const;
};

inline XBee::XBee(const int speed) {
    Serial1.begin(speed);
}

inline bool XBee::has_data() const {
    return Serial1.available() > 0;
}

inline int XBee::read_data() const {
    int ret = 0;
    while (Serial1.available() > 0) {
        ret = Serial1.read();
    }
    return ret;
}

inline void XBee::send_data(const int data) const {
    Serial.write(data);
}
}  // namespace component

#endif
