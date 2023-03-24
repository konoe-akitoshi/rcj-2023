#ifndef MODULE_TERMINAL_HPP
#define MODULE_TERMINAL_HPP

#include "core/terminal.hpp"
#include "i2c_manager.hpp"

constexpr module::Terminal Terminal(I2CManager, 0x11);

#endif
