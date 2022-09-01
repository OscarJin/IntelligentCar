import sensor, image, time
import math
import gc
from pyb import UART


######  设置一些常数  ########
orange_threshold = (20, 90, 15, 60, 20, 100)
green_threshold = (10, 50, -30, -5, 10, 65)
measurement_const = 1450
uart = UART(3, 115200)


######  函数定义  ######
def DistanceMeasurement(r):
    dist2 = (measurement_const/r)*(measurement_const/r) - 400.0
    if dist2 < 0:
        dist2 = 0
    dist = math.sqrt(dist2)
    return (int)(dist)  #precision: 1cm


def AngleMeasurement(obj):
    d_pix = obj.cx()-w/2
    return (int)(math.atan(d_pix * 2 / measurement_const) * 180 * 100 / math.pi)  #precision:0.01deg


def BallinGreen(obj):
    #green_roi = (obj.x(), 0, obj.w(), obj.h())
    #if img.find_blobs([green_threshold], roi=green_roi, x_stride=2, pixels_threshold=20, merge=True):
        #return True
    #else:
        #return False
    return False


######  摄像头设置  ######
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.SVGA)
sensor.set_auto_whitebal(True)
w = 800
h = 600   #w,h of HD
#sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
clock = time.clock()
gc.enable() #auto garbage collection


###### 主循环  ######
while(True):
    clock.tick()

    output_str = '0'*11
    img = sensor.snapshot().lens_corr(1.8)

    ###### output format ######
    '''
    第0位 是否找到小球
    第1-4位 最近小球距离
    第5-9位 最近小球的方位角（乘100）
    第10位 最近小球左/右
    第11位 是否找到终点区域
    '''


    ###### 寻找小球 ######
    OrangeCircles = []
    ball_blobs = img.find_blobs([orange_threshold], x_stride=4, pixels_threshold=40, merge=True)

    if ball_blobs:
        for b in ball_blobs:
            if not BallinGreen(b):
                OrangeCircles.append(b)
                radius = (int)((b.w()+b.h()) / 4)
                img.draw_circle(b.cx(), b.cy(), radius, color=(0,255,0))


    if(len(OrangeCircles) != 0):
        #print(OrangeCircles)
        distance = []
        for o in OrangeCircles:
            dist = DistanceMeasurement((o.w()+o.h()) / 4)
            distance.append(dist)
        dist_min = min(distance)
        dist_min_i = distance.index(dist_min)
        angle = AngleMeasurement(OrangeCircles[dist_min_i])
        sign = 0    #angle -R1+L0
        if angle<=0:
            sign = 1
        else:
            sign = 0
        output_str = '1' + str("%04d" % dist_min) + str("%05d" %abs(angle)) + str(sign)
    #else:
        #print("Not Found!")

    ###### 寻找终点 ######
    green_blobs = img.find_blobs([green_threshold], x_stride=4, pixels_threshold=40, merge=True)
    if green_blobs:
        output_str += '1'
        for g in green_blobs:
            img.draw_rectangle(g.rect(), color=(255,0,0))
    else:
        output_str += '0'

    uart.write(output_str)
    print(output_str)
    time.sleep_ms(50)
