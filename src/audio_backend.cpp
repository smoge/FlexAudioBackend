#include "audio_backend.h"
#include "jack_backend.h"
#include "pipewire_backend.h"
#include <stdexcept>

std::unique_ptr<AudioBackend> create_audio_backend(const std::string& backend_type) {
    if (backend_type == "jack") {
        return std::make_unique<JackBackend>();
    } else if (backend_type == "pipewire") {
        return std::make_unique<PipeWireBackend>();
    } else {
        throw std::runtime_error("Unknown audio backend type: " + backend_type);
    }
}