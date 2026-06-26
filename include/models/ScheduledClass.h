#pragma once
#include <memory>
#include "models/Course.h"
#include "models/Teacher.h"
#include "models/ClassGroup.h"
#include "models/Classroom.h"
#include "models/TimeSlot.h"

namespace timetable {

class ScheduledClass {
public:
    ScheduledClass() = default;
    ScheduledClass(int id, CoursePtr course, TeacherPtr teacher,
                   ClassGroupPtr classGroup, int durationSlots);

    int getId() const { return id_; }
    CoursePtr getCourse() const { return course_; }
    TeacherPtr getTeacher() const { return teacher_; }
    ClassGroupPtr getClassGroup() const { return classGroup_; }
    int getDurationSlots() const { return durationSlots_; }

    bool isScheduled() const { return timeSlot_ != nullptr && classroom_ != nullptr; }
    TimeSlotPtr getTimeSlot() const { return timeSlot_; }
    ClassroomPtr getClassroom() const { return classroom_; }

    void setTimeSlot(TimeSlotPtr slot) { timeSlot_ = slot; }
    void setClassroom(ClassroomPtr room) { classroom_ = room; }

private:
    int id_{-1};
    CoursePtr course_;
    TeacherPtr teacher_;
    ClassGroupPtr classGroup_;
    int durationSlots_{2};
    TimeSlotPtr timeSlot_{nullptr};
    ClassroomPtr classroom_{nullptr};
};

using ScheduledClassPtr = std::shared_ptr<ScheduledClass>;

}
