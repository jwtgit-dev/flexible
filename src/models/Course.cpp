#include "models/Course.h"
#include <stdexcept>

namespace timetable {

Course::Course(int id, const std::string& name, const std::string& code,
               int weeklyHours, int credits, CourseType type,
               bool requiresLab, int studentCount)
    : id_(id), name_(name), code_(code), weeklyHours_(weeklyHours),
      credits_(credits), type_(type), requiresLab_(requiresLab),
      studentCount_(studentCount) {}

std::string Course::typeToString(CourseType type) {
    switch(type) {
        case CourseType::General: return "通识课";
        case CourseType::MajorRequired: return "专业必修";
        case CourseType::MajorElective: return "专业选修";
        case CourseType::PublicElective: return "公选课";
        case CourseType::Practical: return "实践课";
        case CourseType::Lab: return "实验课";
        default: return "未知";
    }
}

CourseType Course::stringToType(const std::string& str) {
    if (str == "通识课" || str == "General") return CourseType::General;
    if (str == "专业必修" || str == "MajorRequired") return CourseType::MajorRequired;
    if (str == "专业选修" || str == "MajorElective") return CourseType::MajorElective;
    if (str == "公选课" || str == "PublicElective") return CourseType::PublicElective;
    if (str == "实践课" || str == "Practical") return CourseType::Practical;
    if (str == "实验课" || str == "Lab") return CourseType::Lab;
    throw std::invalid_argument("Invalid course type: " + str);
}

}
