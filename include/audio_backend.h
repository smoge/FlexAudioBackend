#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class AudioBackend {
public:
    struct AudioFormat {
        unsigned int sample_rate;
        unsigned int channels;
        enum class SampleFormat { Float32, Int16, Int32 } format;
    };

    virtual ~AudioBackend() = default;

    virtual bool initialize(const std::string& client_name, const AudioFormat& format) = 0;
    virtual void shutdown() = 0;

    virtual bool set_process_callback(std::function<void(const float*, size_t)> callback) = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;

    virtual AudioFormat get_current_audio_format() const = 0;
    virtual std::vector<AudioFormat> get_supported_formats() const = 0;

    virtual float get_cpu_load() const = 0;
    virtual size_t get_buffer_size() const = 0;
    virtual bool set_buffer_size(size_t size) = 0;

    virtual std::string get_error_message() const = 0;
};

// Factory function declaration
std::unique_ptr<AudioBackend> create_audio_backend(const std::string& backend_type);

#endif // AUDIO_BACKEND_H