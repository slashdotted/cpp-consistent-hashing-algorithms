
#include "dx/DxEngine.h"
#include "balance.cpp"

#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>
using namespace std;

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            cerr << "Usage: " << argv[0] << " <your_configuration>.yaml" << endl;
            return 1;
        }

        string filename = "../configs/" + string(argv[1]);
        YAML::Node config = YAML::LoadFile(filename);

        /**
         * Here starts the benchmark routine
        */
        for (const auto& algorithm : config["algorithms"]) {
            string name = algorithm["name"].as<string>();

            if (name == "dx") {
                return bench<DxEngine>(
                    name, "dx.log",
                    1000000, 1000000,
                    20000, 1000000
                );
            }
        }

    } catch (const YAML::Exception& e) {
        cerr << "YAML Exception: " << e.what() << endl;
        return 1;
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
