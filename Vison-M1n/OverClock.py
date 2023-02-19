# オーバークロック用プログラム

from Maix import freq  # 周波数読み出し

# オーバークロックの設定：設定後自動再起動がかかる→その後コメントアウト
# CPU[26-600] Max:800(MaixPy上では設定不可) default:400
# pll1[26-1200] Max:1800(MaixPy上では設定不可) default:400
# kpu_div[1-16] default:1 (kpu frequency = pll1/kpu_div)

# freq.set(cpu=600, pll1=600, kpu_div=1)

import sensor, image, time, lcd

sensor.reset()  # Reset and initialize the sensor. It will
# run automatically, call sensor.run(0) to stop
sensor.set_pixformat(sensor.RGB565)  # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # Set frame size to QVGA (320x240)
sensor.skip_frames(time=2000)  # Wait for settings take effect.
clock = time.clock()  # Create a clock object to track the FPS.
sensor.run(1)

while True:
    clock.tick()  # Update the FPS clock.
    img = sensor.snapshot()  # Take a picture and return the image.
    print(" FPS=%d" % clock.fps(), end="")  # FPSの出力：MaixPy上の値は実際の値の半分になる
    print(" CPU_freq=%d" % freq.get_cpu(), end="")  # cpuの周波数：オーバークロックしてない場合はエラーになる
    print(" CPU_freq=%d" % freq.get_kpu())  # kpuの周波数：オーバークロックしてない場合はエラーになる
