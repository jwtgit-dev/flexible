#include "models/TimetableData.h"
#include <algorithm>

namespace timetable {

std::vector<TimeSlotPtr> TimetableData::getTimeSlotsByDay(DayOfWeek day) const {
    std::vector<TimeSlotPtr> result;
    for (const auto& slot : timeSlots_) {
        if (slot->getDay() == day) {
            result.push_back(slot);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const TimeSlotPtr& a, const TimeSlotPtr& b) {
                  return a->getSlotIndex() < b->getSlotIndex();
              });
    return result;
}

std::vector<ClassroomPtr> TimetableData::getClassroomsByType(ClassroomType type) const {
    std::vector<ClassroomPtr> result;
    for (const auto& room : classrooms_) {
        if (room->getType() == type) {
            result.push_back(room);
        }
    }
    return result;
}

}
