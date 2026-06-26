#include "utils/SampleDataGenerator.h"
#include <random>
#include <iostream>

namespace timetable {

void SampleDataGenerator::generateTimeSlots(TimetableDataPtr data, int days, int slotsPerDay) {
    data->setDayCount(days);
    data->setSlotsPerDay(slotsPerDay);

    std::vector<std::pair<std::string, std::string>> morningSlots = {
        {"08:00", "09:30"},
        {"09:50", "11:20"},
        {"11:30", "12:15"}
    };
    std::vector<std::pair<std::string, std::string>> afternoonSlots = {
        {"14:00", "15:30"},
        {"15:50", "17:20"}
    };
    std::vector<std::pair<std::string, std::string>> eveningSlots = {
        {"19:00", "20:30"},
        {"20:45", "22:15"}
    };

    int id = 0;
    for (int d = 0; d < days; ++d) {
        DayOfWeek day = static_cast<DayOfWeek>(d);
        for (int s = 0; s < slotsPerDay; ++s) {
            TimeSlotType type;
            std::string start, end;
            if (s < static_cast<int>(morningSlots.size())) {
                type = TimeSlotType::Morning;
                start = morningSlots[s].first;
                end = morningSlots[s].second;
            } else if (s < static_cast<int>(morningSlots.size() + afternoonSlots.size())) {
                type = TimeSlotType::Afternoon;
                int idx = s - static_cast<int>(morningSlots.size());
                start = afternoonSlots[idx].first;
                end = afternoonSlots[idx].second;
            } else {
                type = TimeSlotType::Evening;
                int idx = s - static_cast<int>(morningSlots.size() + afternoonSlots.size());
                start = eveningSlots[idx].first;
                end = eveningSlots[idx].second;
            }
            data->addTimeSlot(std::make_shared<TimeSlot>(id++, day, s, start, end, type));
        }
    }
}

void SampleDataGenerator::generateClassrooms(TimetableDataPtr data, int count) {
    std::vector<ClassroomType> types = {
        ClassroomType::General,
        ClassroomType::General,
        ClassroomType::General,
        ClassroomType::LectureHall,
        ClassroomType::Laboratory,
        ClassroomType::ComputerRoom
    };

    for (int i = 0; i < count; ++i) {
        ClassroomType type = types[i % types.size()];
        int capacity = (type == ClassroomType::LectureHall) ? 150 :
                       (type == ClassroomType::General) ? 60 : 40;
        std::string name = "教" + std::to_string(100 + i);
        data->addClassroom(std::make_shared<Classroom>(
            i, name, capacity, type, "教学楼" + std::to_string(i/5 + 1), (i % 5) + 1));
    }
}

void SampleDataGenerator::generateTeachers(TimetableDataPtr data, int count) {
    std::vector<std::string> names = {
        "张老师", "李老师", "王老师", "赵老师", "刘老师",
        "陈老师", "杨老师", "黄老师", "周老师", "吴老师",
        "徐老师", "孙老师", "马老师", "朱老师", "胡老师"
    };
    std::vector<std::string> titles = {
        "教授", "副教授", "讲师", "助教"
    };
    std::vector<std::string> depts = {
        "计算机学院", "数学学院", "物理学院", "外语学院", "经管学院"
    };

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> titleDist(0, static_cast<int>(titles.size()) - 1);
    std::uniform_int_distribution<int> deptDist(0, static_cast<int>(depts.size()) - 1);
    std::uniform_int_distribution<int> prefDist(0, 2);

    for (int i = 0; i < count; ++i) {
        std::string name = (i < static_cast<int>(names.size()))
                               ? names[i] : ("教师" + std::to_string(i));
        auto teacher = std::make_shared<Teacher>(
            i, name, titles[titleDist(rng)], depts[deptDist(rng)]);

        TeacherPreference pref;
        pref.maxConsecutiveHours = 2;
        pref.maxDailyHours = 4;
        pref.minGapBetweenClasses = 0;

        int prefType = prefDist(rng);
        if (prefType == 0) {
            pref.distributionPref = DistributionPreference::Compact;
            pref.preferredTeachingDays = 3;
            pref.preferredDays = {DayOfWeek::Monday, DayOfWeek::Wednesday, DayOfWeek::Friday};
        } else if (prefType == 1) {
            pref.distributionPref = DistributionPreference::Balanced;
            pref.preferredTeachingDays = 4;
        } else {
            pref.distributionPref = DistributionPreference::Spread;
            pref.preferredTeachingDays = 5;
            pref.preferredSlotTypes = {TimeSlotType::Morning, TimeSlotType::Afternoon};
        }

        if (i % 3 == 0) {
            pref.avoidEarlyMorning = true;
        }

        if (i % 5 == 0) {
            pref.avoidedDays = {DayOfWeek::Friday};
        }

        pref.timePreferenceWeight = 1.0 + (i % 3) * 0.5;
        pref.intensityPreferenceWeight = 1.0 + (i % 2) * 0.3;
        pref.distributionPreferenceWeight = 1.0;

        teacher->setPreference(pref);
        data->addTeacher(teacher);
    }
}

void SampleDataGenerator::generateCourses(TimetableDataPtr data, int count) {
    std::vector<std::pair<std::string, CourseType>> courseList = {
        {"高等数学", CourseType::General},
        {"线性代数", CourseType::General},
        {"大学英语", CourseType::General},
        {"数据结构", CourseType::MajorRequired},
        {"操作系统", CourseType::MajorRequired},
        {"计算机网络", CourseType::MajorRequired},
        {"数据库原理", CourseType::MajorRequired},
        {"人工智能", CourseType::MajorElective},
        {"机器学习", CourseType::MajorElective},
        {"软件工程", CourseType::MajorRequired},
        {"算法设计", CourseType::MajorRequired},
        {"编译原理", CourseType::MajorElective},
        {"大学物理", CourseType::General},
        {"程序设计", CourseType::MajorRequired},
        {"离散数学", CourseType::MajorRequired},
        {"计算机组成", CourseType::MajorRequired},
        {"Java程序设计", CourseType::MajorElective},
        {"Python编程", CourseType::MajorElective},
        {"Web开发", CourseType::MajorElective},
        {"云计算基础", CourseType::MajorElective}
    };

    std::mt19937 rng(123);
    for (int i = 0; i < count; ++i) {
        auto& courseInfo = courseList[i % courseList.size()];
        int weeklyHours = 2 + (i % 3) * 2;
        int credits = weeklyHours / 2 + (i % 2);
        bool requiresLab = (courseInfo.second == CourseType::MajorRequired && i % 3 == 0);
        int studentCount = 40 + (i % 5) * 10;

        std::string code = "CS" + std::to_string(1000 + i);
        auto course = std::make_shared<Course>(
            i, courseInfo.first, code, weeklyHours, credits,
            courseInfo.second, requiresLab, studentCount);
        data->addCourse(course);
    }
}

void SampleDataGenerator::generateClassGroups(TimetableDataPtr data, int count) {
    std::vector<ClassLevel> levels = {
        ClassLevel::Freshman, ClassLevel::Sophomore,
        ClassLevel::Junior, ClassLevel::Senior
    };
    std::vector<std::string> majors = {
        "计算机科学与技术", "软件工程", "人工智能", "数据科学"
    };

    for (int i = 0; i < count; ++i) {
        ClassLevel level = levels[i / 5 % levels.size()];
        std::string major = majors[i % majors.size()];
        std::string name = ClassGroup::levelToString(level)
                           + std::to_string(i % 5 + 1) + "班";
        int students = 35 + (i % 4) * 5;
        data->addClassGroup(std::make_shared<ClassGroup>(
            i, name, level, students, major));
    }
}

void SampleDataGenerator::generateScheduledClasses(TimetableDataPtr data, int count) {
    const auto& courses = data->getCourses();
    const auto& teachers = data->getTeachers();
    const auto& classGroups = data->getClassGroups();

    if (courses.empty() || teachers.empty() || classGroups.empty()) return;

    std::mt19937 rng(456);
    std::uniform_int_distribution<int> teacherDist(0, static_cast<int>(teachers.size()) - 1);
    std::uniform_int_distribution<int> classDist(0, static_cast<int>(classGroups.size()) - 1);
    std::uniform_int_distribution<int> durDist(1, 2);

    int scId = 0;
    for (int i = 0; i < static_cast<int>(courses.size()) && scId < count; ++i) {
        auto course = courses[i];
        int sessions = course->getWeeklyHours() / 2;

        for (int s = 0; s < sessions && scId < count; ++s) {
            int teacherIdx = teacherDist(rng);
            int classIdx = classDist(rng);
            int duration = durDist(rng);

            auto sc = std::make_shared<ScheduledClass>(
                scId++, course, teachers[teacherIdx],
                classGroups[classIdx], duration);
            data->addScheduledClass(sc);
        }
    }

    while (scId < count) {
        int courseIdx = scId % courses.size();
        int teacherIdx = teacherDist(rng);
        int classIdx = classDist(rng);
        int duration = durDist(rng);

        auto sc = std::make_shared<ScheduledClass>(
            scId++, courses[courseIdx], teachers[teacherIdx],
            classGroups[classIdx], duration);
        data->addScheduledClass(sc);
    }
}

TimetableDataPtr SampleDataGenerator::generateSmallExample() {
    auto data = std::make_shared<TimetableData>();
    generateTimeSlots(data, 5, 5);
    generateClassrooms(data, 6);
    generateTeachers(data, 6);
    generateCourses(data, 8);
    generateClassGroups(data, 4);
    generateScheduledClasses(data, 16);
    return data;
}

TimetableDataPtr SampleDataGenerator::generateMediumExample() {
    auto data = std::make_shared<TimetableData>();
    generateTimeSlots(data, 5, 5);
    generateClassrooms(data, 12);
    generateTeachers(data, 12);
    generateCourses(data, 15);
    generateClassGroups(data, 8);
    generateScheduledClasses(data, 40);
    return data;
}

}
