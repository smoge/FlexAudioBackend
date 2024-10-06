#ifndef PIPEWIRE_BACKEND_H
#define PIPEWIRE_BACKEND_H

#include "audio_backend.h"
#include <pipewire/pipewire.h>

class PipeWireBackend : public AudioBackend {
public:
    PipeWireBackend();
    ~PipeWireBackend() override;

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
    pw_main_loop* loop_;
    pw_stream* stream_;
    std::function<void(const float*, size_t)> process_callback_;
    AudioFormat current_format_;
    std::string error_message_;

    pw_stream_events stream_events_;

    void setup_stream_events();

    static void on_process(void* userdata);
    static void on_stream_state_changed(void* userdata, pw_stream_state old_state,
                                        pw_stream_state state, const char* error);
};

#endif // PIPEWIRE_BACKEND_H