#include "constraints/IConstraint.h"
#include <algorithm>

namespace timetable {

void ConstraintManager::addConstraint(IConstraintPtr constraint) {
    constraints_.push_back(constraint);
}

void ConstraintManager::removeConstraint(const std::string& name) {
    constraints_.erase(
        std::remove_if(constraints_.begin(), constraints_.end(),
                       [&name](const IConstraintPtr& c) { return c->getName() == name; }),
        constraints_.end());
}

std::vector<IConstraintPtr> ConstraintManager::getHardConstraints() const {
    std::vector<IConstraintPtr> result;
    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Hard) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<IConstraintPtr> ConstraintManager::getSoftConstraints() const {
    std::vector<IConstraintPtr> result;
    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Soft) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<IConstraintPtr> ConstraintManager::getAllConstraints() const {
    return constraints_;
}

IConstraintPtr ConstraintManager::getConstraint(const std::string& name) const {
    for (const auto& c : constraints_) {
        if (c->getName() == name) {
            return c;
        }
    }
    return nullptr;
}

ConstraintResult ConstraintManager::evaluateAllHard(const TimetableDataPtr& data,
                                                    const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult total;
    total.satisfied = true;
    total.penalty = 0.0;

    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Hard) {
            auto result = c->evaluate(data, schedule);
            if (!result.satisfied) {
                total.satisfied = false;
                total.violationDetails.insert(
                    total.violationDetails.end(),
                    result.violationDetails.begin(),
                    result.violationDetails.end());
            }
        }
    }
    return total;
}

ConstraintResult ConstraintManager::evaluateAllSoft(const TimetableDataPtr& data,
                                                    const std::vector<ScheduledClassPtr>& schedule) const {
    ConstraintResult total;
    total.satisfied = true;
    total.penalty = 0.0;

    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Soft) {
            auto result = c->evaluate(data, schedule);
            total.penalty += result.penalty * c->getWeight();
        }
    }
    return total;
}

bool ConstraintManager::isFeasible(const TimetableDataPtr& data,
                                   const std::vector<ScheduledClassPtr>& schedule) const {
    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Hard) {
            auto result = c->evaluate(data, schedule);
            if (!result.satisfied) {
                return false;
            }
        }
    }
    return true;
}

double ConstraintManager::calculateTotalSoftPenalty(const TimetableDataPtr& data,
                                                    const std::vector<ScheduledClassPtr>& schedule) const {
    double total = 0.0;
    for (const auto& c : constraints_) {
        if (c->getType() == ConstraintType::Soft) {
            auto result = c->evaluate(data, schedule);
            total += result.penalty * c->getWeight();
        }
    }
    return total;
}

}
