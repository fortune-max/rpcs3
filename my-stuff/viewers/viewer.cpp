#include <opencv2/opencv.hpp>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

// todo add frame rate

int main() {
    const char *shmName = "/vectframe_shm";
    const int width = 1280;
    const int height = 720;
    const int channels = 4;
    const size_t size = width * height * channels;

    int shm_fd = shm_open(shmName, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory: " << shmName << std::endl;
        return -1;
    }

    void *shm_ptr = mmap(0, size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shm_fd);
        return -1;
    }

    while (true) {
        cv::Mat image(height, width, CV_8UC4, shm_ptr);

        cv::imshow("Image", image);
        if (cv::waitKey(30) >= 0) break; // Exit on key press

        usleep(10000); // Sleep for 10ms
    }

    munmap(shm_ptr, size);
    close(shm_fd);
    return 0;
}
