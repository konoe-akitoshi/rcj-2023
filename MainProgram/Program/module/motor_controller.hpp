#ifndef MODULE_MOTOR_CONTOROLER_HPP
#define MODULE_MOTOR_CONTOROLER_HPP

#include "core/motor_controller.hpp"
#include "i2c_manager.hpp"

constexpr module::MotorController MotorController(I2CManager, 0x0A);

#endif
