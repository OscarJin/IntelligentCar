import sensor, image, time

orange_threshold = (40, 90, 0, 45, 30, 100)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.HD)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
clock = time.clock()

def DistanceMeasurement(r):
    return 500/r    # modify const

while(True):
    clock.tick()
    OrangeCircles = []
    img = sensor.snapshot().lens_corr(1.8)
    blobs = img.find_blobs([orange_threshold])

    if blobs:
        for b in blobs:
           roi = (b[0:4])
           c_threshold = 3000
           while(c_threshold>0):
            circles = img.find_circles(roi=roi, threshold = c_threshold, x_margin = 10, y_margin = 10, r_margin = 10,
            r_min = 1, r_max = 100, r_step = 2)
            if(len(circles) == 1):
                c = circles[0]
                img.draw_circle(c.x(), c.y(), c.r(), color=(255,0,0))
                OrangeCircles.append(c)
                break
            c_threshold -= 100

    if(len(OrangeCircles) != 0):
        print(OrangeCircles)
        distance = []
        for o in OrangeCircles:
            dist = DistanceMeasurement(o.r())
            distance.append(dist)
        dist_min = min(distance)
        dist_min_i = distance.index(dist_min)
        print(dist_min)
    else:
        print("Not Found!")
