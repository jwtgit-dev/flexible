#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace timetable {

enum class CourseType {
    General,
    MajorRequired,
    MajorElective,
    PublicElective,
    Practical,
    Lab
};

class Course {
public:
    Course() = default;
    Course(int id, const std::string& name, const std::string& code,
           int weeklyHours, int credits, CourseType type = CourseType::General,
           bool requiresLab = false, int studentCount = 0);

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getCode() const { return code_; }
    int getWeeklyHours() const { return weeklyHours_; }
    int getCredits() const { return credits_; }
    CourseType getType() const { return type_; }
    bool requiresLab() const { return requiresLab_; }
    int getStudentCount() const { return studentCount_; }

    void setWeeklyHours(int hours) { weeklyHours_ = hours; }
    void setRequiresLab(bool lab) { requiresLab_ = lab; }
    void setStudentCount(int count) { studentCount_ = count; }

    static std::string typeToString(CourseType type);
    static CourseType stringToType(const std::string& str);

private:
    int id_{-1};
    std::string name_;
    std::string code_;
    int weeklyHours_{0};
    int credits_{0};
    CourseType type_{CourseType::General};
    bool requiresLab_{false};
    int studentCount_{0};
};

using CoursePtr = std::shared_ptr<Course>;

}
