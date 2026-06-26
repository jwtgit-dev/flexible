#include "constraints/HardConstraints.h"
#include <unordered_map>
#include <unordered_set>
#include <sstream>

namespace timetable {

ConstraintResult NoTeacherConflictConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    std::unordered_map<int, std::unordered_set<int>> teacherSlots;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int teacherId = sc->getTeacher()->getId();
        int slotId = sc->getTimeSlot()->getId();

        if (teacherSlots[teacherId].count(slotId) > 0) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "教师 " << sc->getTeacher()->getName()
                << " 在 " << sc->getTimeSlot()->getDisplayName()
                << " 有课程冲突";
            result.violationDetails.push_back(oss.str());
        } else {
            teacherSlots[teacherId].insert(slotId);
        }
    }
    return result;
}

ConstraintResult NoClassroomConflictConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    std::unordered_map<int, std::unordered_set<int>> classroomSlots;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int roomId = sc->getClassroom()->getId();
        int slotId = sc->getTimeSlot()->getId();

        if (classroomSlots[roomId].count(slotId) > 0) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "教室 " << sc->getClassroom()->getName()
                << " 在 " << sc->getTimeSlot()->getDisplayName()
                << " 有课程冲突";
            result.violationDetails.push_back(oss.str());
        } else {
            classroomSlots[roomId].insert(slotId);
        }
    }
    return result;
}

ConstraintResult NoClassGroupConflictConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    std::unordered_map<int, std::unordered_set<int>> classGroupSlots;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int classId = sc->getClassGroup()->getId();
        int slotId = sc->getTimeSlot()->getId();

        if (classGroupSlots[classId].count(slotId) > 0) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "班级 " << sc->getClassGroup()->getName()
                << " 在 " << sc->getTimeSlot()->getDisplayName()
                << " 有课程冲突";
            result.violationDetails.push_back(oss.str());
        } else {
            classGroupSlots[classId].insert(slotId);
        }
    }
    return result;
}

ConstraintResult ClassroomSuitabilityConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        auto course = sc->getCourse();
        auto room = sc->getClassroom();

        if (course->requiresLab() && room->getType() != ClassroomType::Laboratory
            && room->getType() != ClassroomType::ComputerRoom) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "课程 " << course->getName()
                << " 需要实验室，但安排在 " << room->getName()
                << "(" << Classroom::typeToString(room->getType()) << ")";
            result.violationDetails.push_back(oss.str());
        }
    }
    return result;
}

ConstraintResult ClassroomCapacityConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        auto course = sc->getCourse();
        auto room = sc->getClassroom();
        int students = course->getStudentCount() > 0
                           ? course->getStudentCount()
                           : sc->getClassGroup()->getStudentCount();

        if (students > room->getCapacity()) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "课程 " << course->getName()
                << " 学生人数 " << students
                << " 超过教室 " << room->getName()
                << " 容量 " << room->getCapacity();
            result.violationDetails.push_back(oss.str());
        }
    }
    return result;
}

ConstraintResult TeacherUnavailableConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        auto teacher = sc->getTeacher();
        auto slot = sc->getTimeSlot();

        if (!teacher->isAvailableOn(slot->getDay())) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "教师 " << teacher->getName()
                << " 在 " << TimeSlot::dayToString(slot->getDay())
                << " 不可用";
            result.violationDetails.push_back(oss.str());
        }

        if (teacher->isSlotUnavailable(slot->getId())) {
            result.satisfied = false;
            std::ostringstream oss;
            oss << "教师 " << teacher->getName()
                << " 在 " << slot->getDisplayName()
                << " 不可用";
            result.violationDetails.push_back(oss.str());
        }
    }
    return result;
}

}
