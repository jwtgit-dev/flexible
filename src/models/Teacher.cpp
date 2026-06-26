#include "models/Teacher.h"

namespace timetable {

TeacherPreference::TeacherPreference() = default;

Teacher::Teacher(int id, const std::string& name, const std::string& title,
                 const std::string& department)
    : id_(id), name_(name), title_(title), department_(department) {
    for (int d = 0; d < static_cast<int>(DayOfWeek::Friday) + 1; ++d) {
        availableDays_.insert(static_cast<DayOfWeek>(d));
    }
}

}
