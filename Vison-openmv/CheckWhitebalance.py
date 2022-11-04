# Sensor Auto Gain Control
#
# This example shows off how to control the sensor's gain
# using the automatic gain control algorithm.

# What's the difference between gain and exposure control?
#
# Well, by increasing the exposure time for the image you're getting more
# light on the camera. This gives you the best signal to noise ratio. You
# in general always want to increase the expsoure time... except, when you
# increase the exposure time you decrease the maximum possible frame rate
# and if anything moves in the image it will start to blur more with a
# higher exposure time. Gain control allows you to increase the output per
# pixel using analog and digital multipliers... however, it also amplifies
# noise. So, it's best to let the exposure increase as much as possible
# and then use gain control to make up any remaining ground.

# We can achieve the above by setting a gain ceiling on the automatic
# gain control algorithm. Once this is set the algorithm will have to
# increase the exposure time to meet any gain needs versus using gain
# to do so. However, this comes at the price of the exposure time varying
# more when the lighting changes versus the exposure being constant and
# the gain changing.
# This example shows off how to control the camera sensor's
# white balance gain manually versus letting the AWB control run.

# White balance is achieve by adjusting R/G/B gain values
# such that the average color of the image is gray. The
# automatic white balance (AWB) algorithm does this for
# you but usually ends up with a different result each
# time you turn the camera on making it hard to get
# color tracking settings right. By manually recording
# the gain values you like and then forcing them to
# the sensor on startup you can control the colors
# the camera sees.

import sensor, image, time

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)

# The gain db ceiling maxes out at about 24 db for the OV7725 sensor.
sensor.set_auto_gain(True, gain_db_ceiling = 16.0) # Default gain.

# Note! If you set the gain ceiling to low without adjusting the exposure control
# target value then you'll just get a lot of oscillation from the exposure
# control if it's on.

# You can control the white balance gains here. The first value is the
# R gain in db, and then the G gain in db, followed by the B gain in db.
#
# Uncomment the below line with gain values you like (get them from the print out).
#
# sensor.set_auto_whitebal(False, rgb_gain_db = (0.0, 0.0, 0.0))

# Note: Putting (0.0, 0.0, 0.0) for the gain results in something close to zero
# comming out. Do not expect the exact value going in to be equal to the value
# comming out.

sensor.skip_frames(time = 2000)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS.

while(True):
    clock.tick()                    # Update the FPS clock.
    img = sensor.snapshot()         # Take a picture and return the image.
    print("FPS %.2f, Gain %.2f dB, Exposure %d us," % \
        (clock.fps(), sensor.get_gain_db(), sensor.get_exposure_us()),end=""),
    print(" RGB Gain= (%.2f, %.2f, %.2f)" % \
        sensor.get_rgb_gain_db())   # Prints the AWB current RGB gains.
    time.sleep(250)

