#include "jack_backend.h"
#include <stdexcept>

JackBackend::JackBackend() : client_(nullptr), input_port_(nullptr) {}

JackBackend::~JackBackend() {
    shutdown();
}

bool JackBackend::initialize(const std::string& client_name, const AudioFormat& format) {
    client_ = jack_client_open(client_name.c_str(), JackNullOption, nullptr);
    if (client_ == nullptr) {
        error_message_ = "Failed to create JACK client";
        return false;
    }

    jack_set_process_callback(client_, jack_process_callback, this);

    input_port_ = jack_port_register(client_, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if (input_port_ == nullptr) {
        error_message_ = "Failed to register JACK input port";
        jack_client_close(client_);
        client_ = nullptr;
        return false;
    }

    return true;
}

void JackBackend::shutdown() {
    if (client_ != nullptr) {
        jack_client_close(client_);
        client_ = nullptr;
        input_port_ = nullptr;
    }
}

bool JackBackend::set_process_callback(std::function<void(const float*, size_t)> callback) {
    process_callback_ = std::move(callback);
    return true;
}

bool JackBackend::activate() {
    if (client_ == nullptr) {
        error_message_ = "JACK client not initialized";
        return false;
    }
    return (jack_activate(client_) == 0);
}

bool JackBackend::deactivate() {
    if (client_ == nullptr) {
        error_message_ = "JACK client not initialized";
        return false;
    }
    return (jack_deactivate(client_) == 0);
}

AudioBackend::AudioFormat JackBackend::get_current_audio_format() const {
    if (client_ == nullptr) {
        throw std::runtime_error("JACK client not initialized");
    }
    return AudioFormat{
        static_cast<unsigned int>(jack_get_sample_rate(client_)),
        1,  // Assuming mono for simplicity
        AudioFormat::SampleFormat::Float32
    };
}

std::vector<AudioBackend::AudioFormat> JackBackend::get_supported_formats() const {
    // JACK supports various sample rates, but always uses float32
    return {
        {44100, 1, AudioFormat::SampleFormat::Float32},
        {48000, 1, AudioFormat::SampleFormat::Float32},
        {96000, 1, AudioFormat::SampleFormat::Float32}
    };
}

float JackBackend::get_cpu_load() const {
    return client_ ? jack_cpu_load(client_) : 0.0f;
}

size_t JackBackend::get_buffer_size() const {
    return client_ ? jack_get_buffer_size(client_) : 0;
}

bool JackBackend::set_buffer_size(size_t size) {
    if (client_ == nullptr) {
        error_message_ = "JACK client not initialized";
        return false;
    }
    return (jack_set_buffer_size(client_, size) == 0);
}

std::string JackBackend::get_error_message() const {
    return error_message_;
}

int JackBackend::jack_process_callback(jack_nframes_t nframes, void* arg) {
    auto* backend = static_cast<JackBackend*>(arg);
    if (backend->process_callback_) {
        const float* buffer = static_cast<const float*>(jack_port_get_buffer(backend->input_port_, nframes));
        backend->process_callback_(buffer, nframes);
    }
    return 0;
}