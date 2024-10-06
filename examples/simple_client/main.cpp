#include "audio_backend.h"
#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

// Simple audio processor that calculates RMS volume
class AudioProcessor {
public:
    void process(const float* buffer, size_t nframes) {
        float sum = 0.0f;
        for (size_t i = 0; i < nframes; ++i) {
            sum += buffer[i] * buffer[i];
        }
        rms_ = std::sqrt(sum / nframes);
    }

    float getRMS() const { return rms_; }

private:
    float rms_ = 0.0f;
};

std::atomic<bool> running(true);

void run_pipewire_loop(AudioBackend* backend) { backend->activate(); }

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <jack|pipewire>" << std::endl;
        return 1;
    }

    std::string backend_type = argv[1];
    std::unique_ptr<AudioBackend> audio_backend;

    try {
        audio_backend = create_audio_backend(backend_type);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error creating audio backend: " << e.what() << std::endl;
        return 1;
    }

    AudioProcessor processor;

    if (!audio_backend->initialize("SimpleClient",
                                   {48000, 1, AudioBackend::AudioFormat::SampleFormat::Float32})) {
        std::cerr << "Failed to initialize audio backend: " << audio_backend->get_error_message()
                  << std::endl;
        return 1;
    }

    audio_backend->set_process_callback(
        [&processor](const float* buffer, size_t nframes) { processor.process(buffer, nframes); });

    std::thread pipewire_thread;
    if (backend_type == "pipewire") {
        pipewire_thread = std::thread(run_pipewire_loop, audio_backend.get());
    } else {
        if (!audio_backend->activate()) {
            std::cerr << "Failed to activate audio backend: " << audio_backend->get_error_message()
                      << std::endl;
            return 1;
        }
    }

    std::cout << "Audio capture started. Press Ctrl+C to stop." << std::endl;

    // Set up signal handling to catch Ctrl+C
    std::signal(SIGINT, [](int) { running = false; });

    while (running) {
        float rms = processor.getRMS();
        float db = 20 * std::log10(rms);

        // Simple ASCII volume meter
        int meter_width = 50;
        int filled_width = static_cast<int>((db + 60) / 60 * meter_width);
        filled_width = std::max(0, std::min(meter_width, filled_width));

        std::cout << "\rVolume: [" << std::string(filled_width, '#')
                  << std::string(meter_width - filled_width, ' ') << "] " << std::fixed
                  << std::setprecision(1) << db << " dB" << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nStopping audio capture..." << std::endl;

    audio_backend->deactivate();
    if (pipewire_thread.joinable()) {
        pipewire_thread.join();
    }
    audio_backend->shutdown();

    return 0;
}
