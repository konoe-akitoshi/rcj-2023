#ifndef MODULE_CAMERA_HPP
#define MODULE_CAMERA_HPP

#include "i2c_manager.hpp"
#include "core/camera.hpp"

constexpr module::Camera Camera(I2CManager, 0x10);

#endif
