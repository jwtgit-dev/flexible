#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace timetable {

enum class DayOfWeek {
    Monday = 0,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday,
    COUNT
};

enum class TimeSlotType {
    Morning,
    Afternoon,
    Evening
};

class TimeSlot {
public:
    TimeSlot() = default;
    TimeSlot(int id, DayOfWeek day, int slotIndex, const std::string& startTime,
             const std::string& endTime, TimeSlotType type);

    int getId() const { return id_; }
    DayOfWeek getDay() const { return day_; }
    int getSlotIndex() const { return slotIndex_; }
    const std::string& getStartTime() const { return startTime_; }
    const std::string& getEndTime() const { return endTime_; }
    TimeSlotType getType() const { return type_; }
    std::string getDisplayName() const;

    static std::string dayToString(DayOfWeek day);
    static DayOfWeek stringToDay(const std::string& str);

    bool isMorning() const { return type_ == TimeSlotType::Morning; }
    bool isAfternoon() const { return type_ == TimeSlotType::Afternoon; }
    bool isEvening() const { return type_ == TimeSlotType::Evening; }
    bool isEarlyMorning() const;

private:
    int id_{-1};
    DayOfWeek day_{DayOfWeek::Monday};
    int slotIndex_{0};
    std::string startTime_;
    std::string endTime_;
    TimeSlotType type_{TimeSlotType::Morning};
};

using TimeSlotPtr = std::shared_ptr<TimeSlot>;

}
