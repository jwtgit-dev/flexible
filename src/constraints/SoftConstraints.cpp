#include "constraints/SoftConstraints.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <numeric>

namespace timetable {

static bool isInDays(const std::vector<DayOfWeek>& days, DayOfWeek day) {
    return std::find(days.begin(), days.end(), day) != days.end();
}

static bool isInSlotTypes(const std::vector<TimeSlotType>& types, TimeSlotType type) {
    return std::find(types.begin(), types.end(), type) != types.end();
}

ConstraintResult TeacherTimePreferenceConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;
    result.penalty = 0.0;

    int totalClasses = 0;
    int satisfiedClasses = 0;

    std::unordered_map<int, std::vector<TimeSlotPtr>> teacherSchedule;
    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        teacherSchedule[sc->getTeacher()->getId()].push_back(sc->getTimeSlot());
    }

    for (const auto& [teacherId, slots] : teacherSchedule) {
        auto teacher = data->getTeacher(teacherId);
        if (!teacher) continue;
        const auto& pref = teacher->getPreference();

        for (const auto& slot : slots) {
            totalClasses++;
            bool satisfied = true;
            double penalty = 0.0;

            if (!pref.preferredDays.empty() && !isInDays(pref.preferredDays, slot->getDay())) {
                penalty += 1.0;
                satisfied = false;
            }
            if (isInDays(pref.avoidedDays, slot->getDay())) {
                penalty += 2.0;
                satisfied = false;
            }

            if (!pref.preferredSlotTypes.empty() && !isInSlotTypes(pref.preferredSlotTypes, slot->getType())) {
                penalty += 0.5;
                satisfied = false;
            }
            if (isInSlotTypes(pref.avoidedSlotTypes, slot->getType())) {
                penalty += 1.5;
                satisfied = false;
            }

            if (pref.avoidEarlyMorning && slot->isEarlyMorning()) {
                penalty += 1.5;
                satisfied = false;
            }

            result.penalty += penalty * pref.timePreferenceWeight;
            if (satisfied) satisfiedClasses++;
        }
    }

    result.description = "教师时间偏好满足率: " + std::to_string(
        totalClasses > 0 ? (satisfiedClasses * 100.0 / totalClasses) : 100.0) + "%";
    return result;
}

ConstraintResult TeacherTeachingIntensityConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;
    result.penalty = 0.0;

    std::unordered_map<int, std::unordered_map<DayOfWeek, std::vector<int>>> teacherDailySlots;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int teacherId = sc->getTeacher()->getId();
        DayOfWeek day = sc->getTimeSlot()->getDay();
        int slotIdx = sc->getTimeSlot()->getSlotIndex();
        teacherDailySlots[teacherId][day].push_back(slotIdx);
    }

    for (auto& [teacherId, dayMap] : teacherDailySlots) {
        auto teacher = data->getTeacher(teacherId);
        if (!teacher) continue;
        const auto& pref = teacher->getPreference();

        for (auto& [day, slotIndices] : dayMap) {
            std::sort(slotIndices.begin(), slotIndices.end());

            if (static_cast<int>(slotIndices.size()) > pref.maxDailyHours) {
                result.penalty += (slotIndices.size() - pref.maxDailyHours) * 2.0
                                  * pref.intensityPreferenceWeight;
            }

            int consecutive = 1;
            int maxConsecutive = 1;
            for (size_t i = 1; i < slotIndices.size(); ++i) {
                if (slotIndices[i] == slotIndices[i-1] + 1) {
                    consecutive++;
                    maxConsecutive = std::max(maxConsecutive, consecutive);
                } else {
                    int gap = slotIndices[i] - slotIndices[i-1] - 1;
                    if (gap < pref.minGapBetweenClasses) {
                        result.penalty += (pref.minGapBetweenClasses - gap) * 1.0
                                          * pref.intensityPreferenceWeight;
                    }
                    consecutive = 1;
                }
            }

            if (maxConsecutive > pref.maxConsecutiveHours) {
                result.penalty += (maxConsecutive - pref.maxConsecutiveHours) * 1.5
                                  * pref.intensityPreferenceWeight;
            }
        }
    }

    return result;
}

ConstraintResult TeacherDistributionPreferenceConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;
    result.penalty = 0.0;

    std::unordered_map<int, std::unordered_set<DayOfWeek>> teacherTeachingDays;
    std::unordered_map<int, std::unordered_map<DayOfWeek, int>> teacherDayHours;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int teacherId = sc->getTeacher()->getId();
        DayOfWeek day = sc->getTimeSlot()->getDay();
        teacherTeachingDays[teacherId].insert(day);
        teacherDayHours[teacherId][day] += sc->getDurationSlots();
    }

    for (const auto& [teacherId, days] : teacherTeachingDays) {
        auto teacher = data->getTeacher(teacherId);
        if (!teacher) continue;
        const auto& pref = teacher->getPreference();

        if (pref.distributionPref == DistributionPreference::Compact) {
            if (static_cast<int>(days.size()) > pref.preferredTeachingDays) {
                result.penalty += (days.size() - pref.preferredTeachingDays) * 3.0
                                  * pref.distributionPreferenceWeight;
            }
        } else if (pref.distributionPref == DistributionPreference::Balanced
                   || pref.distributionPref == DistributionPreference::Spread) {
            const auto& dayHours = teacherDayHours[teacherId];
            if (!dayHours.empty()) {
                int totalHours = 0;
                for (const auto& [d, h] : dayHours) totalHours += h;
                double avgHours = static_cast<double>(totalHours) / dayHours.size();

                double variance = 0.0;
                for (const auto& [d, h] : dayHours) {
                    variance += std::pow(h - avgHours, 2);
                }
                variance /= dayHours.size();
                result.penalty += std::sqrt(variance) * 1.0 * pref.distributionPreferenceWeight;
            }
        }
    }

    return result;
}

ConstraintResult ClassScheduleCompactnessConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;
    result.penalty = 0.0;

    std::unordered_map<int, std::unordered_map<DayOfWeek, std::vector<int>>> classDailySlots;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        int classId = sc->getClassGroup()->getId();
        DayOfWeek day = sc->getTimeSlot()->getDay();
        int slotIdx = sc->getTimeSlot()->getSlotIndex();
        classDailySlots[classId][day].push_back(slotIdx);
    }

    for (auto& [classId, dayMap] : classDailySlots) {
        for (auto& [day, slotIndices] : dayMap) {
            if (slotIndices.size() <= 1) continue;
            std::sort(slotIndices.begin(), slotIndices.end());

            int totalGap = 0;
            for (size_t i = 1; i < slotIndices.size(); ++i) {
                int gap = slotIndices[i] - slotIndices[i-1] - 1;
                if (gap > 0) {
                    totalGap += gap;
                }
            }
            result.penalty += totalGap * 0.5;
        }
    }

    return result;
}

ConstraintResult ClassroomUtilizationBalanceConstraint::evaluate(
        const TimetableDataPtr& data,
        const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult result;
    result.satisfied = true;
    result.penalty = 0.0;

    std::unordered_map<int, int> classroomUsage;

    for (const auto& sc : schedule) {
        if (!sc->isScheduled()) continue;
        classroomUsage[sc->getClassroom()->getId()] += sc->getDurationSlots();
    }

    if (classroomUsage.empty()) return result;

    int totalUsage = 0;
    for (const auto& [id, usage] : classroomUsage) {
        totalUsage += usage;
    }
    double avgUsage = static_cast<double>(totalUsage) / data->getClassroomCount();

    double variance = 0.0;
    for (const auto& room : data->getClassrooms()) {
        int usage = classroomUsage.count(room->getId()) ? classroomUsage[room->getId()] : 0;
        variance += std::pow(usage - avgUsage, 2);
    }
    variance /= data->getClassroomCount();
    result.penalty = std::sqrt(variance) * 0.3;

    return result;
}

}
