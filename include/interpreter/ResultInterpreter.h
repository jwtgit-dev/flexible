#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "models/TimetableData.h"
#include "engine/TimetableEngine.h"

namespace timetable {

struct SatisfactionReport {
    double overallScore{0.0};
    double hardConstraintSatisfaction{100.0};
    double softConstraintSatisfaction{0.0};
    double teacherTimePreferenceRate{0.0};
    double teacherIntensityPreferenceRate{0.0};
    double teacherDistributionPreferenceRate{0.0};
    double classCompactnessScore{0.0};
    double classroomUtilizationScore{0.0};
    int totalHardConstraints{0};
    int satisfiedHardConstraints{0};
    int totalSoftRequirements{0};
    int satisfiedSoftRequirements{0};
    std::vector<std::string> unsatisfiedItems;
    std::vector<std::string> explanations;
    std::map<std::string, double> teacherScores;
};

struct ComparisonReport {
    int solutionIndex;
    std::string name;
    std::vector<double> objectiveValues;
    std::vector<std::string> highlights;
    std::vector<std::string> tradeoffs;
};

class ResultInterpreter {
public:
    ResultInterpreter(const TimetableDataPtr& data,
                    const ConstraintManagerPtr& constraintManager);

    SatisfactionReport analyze(const TimetableResult& result);

    std::vector<ComparisonReport> compareSolutions(const std::vector<TimetableResult>& results);

    std::string generateTextReport(const SatisfactionReport& report) const;
    std::string generateComparisonText(const std::vector<ComparisonReport>& comparisons) const;

    void printTimetableView(const TimetableResult& result);
    void printTeacherSchedule(const TimetableResult& result, int teacherId);
    void printClassSchedule(const TimetableResult& result, int classId);

private:
    double calculateTeacherSatisfaction(const TimetableResult& result, int teacherId);
    std::string explainUnsatisfiedConstraint(const std::string& constraintName,
                                               double penalty);

    TimetableDataPtr data_;
    ConstraintManagerPtr constraintManager_;
};

}
