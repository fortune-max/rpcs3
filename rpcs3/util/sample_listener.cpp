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

bool send_sample(AVFrame *frame) {
    cout << "channels" << frame->channels << endl;
    cout << "nb_samples" << frame->nb_samples << endl;
    cout << "format: " << frame->format << endl;

    if (shm_ptr_audio == nullptr) {
        SHM_SIZE_audio = frame->nb_samples * frame->ch_layout.nb_channels * sizeof(f32);
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

    for (int ch = 0; ch < frame->channels; ch++) {
        memcpy(shm_ptr_audio + ch * frame->nb_samples * sizeof(f32), frame->data[ch], frame->nb_samples * sizeof(f32));
    }

    FILE *f = fopen("audio.raw", "wb");
    if (f == NULL) {
        std::cerr << "Error opening file!\n";
        return false;
    }
    for (int ch = 0; ch < frame->channels; ch++) {
        fwrite(frame->data[ch], sizeof(f32), frame->nb_samples, f);
    }
    fclose(f);
    // cout << "frame copied" << endl;
    return true;
    // munmap(shm_ptr_audio, SHM_SIZE_audio);
}
