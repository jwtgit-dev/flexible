#include "config/ConfigManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace timetable {

ConfigManager::ConfigManager() {
    setDefaults();
}

void ConfigManager::setDefaults() {
    settings_["algorithm.population_size"] = "80";
    settings_["algorithm.max_generations"] = "150";
    settings_["algorithm.crossover_rate"] = "0.85";
    settings_["algorithm.mutation_rate"] = "0.15";
    settings_["algorithm.num_objectives"] = "3";
    settings_["algorithm.max_time_seconds"] = "120";
    settings_["algorithm.use_greedy_init"] = "true";

    settings_["weight.time_preference"] = "1.5";
    settings_["weight.intensity_preference"] = "1.2";
    settings_["weight.distribution_preference"] = "1.0";
    settings_["weight.compactness"] = "0.8";
    settings_["weight.utilization"] = "0.6";

    settings_["timetable.slots_per_day"] = "5";
    settings_["timetable.days_per_week"] = "5";
    settings_["timetable.start_time"] = "08:00";
    settings_["timetable.slot_duration_minutes"] = "90";
}

std::string ConfigManager::trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot open config file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));
        settings_[key] = value;
    }

    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot write config file: " << filepath << std::endl;
        return false;
    }

    file << "# Flexible Timetabling System Configuration\n";
    file << "# ===========================================\n\n";

    std::string lastSection;
    for (const auto& [key, value] : settings_) {
        size_t dotPos = key.find('.');
        std::string section = (dotPos != std::string::npos) ? key.substr(0, dotPos) : "";

        if (section != lastSection) {
            file << "\n# " << section << "\n";
            lastSection = section;
        }

        file << key << " = " << value << "\n";
    }

    return true;
}

void ConfigManager::set(const std::string& key, const std::string& value) {
    settings_[key] = value;
}

std::string ConfigManager::get(const std::string& key, const std::string& defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool ConfigManager::hasKey(const std::string& key) const {
    return settings_.find(key) != settings_.end();
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        try {
            return std::stod(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        const std::string& v = it->second;
        return v == "true" || v == "1" || v == "yes" || v == "on";
    }
    return defaultValue;
}

std::vector<std::string> ConfigManager::getList(const std::string& key) const {
    std::vector<std::string> result;
    auto it = settings_.find(key);
    if (it == settings_.end()) {
        return result;
    }

    std::istringstream iss(it->second);
    std::string item;
    while (std::getline(iss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

SchedulingParams ConfigManager::getSchedulingParams() const {
    SchedulingParams params;
    params.populationSize = getInt("algorithm.population_size", params.populationSize);
    params.maxGenerations = getInt("algorithm.max_generations", params.maxGenerations);
    params.crossoverRate = getDouble("algorithm.crossover_rate", params.crossoverRate);
    params.mutationRate = getDouble("algorithm.mutation_rate", params.mutationRate);
    params.numObjectives = getInt("algorithm.num_objectives", params.numObjectives);
    params.maxTimeSeconds = getInt("algorithm.max_time_seconds", params.maxTimeSeconds);
    params.useGreedyInit = getBool("algorithm.use_greedy_init", params.useGreedyInit);
    params.timePreferenceWeight = getDouble("weight.time_preference", params.timePreferenceWeight);
    params.intensityPreferenceWeight = getDouble("weight.intensity_preference", params.intensityPreferenceWeight);
    params.distributionPreferenceWeight = getDouble("weight.distribution_preference", params.distributionPreferenceWeight);
    params.compactnessWeight = getDouble("weight.compactness", params.compactnessWeight);
    params.utilizationWeight = getDouble("weight.utilization", params.utilizationWeight);
    return params;
}

void ConfigManager::setSchedulingParams(const SchedulingParams& params) {
    set("algorithm.population_size", std::to_string(params.populationSize));
    set("algorithm.max_generations", std::to_string(params.maxGenerations));
    set("algorithm.crossover_rate", std::to_string(params.crossoverRate));
    set("algorithm.mutation_rate", std::to_string(params.mutationRate));
    set("algorithm.num_objectives", std::to_string(params.numObjectives));
    set("algorithm.max_time_seconds", std::to_string(params.maxTimeSeconds));
    set("algorithm.use_greedy_init", params.useGreedyInit ? "true" : "false");
    set("weight.time_preference", std::to_string(params.timePreferenceWeight));
    set("weight.intensity_preference", std::to_string(params.intensityPreferenceWeight));
    set("weight.distribution_preference", std::to_string(params.distributionPreferenceWeight));
    set("weight.compactness", std::to_string(params.compactnessWeight));
    set("weight.utilization", std::to_string(params.utilizationWeight));
}

}
