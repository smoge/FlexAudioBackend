#include <iostream>

// Example of a hypothetical backend class
class PipeWireBackend {
public:
    // Static method simulating some processing
    static void on_process(void* userdata, void* buffer) {
        std::cout << "Processing data..." << std::endl;
    }

    // Method to simulate starting the backend
    void start() {
        std::cout << "Starting backend..." << std::endl;
    }
};

int main() {
    // Instantiate a backend object
    PipeWireBackend backend;

    // Start the backend
    backend.start();

    // Dummy data to simulate processing
    void* userdata = nullptr;
    void* buffer = nullptr;

    // Call the processing function
    PipeWireBackend::on_process(userdata, buffer);

    std::cout << "Simple client is running..." << std::endl;

    return 0;
}
