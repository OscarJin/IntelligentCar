import sensor, image, time
import math
import gc
from pyb import UART
import json


######  设置一些常数  ########
orange_threshold = (20, 90, 15, 60, 40, 99)
measurement_const = 1450
uart = UART(3, 19200)


######  函数定义  ######
def DistanceMeasurement(r):
    return measurement_const/r  #unit: cm


def AngleMeasurement(obj):
    d_pix = abs(obj.cx()-w/2)
    return math.atan(d_pix * 2 / measurement_const) * 180 / math.pi


######  摄像头设置  ######
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.SVGA)
w = 800
h = 600   #w,h of HD
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
clock = time.clock()
gc.enable()


###### 主循环  ######
while(True):
    clock.tick()
    #if uart.any():
        #if uart.readline().decode() != '1':
            #continue
    #else:
        #continue

    OrangeCircles = []
    img = sensor.snapshot().lens_corr(1.8)
    blobs = img.find_blobs([orange_threshold], x_stride=4, pixels_threshold=40, merge=True)

    if blobs:
        for b in blobs:
           OrangeCircles.append(b)
           radius = (int)((b.w()+b.h()) / 4)
           img.draw_circle(b.cx(), b.cy(), radius, color=(0,255,0))

    output_str = json.dumps([False, 0, 0])
    if(len(OrangeCircles) != 0):
        print(OrangeCircles)
        distance = []
        for o in OrangeCircles:
            dist = DistanceMeasurement((o.w()+o.h()) / 4)
            distance.append(dist)
        dist_min = min(distance)
        dist_min_i = distance.index(dist_min)
        angle = AngleMeasurement(OrangeCircles[dist_min_i])
        output_str=json.dumps([True, dist_min, angle])
        print(output_str)
    else:
        print("Not Found!")

    uart.write(output_str)
    time.sleep_ms(100)
