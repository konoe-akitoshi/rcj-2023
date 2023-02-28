#ifndef MODULE_MOTOR_CONTOROLER_HPP
#define MODULE_MOTOR_CONTOROLER_HPP

#include "pin.hpp"
#include "core/motor_controller.hpp"

constexpr module::MotorController MotorController(raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);

#endif
