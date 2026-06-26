#pragma once
#include <string>
#include <vector>
#include <memory>

namespace timetable {

enum class ClassroomType {
    General,
    LectureHall,
    Laboratory,
    ComputerRoom,
    ArtRoom,
    MusicRoom
};

class Classroom {
public:
    Classroom() = default;
    Classroom(int id, const std::string& name, int capacity, ClassroomType type,
              const std::string& building = "", int floor = 0);

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    int getCapacity() const { return capacity_; }
    ClassroomType getType() const { return type_; }
    const std::string& getBuilding() const { return building_; }
    int getFloor() const { return floor_; }

    void setCapacity(int cap) { capacity_ = cap; }
    void setType(ClassroomType type) { type_ = type; }

    static std::string typeToString(ClassroomType type);
    static ClassroomType stringToType(const std::string& str);

private:
    int id_{-1};
    std::string name_;
    int capacity_{0};
    ClassroomType type_{ClassroomType::General};
    std::string building_;
    int floor_{0};
};

using ClassroomPtr = std::shared_ptr<Classroom>;

}
