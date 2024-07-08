#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "frame_listener.hpp"
#include "util/video_sink.h"


using namespace std;

const char* SHM_NAME = "/vectframe_shm";
const int SHM_SIZE = 1920 * 1920;
u_int8_t* shm_ptr = nullptr;

bool send_frame(utils::video_sink::encoder_frame *frame) {
    int size = frame->data.size();
    cout << "frame size: " << size << ", timestamp_ms" << frame->timestamp_ms << endl;
    int total = 0;
    for (int i = 0; i < SHM_SIZE; i++)
        total += frame->data[i];
    cout << "total: " << total << endl;

    if (shm_ptr == nullptr) {
        // print meta data on frame
        cout << "height: " << frame->height << ", width: " << frame->width << ", pitch: " << frame->pitch << ", pixel_format: " << frame->av_pixel_format << endl;
        int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd < 0) {
            std::cerr << "shm_open failed\n";
            return false;
        }
        ftruncate(shm_fd, SHM_SIZE);

        shm_ptr = static_cast<u_int8_t*>(mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        if (shm_ptr == MAP_FAILED) {
            std::cerr << "mmap failed, error -> " << errno << " " << strerror(errno) << "\n";
            return false;
        }
    }


    memcpy(shm_ptr, frame->data.data(), SHM_SIZE);
    cout << "frame copied" << endl;

    // confirm sum matches
    total = 0;
    for (int i = 0; i < SHM_SIZE; i++)
        total += shm_ptr[i];
    cout << "shm total: " << total << endl;

    // write to a file as well
    FILE* f = fopen("frame.bin", "wb");
    fwrite(frame->data.data(), 1, 1920 * 1920, f);
    fclose(f);

    return true;
    // munmap(shm_ptr, SHM_SIZE);
}
