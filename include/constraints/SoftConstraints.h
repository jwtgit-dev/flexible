#pragma once
#include "constraints/IConstraint.h"
#include <unordered_map>

namespace timetable {

class TeacherTimePreferenceConstraint : public IConstraint {
public:
    std::string getName() const override { return "TeacherTimePreference"; }
    ConstraintType getType() const override { return ConstraintType::Soft; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::TimePreference; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class TeacherTeachingIntensityConstraint : public IConstraint {
public:
    std::string getName() const override { return "TeacherTeachingIntensity"; }
    ConstraintType getType() const override { return ConstraintType::Soft; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::TeachingIntensity; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class TeacherDistributionPreferenceConstraint : public IConstraint {
public:
    std::string getName() const override { return "TeacherDistributionPreference"; }
    ConstraintType getType() const override { return ConstraintType::Soft; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::CourseDistribution; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class ClassScheduleCompactnessConstraint : public IConstraint {
public:
    std::string getName() const override { return "ClassScheduleCompactness"; }
    ConstraintType getType() const override { return ConstraintType::Soft; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::ClassSchedule; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

class ClassroomUtilizationBalanceConstraint : public IConstraint {
public:
    std::string getName() const override { return "ClassroomUtilizationBalance"; }
    ConstraintType getType() const override { return ConstraintType::Soft; }
    ConstraintCategory getCategory() const override { return ConstraintCategory::SpecialRule; }
    double getWeight() const override { return weight_; }
    void setWeight(double w) override { weight_ = w; }
    ConstraintResult evaluate(const TimetableDataPtr& data,
                               const std::vector<ScheduledClassPtr>& schedule) const override;
private:
    double weight_{1.0};
};

}
