#include "algorithm/Chromosome.h"
#include <algorithm>
#include <random>

namespace timetable {

bool Chromosome::dominates(const Chromosome& other) const {
    bool atLeastOneBetter = false;
    for (size_t i = 0; i < objectives_.size(); ++i) {
        if (objectives_[i] > other.objectives_[i]) return false;
        if (objectives_[i] < other.objectives_[i]) atLeastOneBetter = true;
    }
    return atLeastOneBetter;
}

std::vector<ScheduledClassPtr> ChromosomeDecoder::decode(
        const Chromosome& chromosome,
        const TimetableDataPtr& data) {
    std::vector<ScheduledClassPtr> schedule;
    for (int i = 0; i < chromosome.size(); ++i) {
        const auto& gene = chromosome[i];
        auto sc = data->getScheduledClass(gene.scheduledClassId);
        if (!sc) continue;

        auto newSc = std::make_shared<ScheduledClass>(*sc);
        auto timeSlot = data->getTimeSlot(gene.timeSlotId);
        auto classroom = data->getClassroom(gene.classroomId);

        newSc->setTimeSlot(timeSlot);
        newSc->setClassroom(classroom);
        schedule.push_back(newSc);
    }
    return schedule;
}

ChromosomeInitializer::ChromosomeInitializer(const TimetableDataPtr& data)
    : data_(data) {}

Chromosome ChromosomeInitializer::generateRandom(std::mt19937& rng) const {
    const auto& classes = data_->getScheduledClasses();
    const auto& timeSlots = data_->getTimeSlots();
    const auto& classrooms = data_->getClassrooms();

    Chromosome chrom(static_cast<int>(classes.size()));

    for (size_t i = 0; i < classes.size(); ++i) {
        int scId = classes[i]->getId();

        std::uniform_int_distribution<int> slotDist(0, static_cast<int>(timeSlots.size()) - 1);
        std::uniform_int_distribution<int> roomDist(0, static_cast<int>(classrooms.size()) - 1);

        int slotIdx = slotDist(rng);
        int roomIdx = roomDist(rng);

        chrom[i] = Gene(scId, timeSlots[slotIdx]->getId(), classrooms[roomIdx]->getId());
    }

    return chrom;
}

Chromosome ChromosomeInitializer::generateGreedy(std::mt19937& rng) const {
    const auto& classes = data_->getScheduledClasses();
    const auto& timeSlots = data_->getTimeSlots();
    const auto& classrooms = data_->getClassrooms();

    Chromosome chrom(static_cast<int>(classes.size()));

    std::unordered_map<int, std::unordered_set<int>> teacherUsedSlots;
    std::unordered_map<int, std::unordered_set<int>> roomUsedSlots;
    std::unordered_map<int, std::unordered_set<int>> classUsedSlots;

    for (size_t i = 0; i < classes.size(); ++i) {
        auto sc = classes[i];
        int scId = sc->getId();
        int teacherId = sc->getTeacher()->getId();
        int classId = sc->getClassGroup()->getId();
        auto course = sc->getCourse();

        std::vector<int> candidateSlots;
        for (const auto& slot : timeSlots) {
            if (teacherUsedSlots[teacherId].count(slot->getId())) continue;
            if (classUsedSlots[classId].count(slot->getId())) continue;
            candidateSlots.push_back(slot->getId());
        }

        if (candidateSlots.empty()) {
            std::uniform_int_distribution<int> slotDist(0, static_cast<int>(timeSlots.size()) - 1);
            candidateSlots.push_back(timeSlots[slotDist(rng)]->getId());
        }

        std::vector<int> candidateRooms;
        for (const auto& room : classrooms) {
            if (course->requiresLab() && room->getType() != ClassroomType::Laboratory
                && room->getType() != ClassroomType::ComputerRoom) {
                continue;
            }
            int students = course->getStudentCount() > 0
                               ? course->getStudentCount()
                               : sc->getClassGroup()->getStudentCount();
            if (students > room->getCapacity()) continue;

            bool slotAvailable = true;
            for (int slotId : candidateSlots) {
                if (roomUsedSlots[room->getId()].count(slotId)) {
                    slotAvailable = false;
                    break;
                }
            }
            if (slotAvailable) {
                candidateRooms.push_back(room->getId());
            }
        }

        if (candidateRooms.empty()) {
            for (const auto& room : classrooms) {
                candidateRooms.push_back(room->getId());
            }
        }

        std::uniform_int_distribution<int> slotChoice(0, static_cast<int>(candidateSlots.size()) - 1);
        std::uniform_int_distribution<int> roomChoice(0, static_cast<int>(candidateRooms.size()) - 1);

        int chosenSlot = candidateSlots[slotChoice(rng)];
        int chosenRoom = candidateRooms[roomChoice(rng)];

        teacherUsedSlots[teacherId].insert(chosenSlot);
        roomUsedSlots[chosenRoom].insert(chosenSlot);
        classUsedSlots[classId].insert(chosenSlot);

        chrom[i] = Gene(scId, chosenSlot, chosenRoom);
    }

    return chrom;
}

}
