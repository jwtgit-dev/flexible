#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>
#include "models/TimeSlot.h"
#include "models/Classroom.h"
#include "models/Course.h"
#include "models/ClassGroup.h"
#include "models/Teacher.h"
#include "models/ScheduledClass.h"

namespace timetable {

class TimetableData {
public:
    void addTimeSlot(TimeSlotPtr slot) { timeSlots_.push_back(slot); timeSlotMap_[slot->getId()] = slot; }
    void addClassroom(ClassroomPtr room) { classrooms_.push_back(room); classroomMap_[room->getId()] = room; }
    void addCourse(CoursePtr course) { courses_.push_back(course); courseMap_[course->getId()] = course; }
    void addClassGroup(ClassGroupPtr cls) { classGroups_.push_back(cls); classGroupMap_[cls->getId()] = cls; }
    void addTeacher(TeacherPtr teacher) { teachers_.push_back(teacher); teacherMap_[teacher->getId()] = teacher; }
    void addScheduledClass(ScheduledClassPtr sc) { scheduledClasses_.push_back(sc); scheduledClassMap_[sc->getId()] = sc; }

    const std::vector<TimeSlotPtr>& getTimeSlots() const { return timeSlots_; }
    const std::vector<ClassroomPtr>& getClassrooms() const { return classrooms_; }
    const std::vector<CoursePtr>& getCourses() const { return courses_; }
    const std::vector<ClassGroupPtr>& getClassGroups() const { return classGroups_; }
    const std::vector<TeacherPtr>& getTeachers() const { return teachers_; }
    const std::vector<ScheduledClassPtr>& getScheduledClasses() const { return scheduledClasses_; }

    TimeSlotPtr getTimeSlot(int id) const { return timeSlotMap_.count(id) ? timeSlotMap_.at(id) : nullptr; }
    ClassroomPtr getClassroom(int id) const { return classroomMap_.count(id) ? classroomMap_.at(id) : nullptr; }
    CoursePtr getCourse(int id) const { return courseMap_.count(id) ? courseMap_.at(id) : nullptr; }
    ClassGroupPtr getClassGroup(int id) const { return classGroupMap_.count(id) ? classGroupMap_.at(id) : nullptr; }
    TeacherPtr getTeacher(int id) const { return teacherMap_.count(id) ? teacherMap_.at(id) : nullptr; }
    ScheduledClassPtr getScheduledClass(int id) const { return scheduledClassMap_.count(id) ? scheduledClassMap_.at(id) : nullptr; }

    int getTimeSlotCount() const { return static_cast<int>(timeSlots_.size()); }
    int getClassroomCount() const { return static_cast<int>(classrooms_.size()); }
    int getScheduledClassCount() const { return static_cast<int>(scheduledClasses_.size()); }

    int getSlotsPerDay() const { return slotsPerDay_; }
    void setSlotsPerDay(int n) { slotsPerDay_ = n; }

    int getDayCount() const { return dayCount_; }
    void setDayCount(int n) { dayCount_ = n; }

    std::vector<TimeSlotPtr> getTimeSlotsByDay(DayOfWeek day) const;
    std::vector<ClassroomPtr> getClassroomsByType(ClassroomType type) const;

private:
    std::vector<TimeSlotPtr> timeSlots_;
    std::vector<ClassroomPtr> classrooms_;
    std::vector<CoursePtr> courses_;
    std::vector<ClassGroupPtr> classGroups_;
    std::vector<TeacherPtr> teachers_;
    std::vector<ScheduledClassPtr> scheduledClasses_;

    std::unordered_map<int, TimeSlotPtr> timeSlotMap_;
    std::unordered_map<int, ClassroomPtr> classroomMap_;
    std::unordered_map<int, CoursePtr> courseMap_;
    std::unordered_map<int, ClassGroupPtr> classGroupMap_;
    std::unordered_map<int, TeacherPtr> teacherMap_;
    std::unordered_map<int, ScheduledClassPtr> scheduledClassMap_;

    int slotsPerDay_{5};
    int dayCount_{5};
};

using TimetableDataPtr = std::shared_ptr<TimetableData>;

}
