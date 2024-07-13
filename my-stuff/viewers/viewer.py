#!/usr/bin/env python3
import os
import mmap
import numpy as np
import matplotlib.pyplot as plt

width = 1280
height = 720
channels = 4

shm_fd = os.open("/dev/shm/vectframe_shm", os.O_RDONLY)
shm = mmap.mmap(shm_fd, width * height * channels, access=mmap.ACCESS_READ)


try:
    while True:
        shm.seek(0)
        data = np.frombuffer(shm.read(width * height * channels), dtype=np.uint8)

        image_bgra = data.reshape((height, width, channels))
        image_rgba = image_bgra[:, :, [2, 1, 0, 3]]

        plt.imshow(image_rgba)
        plt.axis('off')
        plt.draw()
        plt.pause(0.01)
        plt.clf()
except KeyboardInterrupt:
    print("Stopped by user")

shm.close()
os.close(shm_fd)
