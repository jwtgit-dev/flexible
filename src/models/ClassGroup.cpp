#include "models/ClassGroup.h"
#include <stdexcept>

namespace timetable {

ClassGroup::ClassGroup(int id, const std::string& name, ClassLevel level,
                       int studentCount, const std::string& major)
    : id_(id), name_(name), level_(level), studentCount_(studentCount), major_(major) {}

std::string ClassGroup::levelToString(ClassLevel level) {
    switch(level) {
        case ClassLevel::Freshman: return "大一";
        case ClassLevel::Sophomore: return "大二";
        case ClassLevel::Junior: return "大三";
        case ClassLevel::Senior: return "大四";
        case ClassLevel::Graduate: return "研究生";
        default: return "未知";
    }
}

ClassLevel ClassGroup::stringToLevel(const std::string& str) {
    if (str == "大一" || str == "Freshman") return ClassLevel::Freshman;
    if (str == "大二" || str == "Sophomore") return ClassLevel::Sophomore;
    if (str == "大三" || str == "Junior") return ClassLevel::Junior;
    if (str == "大四" || str == "Senior") return ClassLevel::Senior;
    if (str == "研究生" || str == "Graduate") return ClassLevel::Graduate;
    throw std::invalid_argument("Invalid class level: " + str);
}

}
