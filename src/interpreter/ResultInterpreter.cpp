#include "interpreter/ResultInterpreter.h"
#include "constraints/SoftConstraints.h"
#include "constraints/HardConstraints.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <limits>

namespace timetable {

ResultInterpreter::ResultInterpreter(const TimetableDataPtr& data,
                                   const ConstraintManagerPtr& constraintManager)
    : data_(data), constraintManager_(constraintManager) {}

SatisfactionReport ResultInterpreter::analyze(const TimetableResult& result) {
    SatisfactionReport report;

    auto hardConstraints = constraintManager_->getHardConstraints();
    auto softConstraints = constraintManager_->getSoftConstraints();

    report.totalHardConstraints = static_cast<int>(hardConstraints.size());

    auto hardResult = constraintManager_->evaluateAllHard(data_, result.schedule);
    report.hardConstraintSatisfaction = hardResult.satisfied ? 100.0 : 0.0;
    report.satisfiedHardConstraints = hardResult.satisfied ? report.totalHardConstraints : 0;
    report.unsatisfiedItems = hardResult.violationDetails;

    double totalSoftPenalty = 0.0;
    double maxPossiblePenalty = 1.0;

    for (const auto& c : softConstraints) {
        auto eval = c->evaluate(data_, result.schedule);
        totalSoftPenalty += eval.penalty * c->getWeight();

        if (c->getName() == "TeacherTimePreference") {
            int totalClasses = 0;
            int satisfied = 0;
            std::unordered_map<int, int> teacherClasses;
            std::unordered_map<int, int> teacherSatisfied;

            for (const auto& sc : result.schedule) {
                if (!sc->isScheduled()) continue;
                auto teacher = sc->getTeacher();
                auto slot = sc->getTimeSlot();
                const auto& pref = teacher->getPreference();
                totalClasses++;
                teacherClasses[teacher->getId()]++;

                bool ok = true;
                if (!pref.preferredDays.empty()) {
                    bool found = false;
                    for (auto d : pref.preferredDays) {
                        if (d == slot->getDay()) { found = true; break; }
                    }
                    if (!found) ok = false;
                }
                for (auto d : pref.avoidedDays) {
                    if (d == slot->getDay()) { ok = false; break; }
                }
                if (pref.avoidEarlyMorning && slot->isEarlyMorning()) {
                    ok = false;
                }

                if (ok) {
                    satisfied++;
                    teacherSatisfied[teacher->getId()]++;
                }
            }

            report.teacherTimePreferenceRate = totalClasses > 0
                ? (satisfied * 100.0 / totalClasses) : 100.0;

            for (const auto& [tid, count] : teacherClasses) {
                double score = teacherSatisfied.count(tid)
                    ? (teacherSatisfied[tid] * 100.0 / count) : 0.0;
                auto teacher = data_->getTeacher(tid);
                if (teacher) {
                    report.teacherScores[teacher->getName()] = score;
                }
            }
        }
    }

    double satRate = totalSoftPenalty < 0.01 ? 100.0 : std::max(0.0, 100.0 - totalSoftPenalty * 2.0);
    report.softConstraintSatisfaction = satRate;
    report.totalSoftRequirements = 5;
    report.satisfiedSoftRequirements = static_cast<int>(satRate / 20.0);

    report.overallScore = report.hardConstraintSatisfaction * 0.6
                        + report.softConstraintSatisfaction * 0.4;

    if (!result.objectiveValues.empty()) {
        report.classCompactnessScore = std::max(0.0, 100.0 - result.objectiveValues[1] * 5.0);
        if (result.objectiveValues.size() >= 3) {
            report.classroomUtilizationScore = std::max(0.0, 100.0 - result.objectiveValues[2] * 10.0);
        }
    }

    for (const auto& c : softConstraints) {
        auto it = result.constraintScores.find(c->getName());
        if (it != result.constraintScores.end() && it->second > 0.1) {
            report.explanations.push_back(explainUnsatisfiedConstraint(c->getName(), it->second));
        }
    }

    return report;
}

std::string ResultInterpreter::explainUnsatisfiedConstraint(const std::string& constraintName,
                                                              double penalty) {
    std::ostringstream oss;
    if (constraintName == "TeacherTimePreference") {
        oss << "部分教师的时间偏好未完全满足（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。"
            << "可能原因：1) 部分教师偏好的时间段资源紧张，被迫安排在非偏好时段；"
            << "2) 教师期望避免的时段因课程数量多、教室资源有限而无法完全回避。"
            << "建议：调整部分教师的时间偏好权重，或增加可用教室资源。";
    } else if (constraintName == "TeacherTeachingIntensity") {
        oss << "部分教师的教学强度偏好未完全满足（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。"
            << "可能原因：1) 部分教师日课时数超过期望上限；"
            << "2) 连续课时数超出教师可接受范围。"
            << "建议：降低每日排课密度，或在课程间预留更多间隔。";
    } else if (constraintName == "TeacherDistributionPreference") {
        oss << "部分教师的课程分布偏好未完全满足（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。"
            << "可能原因：教师期望的教学日数量与课程总量存在冲突；"
            << "紧凑/均匀分布需求与资源约束存在权衡。"
            << "建议：调整教师分布偏好的权重参数。";
    } else if (constraintName == "ClassScheduleCompactness") {
        oss << "班级课表紧凑性有待提升（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。"
            << "可能原因：公共基础课与专业课时间安排存在冲突，导致班级课表出现较多空档。"
            << "建议：优化公共课排课策略，或适当放宽紧凑性要求。";
    } else if (constraintName == "ClassroomUtilizationBalance") {
        oss << "教室资源利用均衡性有待改进（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。"
            << "可能原因：部分教室容量或类型与课程需求匹配度不同，导致使用频率差异较大。"
            << "建议：调整教室分配策略，或优化课程-教室匹配规则。";
    } else {
        oss << constraintName << "存在未满足项（惩罚值: " << std::fixed << std::setprecision(2) << penalty << "）。";
    }
    return oss.str();
}

std::vector<ComparisonReport> ResultInterpreter::compareSolutions(
        const std::vector<TimetableResult>& results) {
    std::vector<ComparisonReport> reports;

    for (size_t i = 0; i < results.size(); ++i) {
        ComparisonReport cr;
        cr.solutionIndex = static_cast<int>(i);
        cr.name = "方案 " + std::to_string(i + 1);
        cr.objectiveValues = results[i].objectiveValues;

        if (!results[i].objectiveValues.empty()) {
            double minObj0 = std::numeric_limits<double>::max();
            double minObj1 = std::numeric_limits<double>::max();
            double minObj2 = std::numeric_limits<double>::max();
            for (const auto& r : results) {
                if (r.objectiveValues.size() > 0) minObj0 = std::min(minObj0, r.objectiveValues[0]);
                if (r.objectiveValues.size() > 1) minObj1 = std::min(minObj1, r.objectiveValues[1]);
                if (r.objectiveValues.size() > 2) minObj2 = std::min(minObj2, r.objectiveValues[2]);
            }

            if (results[i].objectiveValues[0] <= minObj0 + 0.001) {
                cr.highlights.push_back("教师满意度最高");
            } else {
                cr.tradeoffs.push_back("教师满意度有所降低");
            }

            if (results[i].objectiveValues.size() > 1
                && results[i].objectiveValues[1] <= minObj1 + 0.001) {
                cr.highlights.push_back("班级课表最紧凑");
            } else {
                cr.tradeoffs.push_back("班级课表碎片化程度略高");
            }

            if (results[i].objectiveValues.size() > 2
                && results[i].objectiveValues[2] <= minObj2 + 0.001) {
                cr.highlights.push_back("教室利用最均衡");
            } else {
                cr.tradeoffs.push_back("教室利用率均衡性略低");
            }
        }

        reports.push_back(cr);
    }

    return reports;
}

std::string ResultInterpreter::generateTextReport(const SatisfactionReport& report) const {
    std::ostringstream oss;
    oss << "=============================================\n";
    oss << "       排课方案满意度分析报告\n";
    oss << "=============================================\n\n";

    oss << "【总体评分】\n";
    oss << "  综合满意度评分: " << std::fixed << std::setprecision(1) << report.overallScore << " / 100\n";
    oss << "  硬约束满足率: " << std::fixed << std::setprecision(1) << report.hardConstraintSatisfaction << "%\n";
    oss << "  软需求满足率: " << std::fixed << std::setprecision(1) << report.softConstraintSatisfaction << "%\n\n";

    oss << "【各维度评分】\n";
    oss << "  教师时间偏好满足率: " << std::fixed << std::setprecision(1)
        << report.teacherTimePreferenceRate << "%\n";
    oss << "  班级课表紧凑性评分: " << std::fixed << std::setprecision(1)
        << report.classCompactnessScore << " / 100\n";
    oss << "  教室资源利用均衡性: " << std::fixed << std::setprecision(1)
        << report.classroomUtilizationScore << " / 100\n\n";

    if (!report.teacherScores.empty()) {
        oss << "【各教师满意度】\n";
        for (const auto& [name, score] : report.teacherScores) {
            oss << "  " << std::left << std::setw(12) << name
                << ": " << std::fixed << std::setprecision(1) << score << "%\n";
        }
        oss << "\n";
    }

    if (!report.explanations.empty()) {
        oss << "【未满足项说明】\n";
        for (size_t i = 0; i < report.explanations.size(); ++i) {
            oss << "  " << (i + 1) << ". " << report.explanations[i] << "\n";
        }
        oss << "\n";
    }

    if (!report.unsatisfiedItems.empty()) {
        oss << "【硬约束冲突详情】\n";
        for (size_t i = 0; i < report.unsatisfiedItems.size() && i < 10; ++i) {
            oss << "  - " << report.unsatisfiedItems[i] << "\n";
        }
        if (report.unsatisfiedItems.size() > 10) {
            oss << "  ... 共 " << report.unsatisfiedItems.size() << " 项冲突\n";
        }
        oss << "\n";
    }

    oss << "=============================================\n";
    return oss.str();
}

std::string ResultInterpreter::generateComparisonText(
        const std::vector<ComparisonReport>& comparisons) const {
    std::ostringstream oss;
    oss << "=============================================\n";
    oss << "       帕累托最优方案对比\n";
    oss << "=============================================\n\n";

    for (const auto& comp : comparisons) {
        oss << "◆ " << comp.name << "\n";
        oss << "  目标值: [";
        for (size_t i = 0; i < comp.objectiveValues.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << std::fixed << std::setprecision(2) << comp.objectiveValues[i];
        }
        oss << "]\n";

        if (!comp.highlights.empty()) {
            oss << "  优势: ";
            for (size_t i = 0; i < comp.highlights.size(); ++i) {
                if (i > 0) oss << "、";
                oss << comp.highlights[i];
            }
            oss << "\n";
        }

        if (!comp.tradeoffs.empty()) {
            oss << "  权衡: ";
            for (size_t i = 0; i < comp.tradeoffs.size(); ++i) {
                if (i > 0) oss << "、";
                oss << comp.tradeoffs[i];
            }
            oss << "\n";
        }
        oss << "\n";
    }

    oss << "说明：帕累托最优解集中的每个方案在不同目标间各有取舍，\n";
    oss << "      请根据实际管理需求选择最合适的方案。\n";
    oss << "=============================================\n";
    return oss.str();
}

void ResultInterpreter::printTimetableView(const TimetableResult& result) {
    int dayCount = data_->getDayCount();
    int slotsPerDay = data_->getSlotsPerDay();

    std::vector<std::vector<std::string>> grid(dayCount, std::vector<std::string>(slotsPerDay, ""));

    for (const auto& sc : result.schedule) {
        if (!sc->isScheduled()) continue;
        auto slot = sc->getTimeSlot();
        int dayIdx = static_cast<int>(slot->getDay());
        int slotIdx = slot->getSlotIndex();
        if (dayIdx >= 0 && dayIdx < dayCount && slotIdx >= 0 && slotIdx < slotsPerDay) {
            std::ostringstream oss;
            oss << sc->getCourse()->getName()
                << "\n(" << sc->getTeacher()->getName()
                << ", " << sc->getClassroom()->getName() << ")";
            grid[dayIdx][slotIdx] = oss.str();
        }
    }

    std::cout << "\n==================================================";
    std::cout << "班级课表总览（按班级分别展示）\n";

    std::vector<ClassGroupPtr> classes;
    for (const auto& sc : result.schedule) {
        if (sc->getClassGroup()) {
            bool found = false;
            for (const auto& c : classes) {
                if (c->getId() == sc->getClassGroup()->getId()) {
                    found = true;
                    break;
                }
            }
            if (!found) classes.push_back(sc->getClassGroup());
        }
    }

    for (const auto& cls : classes) {
        std::cout << "\n--- " << cls->getName() << " 课表 ---\n";
        std::cout << std::left << std::setw(12) << "节次";
        for (int d = 0; d < dayCount; ++d) {
            std::cout << std::left << std::setw(30) << TimeSlot::dayToString(static_cast<DayOfWeek>(d));
        }
        std::cout << "\n";

        for (int s = 0; s < slotsPerDay; ++s) {
            std::cout << std::left << std::setw(12) << ("第" + std::to_string(s+1) + "节");
            for (int d = 0; d < dayCount; ++d) {
                std::string cell;
                for (const auto& sc : result.schedule) {
                    if (!sc->isScheduled()) continue;
                    if (sc->getClassGroup()->getId() != cls->getId()) continue;
                    auto slot = sc->getTimeSlot();
                    if (static_cast<int>(slot->getDay()) == d && slot->getSlotIndex() == s) {
                        cell = sc->getCourse()->getName() + "/" + sc->getTeacher()->getName();
                        break;
                    }
                }
                std::cout << std::left << std::setw(30) << cell;
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void ResultInterpreter::printTeacherSchedule(const TimetableResult& result, int teacherId) {
    auto teacher = data_->getTeacher(teacherId);
    if (!teacher) return;

    std::cout << "\n--- " << teacher->getName() << " 教师课表 ---\n";

    int dayCount = data_->getDayCount();
    int slotsPerDay = data_->getSlotsPerDay();

    std::cout << std::left << std::setw(12) << "节次";
    for (int d = 0; d < dayCount; ++d) {
        std::cout << std::left << std::setw(28) << TimeSlot::dayToString(static_cast<DayOfWeek>(d));
    }
    std::cout << "\n";

    for (int s = 0; s < slotsPerDay; ++s) {
        std::cout << std::left << std::setw(12) << ("第" + std::to_string(s+1) + "节");
        for (int d = 0; d < dayCount; ++d) {
            std::string cell;
            for (const auto& sc : result.schedule) {
                if (!sc->isScheduled()) continue;
                if (sc->getTeacher()->getId() != teacherId) continue;
                auto slot = sc->getTimeSlot();
                if (static_cast<int>(slot->getDay()) == d && slot->getSlotIndex() == s) {
                    cell = sc->getCourse()->getName() + "(" + sc->getClassroom()->getName() + ")";
                    break;
                }
            }
            std::cout << std::left << std::setw(28) << cell;
        }
        std::cout << "\n";
    }
}

void ResultInterpreter::printClassSchedule(const TimetableResult& result, int classId) {
    auto cls = data_->getClassGroup(classId);
    if (!cls) return;

    std::cout << "\n--- " << cls->getName() << " 课表 ---\n";

    int dayCount = data_->getDayCount();
    int slotsPerDay = data_->getSlotsPerDay();

    std::cout << std::left << std::setw(12) << "节次";
    for (int d = 0; d < dayCount; ++d) {
        std::cout << std::left << std::setw(30) << TimeSlot::dayToString(static_cast<DayOfWeek>(d));
    }
    std::cout << "\n";

    for (int s = 0; s < slotsPerDay; ++s) {
        std::cout << std::left << std::setw(12) << ("第" + std::to_string(s+1) + "节");
        for (int d = 0; d < dayCount; ++d) {
            std::string cell;
            for (const auto& sc : result.schedule) {
                if (!sc->isScheduled()) continue;
                if (sc->getClassGroup()->getId() != classId) continue;
                auto slot = sc->getTimeSlot();
                if (static_cast<int>(slot->getDay()) == d && slot->getSlotIndex() == s) {
                    cell = sc->getCourse()->getName() + "/" + sc->getTeacher()->getName();
                    break;
                }
            }
            std::cout << std::left << std::setw(30) << cell;
        }
        std::cout << "\n";
    }
}

}
