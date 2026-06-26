#pragma once
#include "constraints/IConstraint.h"
#include <unordered_map>
#include <set>

namespace timetable {

class NoTeacherConflictConstraint : public IConstraint {
public:
    std::string getName() const override { return "NoTeacherConflict"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ResourceConflict; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class NoClassroomConflictConstraint : public IConstraint {
public:
    std::string getName() const override { return "NoClassroomConflict"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ResourceConflict; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class NoClassGroupConflictConstraint : public IConstraint {
public:
    std::string getName() const override { return "NoClassGroupConflict"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ResourceConflict; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class ClassroomSuitabilityConstraint : public IConstraint {
public:
    std::string getName() const override { return "ClassroomSuitability"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ClassroomSuitability; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class ClassroomCapacityConstraint : public IConstraint {
public:
    std::string getName() const override { return "ClassroomCapacity"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ClassroomSuitability; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class TeacherUnavailableConstraint : public IConstraint {
public:
    std::string getName() const override { return "TeacherUnavailable"; }
    ConstraintType getType() const override { return ConstraintType::Hard; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::TeacherAvailability; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

}
