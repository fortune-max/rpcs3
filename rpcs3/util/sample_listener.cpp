#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "sample_listener.hpp"
#include "util/video_sink.h"


using namespace std;

const char* SHM_NAME = "/vectsample_shm";
u_int8_t* shm_ptr = nullptr;
int SHM_SIZE;

bool send_sample(utils::video_sink::encoder_sample *sample) {
    if (shm_ptr == nullptr) {
        cout << "channels: " << sample->channels << endl;
        cout << "sample_count: " << sample->sample_count << endl;
        cout << "vect size" << sample->data.size() << endl;
        // SHM_SIZE = frame->height * frame->width * 4;
        // sample->data.size()
        cout << "suspected size: " << sample->sample_count * sample->channels * sizeof(f32) << endl;
        cout << "my size" << sample->data.size() << endl;
        SHM_SIZE = sample->sample_count * sample->channels * sizeof(f32);
        int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd < 0) {
            std::cerr << "shm_open failed\n";
            return false;
        }

        int f_res = ftruncate(shm_fd, SHM_SIZE);
        if (f_res < 0) {
            std::cerr << "ftruncate failed\n";
            return false;
        }

        shm_ptr = static_cast<u_int8_t*>(mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        if (shm_ptr == MAP_FAILED) {
            std::cerr << "mmap failed, error -> " << errno << " " << strerror(errno) << "\n";
            return false;
        }
    }

    memcpy(shm_ptr, sample->data.data(), SHM_SIZE);
    cout << "sample copied" << endl;
    return true;
    // munmap(shm_ptr, SHM_SIZE);
}
