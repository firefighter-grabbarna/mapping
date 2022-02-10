#!/usr/bin/env python3

import pickle
from breezyslam.algorithms import RMHC_SLAM
from breezyslam.sensors import URG04LX

import pygame

pygame.init()

surface = pygame.display.set_mode((400, 400))

scan_data = pickle.load(open("data/output7.pickle", "rb"))


lidar = URG04LX()

mapbytes = bytearray(400*400)

slam = RMHC_SLAM(lidar, 400, 4) 

for scan in scan_data:
    for event in pygame.event.get():
        pass
    
    #scan = readLidar()

    # min_angle = 0
    # max_angle = 240
    # angles = [min_angle + (max_angle - min_angle) * i / len(scan) for i in range(len(scan))]

    #print(angles)

    slam.update(scan) #, scan_angles_degrees=angles)

    x, y, theta = slam.getpos()
    print(x, y, theta)

    slam.getmap(mapbytes)

    pixarray = pygame.PixelArray(surface)

    for y in range(400):
        for x in range(400):
            color = mapbytes[x + y * 400]
            pixarray[x, y] = (color, color, color)
    
    pixarray.close()

    pygame.display.update()