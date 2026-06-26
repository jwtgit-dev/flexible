#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "models/TimeSlot.h"

namespace timetable {

enum class PreferenceLevel {
    StrongLike = 3,
    Like = 2,
    Neutral = 1,
    Dislike = 0,
    StrongDislike = -1
};

enum class DistributionPreference {
    Compact,
    Balanced,
    Spread
};

class TeacherPreference {
public:
    TeacherPreference();

    std::vector<DayOfWeek> preferredDays;
    std::vector<DayOfWeek> avoidedDays;
    std::vector<TimeSlotType> preferredSlotTypes;
    std::vector<TimeSlotType> avoidedSlotTypes;
    bool avoidEarlyMorning{false};

    int maxConsecutiveHours{4};
    int minGapBetweenClasses{0};
    int maxDailyHours{6};

    DistributionPreference distributionPref{DistributionPreference::Balanced};
    int preferredTeachingDays{3};

    double timePreferenceWeight{1.0};
    double intensityPreferenceWeight{1.0};
    double distributionPreferenceWeight{1.0};
};

class Teacher {
public:
    Teacher() = default;
    Teacher(int id, const std::string& name, const std::string& title = "",
            const std::string& department = "");

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getDepartment() const { return department_; }

    void setPreference(const TeacherPreference& pref) { preference_ = pref; }
    const TeacherPreference& getPreference() const { return preference_; }
    TeacherPreference& getPreference() { return preference_; }

    void addAvailableDay(DayOfWeek day) { availableDays_.insert(day); }
    bool isAvailableOn(DayOfWeek day) const { return availableDays_.count(day) > 0; }
    const std::unordered_set<DayOfWeek>& getAvailableDays() const { return availableDays_; }

    void addUnavailableSlot(int slotId) { unavailableSlots_.insert(slotId); }
    bool isSlotUnavailable(int slotId) const { return unavailableSlots_.count(slotId) > 0; }
    const std::unordered_set<int>& getUnavailableSlots() const { return unavailableSlots_; }

private:
    int id_{-1};
    std::string name_;
    std::string title_;
    std::string department_;
    TeacherPreference preference_;
    std::unordered_set<DayOfWeek> availableDays_;
    std::unordered_set<int> unavailableSlots_;
};

using TeacherPtr = std::shared_ptr<Teacher>;

}
