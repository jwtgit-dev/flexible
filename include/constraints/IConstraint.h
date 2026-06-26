#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "models/TimetableData.h"
#include "models/ScheduledClass.h"

namespace timetable {

enum class ConstraintType {
    Hard,
    Soft
};

enum class ConstraintCategory {
    ResourceConflict,
    TeacherAvailability,
    ClassroomSuitability,
    TimePreference,
    TeachingIntensity,
    CourseDistribution,
    ClassSchedule,
    SpecialRule
};

struct ConstraintResult {
    bool satisfied{true};
    double penalty{0.0};
    std::string description;
    std::vector<std::string> violationDetails;
};

class IConstraint {
public:
    virtual ~IConstraint() = default;
    virtual std::string getName() const = 0;
    virtual ConstraintType getType() const = 0;
    virtual ConstraintCategory getCategory() const = 0;
    virtual double getWeight() const = 0;
    virtual void setWeight(double weight) = 0;
    virtual ConstraintResult evaluate(const TimetableDataPtr& data,
                                       const std::vector<ScheduledClassPtr>& schedule) const = 0;
};

using IConstraintPtr = std::shared_ptr<IConstraint>;

class ConstraintManager {
public:
    void addConstraint(IConstraintPtr constraint);
    void removeConstraint(const std::string& name);

    std::vector<IConstraintPtr> getHardConstraints() const;
    std::vector<IConstraintPtr> getSoftConstraints() const;
    std::vector<IConstraintPtr> getAllConstraints() const;
    IConstraintPtr getConstraint(const std::string& name) const;

    ConstraintResult evaluateAllHard(const TimetableDataPtr& data,
                                      const std::vector<ScheduledClassPtr>& schedule) const;
    ConstraintResult evaluateAllSoft(const TimetableDataPtr& data,
                                      const std::vector<ScheduledClassPtr>& schedule) const;

    bool isFeasible(const TimetableDataPtr& data,
                    const std::vector<ScheduledClassPtr>& schedule) const;

    double calculateTotalSoftPenalty(const TimetableDataPtr& data,
                                     const std::vector<ScheduledClassPtr>& schedule) const;

private:
    std::vector<IConstraintPtr> constraints_;
};

using ConstraintManagerPtr = std::shared_ptr<ConstraintManager>;

}
