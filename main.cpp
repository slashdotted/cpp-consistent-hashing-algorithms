#include <iostream>
#include <yaml-cpp/yaml.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <configuration>" << std::endl;
        return 1;
    }

    std::string algo = argv[1];
    std::string config = argv[2];

    try {
        YAML::Node yaml_file = YAML::LoadFile("../configs/config.yaml");
        YAML::Node yaml_config = yaml_file["balance"][config];

        if (yaml_config) {
            int anchor_set = yaml_config["anchor_set"].as<int>();
            int num_keys = yaml_config["num_keys"].as<int>();
            int num_removals = yaml_config["num_removals"].as<int>();
            int working_set = yaml_config["working_set"].as<int>();

            if (algo == "anchor") {
                return bench<AnchorEngine>("AnchorEngine", "anchor_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "dx") {
                return bench<DxEngine>("DxEngine", "dx_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "jump") {
                return bench<JumpEngine>("JumpEngine", "jump_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "maglev") {
                return bench<MaglevEngine>("MaglevEngine", "maglev_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "memento") {
                return bench<MementoEngine<boost::unordered_flat_map>>("MementoEngine<boost::unordered_flat_map>", "memento_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "power") {
                return bench<PowerEngine>("PowerEngine", "power_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else if (algo == "ring") {
                return bench<RingEngine>("RingEngine", "ring_balance.log", anchor_set, working_set, num_removals, num_keys);
            } else {
                std::cerr << "Algorithm <" << algo << "> not found." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Configuration <" << config << "> not found." << std::endl;
            return 1;
        }
    } catch(const YAML::Exception& e) {
        std::cerr << "Error reading YAML file: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
