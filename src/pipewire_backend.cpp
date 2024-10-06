#include "pipewire_backend.h"
#include <spa/param/audio/format-utils.h>
#include <stdexcept>

PipeWireBackend::PipeWireBackend() : loop_(nullptr), stream_(nullptr) { pw_init(nullptr, nullptr); }

PipeWireBackend::~PipeWireBackend() {
    shutdown();
    pw_deinit();
}

bool PipeWireBackend::initialize(const std::string& client_name, const AudioFormat& format) {
    loop_ = pw_main_loop_new(nullptr);
    if (loop_ == nullptr) {
        error_message_ = "Failed to create PipeWire main loop";
        return false;
    }

    pw_properties* props = pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY,
                                             "Capture", PW_KEY_MEDIA_ROLE, "Music", PW_KEY_APP_NAME,
                                             client_name.c_str(), nullptr);

    if (!props) {
        error_message_ = "Failed to create PipeWire properties";
        return false;
    }

    stream_ = pw_stream_new_simple(pw_main_loop_get_loop(loop_), client_name.c_str(), props,
                                   &stream_events, this);

    if (stream_ == nullptr) {
        error_message_ = "Failed to create PipeWire stream";
        pw_main_loop_destroy(loop_);
        loop_ = nullptr;
        return false;
    }

    current_format_ = format;
    return true;
}

void PipeWireBackend::shutdown() {
    if (stream_ != nullptr) {
        pw_stream_destroy(stream_);
        stream_ = nullptr;
    }
    if (loop_ != nullptr) {
        pw_main_loop_destroy(loop_);
        loop_ = nullptr;
    }
}

bool PipeWireBackend::set_process_callback(std::function<void(const float*, size_t)> callback) {
    process_callback_ = std::move(callback);
    return true;
}

bool PipeWireBackend::activate() {
    if (stream_ == nullptr) {
        error_message_ = "PipeWire stream not initialized";
        return false;
    }

    uint8_t buffer[1024];
    spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const spa_pod* params[1];

    spa_audio_info_raw info = {};
    info.format = SPA_AUDIO_FORMAT_F32;
    info.channels = current_format_.channels;
    info.rate = current_format_.sample_rate;

    params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &info);

    return (pw_stream_connect(stream_, PW_DIRECTION_INPUT, PW_ID_ANY,
                              static_cast<pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT |
                                                           PW_STREAM_FLAG_MAP_BUFFERS),
                              params, 1) == 0);
}

bool PipeWireBackend::deactivate() {
    if (stream_ == nullptr) {
        error_message_ = "PipeWire stream not initialized";
        return false;
    }
    return (pw_stream_disconnect(stream_) == 0);
}

AudioBackend::AudioFormat PipeWireBackend::get_current_audio_format() const {
    return current_format_;
}

std::vector<AudioBackend::AudioFormat> PipeWireBackend::get_supported_formats() const {
    // This is a simplification. In reality, you'd query PipeWire for supported formats.
    return {{44100, 1, AudioFormat::SampleFormat::Float32},
            {48000, 1, AudioFormat::SampleFormat::Float32},
            {96000, 1, AudioFormat::SampleFormat::Float32}};
}

float PipeWireBackend::get_cpu_load() const {
    // PipeWire doesn't have a direct equivalent to JACK's CPU load.
    // You might need to implement your own CPU load calculation here.
    return 0.0f;
}

size_t PipeWireBackend::get_buffer_size() const {
    // This would typically be set during stream connection
    // For now, we'll return a default value
    return 1024;
}

bool PipeWireBackend::set_buffer_size(size_t size) {
    // Changing buffer size for an active PipeWire stream is not straightforward
    // You might need to reconnect the stream with new parameters
    error_message_ = "Changing buffer size not implemented for PipeWire backend";
    return false;
}

std::string PipeWireBackend::get_error_message() const { return error_message_; }

void PipeWireBackend::on_process(void* userdata, struct pw_buffer* buffer) {
    // Cast userdata to PipeWireBackend
    auto* backend = static_cast<PipeWireBackend*>(userdata);

    // Verify that the buffer is not null before proceeding
    if (buffer == nullptr)
        return;

    // Access the spa_buffer within the pw_buffer
    struct spa_buffer* buf = buffer->buffer;

    // Retrieve data from the buffer
    void* data = buf->datas[0].data;
    if (data == nullptr)
        return;

    // Determine the number of frames in the buffer
    size_t n_frames = buf->datas[0].chunk->size / sizeof(float);

    // Call the process callback if it is set
    if (backend->process_callback_) {
        backend->process_callback_(static_cast<const float*>(data), n_frames);
    }

    // Queue the buffer back onto the stream
    pw_stream_queue_buffer(backend->stream_, buffer);
}

void PipeWireBackend::on_stream_state_changed(void* userdata, pw_stream_state old_state,
                                              pw_stream_state state, const char* error) {
    auto* backend = static_cast<PipeWireBackend*>(userdata);
    if (state == PW_STREAM_STATE_ERROR) {
        backend->error_message_ = error ? error : "Unknown PipeWire stream error";
    }
}

const pw_stream_events PipeWireBackend::stream_events = {
    PW_VERSION_STREAM_EVENTS,
    nullptr,                                   // destroy
    &PipeWireBackend::on_stream_state_changed, // state_changed
    nullptr,                                   // control_info
    nullptr,                                   // io_changed
    nullptr,                                   // param_changed
    &PipeWireBackend::on_process,              // process
    nullptr,                                   // drained
    nullptr,                                   // command
    nullptr,                                   // trigger_done
};
