#include "models/ScheduledClass.h"

namespace timetable {

ScheduledClass::ScheduledClass(int id, CoursePtr course, TeacherPtr teacher,
                               ClassGroupPtr classGroup, int durationSlots)
    : id_(id), course_(course), teacher_(teacher), classGroup_(classGroup),
      durationSlots_(durationSlots) {}

}
