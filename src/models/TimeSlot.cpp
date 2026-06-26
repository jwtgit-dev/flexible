#include "models/TimeSlot.h"
#include <sstream>
#include <stdexcept>

namespace timetable {

TimeSlot::TimeSlot(int id, DayOfWeek day, int slotIndex, const std::string& startTime,
                   const std::string& endTime, TimeSlotType type)
    : id_(id), day_(day), slotIndex_(slotIndex), startTime_(startTime),
      endTime_(endTime), type_(type) {}

std::string TimeSlot::getDisplayName() const {
    std::ostringstream oss;
    oss << dayToString(day_) << " 第" << (slotIndex_ + 1) << "节(" << startTime_ << "-" << endTime_ << ")";
    return oss.str();
}

bool TimeSlot::isEarlyMorning() const {
    if (type_ != TimeSlotType::Morning) return false;
    int hour = std::stoi(startTime_.substr(0, 2));
    return hour <= 8;
}

std::string TimeSlot::dayToString(DayOfWeek day) {
    switch(day) {
        case DayOfWeek::Monday: return "周一";
        case DayOfWeek::Tuesday: return "周二";
        case DayOfWeek::Wednesday: return "周三";
        case DayOfWeek::Thursday: return "周四";
        case DayOfWeek::Friday: return "周五";
        case DayOfWeek::Saturday: return "周六";
        case DayOfWeek::Sunday: return "周日";
        default: return "未知";
    }
}

DayOfWeek TimeSlot::stringToDay(const std::string& str) {
    if (str == "周一" || str == "Monday") return DayOfWeek::Monday;
    if (str == "周二" || str == "Tuesday") return DayOfWeek::Tuesday;
    if (str == "周三" || str == "Wednesday") return DayOfWeek::Wednesday;
    if (str == "周四" || str == "Thursday") return DayOfWeek::Thursday;
    if (str == "周五" || str == "Friday") return DayOfWeek::Friday;
    if (str == "周六" || str == "Saturday") return DayOfWeek::Saturday;
    if (str == "周日" || str == "Sunday") return DayOfWeek::Sunday;
    throw std::invalid_argument("Invalid day string: " + str);
}

}
