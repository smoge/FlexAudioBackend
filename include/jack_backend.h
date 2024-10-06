#ifndef JACK_BACKEND_H
#define JACK_BACKEND_H

#include "audio_backend.h"
#include <jack/jack.h>

class JackBackend : public AudioBackend {
public:
    JackBackend();
    ~JackBackend() override;

    bool initialize(const std::string& client_name, const AudioFormat& format) override;
    void shutdown() override;

    bool set_process_callback(std::function<void(const float*, size_t)> callback) override;
    bool activate() override;
    bool deactivate() override;

    AudioFormat get_current_audio_format() const override;
    std::vector<AudioFormat> get_supported_formats() const override;

    float get_cpu_load() const override;
    size_t get_buffer_size() const override;
    bool set_buffer_size(size_t size) override;

    std::string get_error_message() const override;

private:
    jack_client_t* client_;
    jack_port_t* input_port_;
    std::function<void(const float*, size_t)> process_callback_;
    std::string error_message_;

    static int jack_process_callback(jack_nframes_t nframes, void* arg);
};

#endif // JACK_BACKEND_H