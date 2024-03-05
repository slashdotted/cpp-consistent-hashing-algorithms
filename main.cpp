#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <configuration>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    try {
        YAML::Node yaml_file = YAML::LoadFile("../configs/" + filename);

        // Read algorithms
        YAML::Node algorithms = yaml_file["algorithms"];
        for (const auto& algorithm : algorithms) {
            std::string algorithm_name = algorithm["name"].as<std::string>();
            // Do something with algorithm_name
            std::cout << "Algorithm Name: " << algorithm_name << std::endl;
        }

        // Read benchmarks
        YAML::Node benchmarks = yaml_file["benchmarks"];
        for (const auto& benchmark : benchmarks) {
            std::string benchmark_name = benchmark["name"].as<std::string>();
            // Do something with benchmark_name
            std::cout << "Benchmark Name: " << benchmark_name << std::endl;

            // If the benchmark has arguments
            if (benchmark["args"]) {
                YAML::Node args = benchmark["args"];
                // Process args according to benchmark_name
                if (benchmark_name == "lookup-time") {
                    std::vector<std::string> distributions = args["distributions"].as<std::vector<std::string>>();
                    // Do something with distributions
                    std::cout << "Distributions: ";
                    for (const std::string& dist : distributions) {
                        std::cout << dist << " ";
                    }
                    std::cout << std::endl;
                } else if (benchmark_name == "monotonicity") {
                    std::vector<float> fractions = args["fractions"].as<std::vector<float>>();
                    // Do something with fractions
                    std::cout << "Fractions: ";
                    for (float fraction : fractions) {
                        std::cout << fraction << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
    } catch(const YAML::Exception& e) {
        std::cerr << "Error reading YAML file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
