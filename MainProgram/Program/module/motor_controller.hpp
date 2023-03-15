#ifndef MODULE_MOTOR_CONTOROLER_HPP
#define MODULE_MOTOR_CONTOROLER_HPP

#include "pin.hpp"
#include "core/motor_controller.hpp"

constexpr module::MotorController MotorController(pins::rp2040::GPIO10, pins::rp2040::GPIO11, 0x0A);

#endif
