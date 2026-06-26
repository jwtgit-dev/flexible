#pragma once
#include "models/TimetableData.h"

namespace timetable {

class SampleDataGenerator {
public:
    static TimetableDataPtr generateSmallExample();
    static TimetableDataPtr generateMediumExample();

private:
    static void generateTimeSlots(TimetableDataPtr data, int days, int slotsPerDay);
    static void generateClassrooms(TimetableDataPtr data, int count);
    static void generateTeachers(TimetableDataPtr data, int count);
    static void generateCourses(TimetableDataPtr data, int count);
    static void generateClassGroups(TimetableDataPtr data, int count);
    static void generateScheduledClasses(TimetableDataPtr data, int count);
};

}
