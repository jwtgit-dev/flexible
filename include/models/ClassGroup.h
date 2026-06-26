#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace timetable {

enum class ClassLevel {
    Freshman,
    Sophomore,
    Junior,
    Senior,
    Graduate
};

class ClassGroup {
public:
    ClassGroup() = default;
    ClassGroup(int id, const std::string& name, ClassLevel level,
               int studentCount, const std::string& major = "");

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    ClassLevel getLevel() const { return level_; }
    int getStudentCount() const { return studentCount_; }
    const std::string& getMajor() const { return major_; }

    void setStudentCount(int count) { studentCount_ = count; }

    static std::string levelToString(ClassLevel level);
    static ClassLevel stringToLevel(const std::string& str);

private:
    int id_{-1};
    std::string name_;
    ClassLevel level_{ClassLevel::Freshman};
    int studentCount_{0};
    std::string major_;
};

using ClassGroupPtr = std::shared_ptr<ClassGroup>;

}
