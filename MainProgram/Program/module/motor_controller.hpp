#ifndef MODULE_MOTOR_CONTOROLER_HPP
#define MODULE_MOTOR_CONTOROLER_HPP

#include "_pin.hpp"
#include "core/motor_controller.hpp"

const module::MotorController MotorController(raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);

#endif
