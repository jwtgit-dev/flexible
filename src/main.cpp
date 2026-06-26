#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include "models/TimetableData.h"
#include "constraints/IConstraint.h"
#include "engine/TimetableEngine.h"
#include "config/ConfigManager.h"
#include "interpreter/ResultInterpreter.h"
#include "utils/SampleDataGenerator.h"

using namespace timetable;

void printBanner() {
    std::cout << "\n";
    std::cout << "=============================================================\n";
    std::cout << "    柔性智能排课系统 v1.0\n";
    std::cout << "    Flexible Intelligent Timetabling System\n";
    std::cout << "=============================================================\n\n";
}

void printMenu() {
    std::cout << "\n请选择操作:\n";
    std::cout << "  1. 运行小型示例排课 (16门课)\n";
    std::cout << "  2. 运行中型示例排课 (40门课)\n";
    std::cout << "  3. 查看当前配置\n";
    std::cout << "  4. 导出默认配置文件\n";
    std::cout << "  0. 退出\n";
    std::cout << "请输入选项: ";
}

void runTimetabling(const std::string& size) {
    std::cout << "\n正在生成排课数据...\n";

    TimetableDataPtr data;
    if (size == "small") {
        data = SampleDataGenerator::generateSmallExample();
    } else {
        data = SampleDataGenerator::generateMediumExample();
    }

    std::cout << "数据规模:\n";
    std::cout << "  时间槽数量: " << data->getTimeSlotCount() << "\n";
    std::cout << "  教室数量: " << data->getClassroomCount() << "\n";
    std::cout << "  教师数量: " << data->getTeachers().size() << "\n";
    std::cout << "  课程数量: " << data->getCourses().size() << "\n";
    std::cout << "  班级数量: " << data->getClassGroups().size() << "\n";
    std::cout << "  待排课程数: " << data->getScheduledClassCount() << "\n\n";

    ConfigManager config;
    SchedulingParams params = config.getSchedulingParams();
    params.populationSize = 60;
    params.maxGenerations = 100;
    params.mutationRate = 0.2;

    std::cout << "算法参数:\n";
    std::cout << "  种群规模: " << params.populationSize << "\n";
    std::cout << "  进化代数: " << params.maxGenerations << "\n";
    std::cout << "  交叉概率: " << params.crossoverRate << "\n";
    std::cout << "  变异概率: " << params.mutationRate << "\n";
    std::cout << "  目标数量: " << params.numObjectives << "\n\n";

    TimetableEngine engine(data);
    engine.setParams(params);

    std::cout << "正在排课计算中...\n";
    int lastGen = -1;
    engine.setProgressCallback([&lastGen](int gen, const std::vector<Chromosome>&) {
        if (gen % 10 == 0 || gen == 1) {
            std::cout << "  第 " << std::setw(3) << gen << " 代...\r" << std::flush;
        }
        lastGen = gen;
    });

    auto startTime = std::chrono::high_resolution_clock::now();

    auto paretoFront = engine.solve();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "\n\n排课完成！耗时: " << duration.count() / 1000.0 << " 秒\n";
    std::cout << "帕累托最优解数量: " << paretoFront.size() << "\n\n";

    if (paretoFront.empty()) {
        std::cout << "警告: 未找到可行解！\n";
        return;
    }

    ResultInterpreter interpreter(data, engine.getConstraintManager());

    auto comparisons = interpreter.compareSolutions(paretoFront);
    std::cout << interpreter.generateComparisonText(comparisons);

    auto bestResult = engine.getBestResult();
    auto report = interpreter.analyze(bestResult);
    std::cout << interpreter.generateTextReport(report);

    char viewChoice;
    std::cout << "\n是否查看课表视图？(y/n): ";
    std::cin >> viewChoice;
    if (viewChoice == 'y' || viewChoice == 'Y') {
        interpreter.printTimetableView(bestResult);
    }

    char teacherChoice;
    std::cout << "\n是否查看教师课表示例？(y/n): ";
    std::cin >> teacherChoice;
    if (teacherChoice == 'y' || teacherChoice == 'Y') {
        auto teachers = data->getTeachers();
        if (!teachers.empty()) {
            interpreter.printTeacherSchedule(bestResult, teachers[0]->getId());
        }
    }
}

void showConfig() {
    ConfigManager config;
    std::cout << "\n当前配置:\n";
    std::cout << "-----------------------------------------\n";
    for (const auto& [key, value] : config.getAllSettings()) {
        std::cout << "  " << std::left << std::setw(35) << key << " = " << value << "\n";
    }
    std::cout << "-----------------------------------------\n";
}

void exportConfig() {
    ConfigManager config;
    std::string filename = "timetable_config.ini";
    if (config.saveToFile(filename)) {
        std::cout << "\n配置文件已导出到: " << filename << "\n";
    } else {
        std::cout << "\n导出失败\n";
    }
}

int main(int argc, char* argv[]) {
    printBanner();

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--small" || arg == "-s") {
            runTimetabling("small");
            return 0;
        } else if (arg == "--medium" || arg == "-m") {
            runTimetabling("medium");
            return 0;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "用法:\n";
            std::cout << "  " << argv[0] << " [选项]\n\n";
            std::cout << "选项:\n";
            std::cout << "  -s, --small     运行小型示例\n";
            std::cout << "  -m, --medium    运行中型示例\n";
            std::cout << "  -h, --help      显示帮助\n";
            return 0;
        }
    }

    while (true) {
        printMenu();
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                runTimetabling("small");
                break;
            case 2:
                runTimetabling("medium");
                break;
            case 3:
                showConfig();
                break;
            case 4:
                exportConfig();
                break;
            case 0:
                std::cout << "\n感谢使用！再见。\n";
                return 0;
            default:
                std::cout << "\n无效选项，请重新选择。\n";
                break;
        }
    }

    return 0;
}
