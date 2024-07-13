#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "sample_listener.hpp"
#include "util/video_sink.h"
extern "C" {
#include <libavutil/frame.h>
}


using namespace std;

const char* SHM_NAME_audio = "/vectsample_shm";
u_int8_t* shm_ptr_audio = nullptr;
int SHM_SIZE_audio;

bool send_sample(AVFrame *sample) {
    cout << "channels" << sample->channels << endl;
    cout << "nb_samples" << sample->nb_samples << endl;
    cout << "sample_rate: " << sample->sample_rate << endl;
    cout << "format: " << sample->format << endl;

    if (shm_ptr_audio == nullptr) {
        // SHM_SIZE_audio = frame->height * frame->width * 4;
        SHM_SIZE_audio = 1; // sample->sample_count * sample->channels * sizeof(f32);
        int shm_fd = shm_open(SHM_NAME_audio, O_CREAT | O_RDWR, 0666);
        if (shm_fd < 0) {
            std::cerr << "shm_open failed\n";
            return false;
        }

        int f_res = ftruncate(shm_fd, SHM_SIZE_audio);
        if (f_res < 0) {
            std::cerr << "ftruncate failed\n";
            return false;
        }

        shm_ptr_audio = static_cast<u_int8_t*>(mmap(0, SHM_SIZE_audio, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        if (shm_ptr_audio == MAP_FAILED) {
            std::cerr << "mmap failed, error -> " << errno << " " << strerror(errno) << "\n";
            return false;
        }
    }

    // memcpy(shm_ptr_audio, sample->data.data(), SHM_SIZE_audio);
    // cout << "sample copied" << endl;
    return true;
    // munmap(shm_ptr_audio, SHM_SIZE_audio);
}
