#include "models/Classroom.h"
#include <stdexcept>

namespace timetable {

Classroom::Classroom(int id, const std::string& name, int capacity, ClassroomType type,
                     const std::string& building, int floor)
    : id_(id), name_(name), capacity_(capacity), type_(type),
      building_(building), floor_(floor) {}

std::string Classroom::typeToString(ClassroomType type) {
    switch(type) {
        case ClassroomType::General: return "普通教室";
        case ClassroomType::LectureHall: return "阶梯教室";
        case ClassroomType::Laboratory: return "实验室";
        case ClassroomType::ComputerRoom: return "机房";
        case ClassroomType::ArtRoom: return "美术教室";
        case ClassroomType::MusicRoom: return "音乐教室";
        default: return "未知";
    }
}

ClassroomType Classroom::stringToType(const std::string& str) {
    if (str == "普通教室" || str == "General") return ClassroomType::General;
    if (str == "阶梯教室" || str == "LectureHall") return ClassroomType::LectureHall;
    if (str == "实验室" || str == "Laboratory") return ClassroomType::Laboratory;
    if (str == "机房" || str == "ComputerRoom") return ClassroomType::ComputerRoom;
    if (str == "美术教室" || str == "ArtRoom") return ClassroomType::ArtRoom;
    if (str == "音乐教室" || str == "MusicRoom") return ClassroomType::MusicRoom;
    throw std::invalid_argument("Invalid classroom type: " + str);
}

}
