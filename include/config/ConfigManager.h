#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "models/TimetableData.h"
#include "engine/TimetableEngine.h"

namespace timetable {

class ConfigManager {
public:
    ConfigManager();

    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    bool hasKey(const std::string& key) const;

    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getList(const std::string& key) const;

    SchedulingParams getSchedulingParams() const;
    void setSchedulingParams(const SchedulingParams& params);

    const std::map<std::string, std::string>& getAllSettings() const { return settings_; }

private:
    std::map<std::string, std::string> settings_;

    void setDefaults();
    static std::string trim(const std::string& s);
};

}
