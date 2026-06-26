# 柔性智能排课系统 (Flexible Intelligent Timetabling System)

## 项目概述

面向个性化需求的柔性智能排课系统，基于多目标优化算法（NSGA-II）实现，支持教师个性化偏好、多目标协同优化和帕累托最优解集输出。

## 核心特性

### 1. 多维度个性化机制
- **教师时间偏好**：偏好/回避特定日期、时段，避免早八课程
- **教学强度偏好**：每日最大课时、连续课时上限、课间最小间隔
- **课程分布偏好**：紧凑模式/均衡模式/分散模式
- **权重体系**：支持不同类型偏好的权重配置，区分硬约束与软需求

### 2. 多目标优化算法（NSGA-II）
- **目标函数**：
  - 教师满意度（时间偏好+教学强度+课程分布）
  - 班级课表紧凑性（减少碎片化）
  - 教室资源利用均衡性
- **算法机制**：非支配排序、拥挤度计算、锦标赛选择、精英保留策略
- **输出**：帕累托最优解集，支持多方案对比与人工决策

### 3. 约束系统
**硬约束（必须满足）**：
- 教师时间冲突
- 教室时间冲突
- 班级时间冲突
- 教室类型适配（实验课需实验室）
- 教室容量限制
- 教师不可用时间

**软约束（优化目标）**：
- 教师时间偏好
- 教师教学强度偏好
- 教师课程分布偏好
- 班级课表紧凑性
- 教室利用率均衡

### 4. 结果解释与满意度分析
- 综合满意度评分
- 各维度偏好满足率
- 教师个体满意度
- 未满足项原因追溯与解释
- 帕累托方案对比分析
- 课表可视化输出

## 项目结构

```
flexible-timetabling/
├── CMakeLists.txt          # CMake构建配置
├── build.bat               # Windows构建脚本
├── build.sh                # Linux/Mac构建脚本
├── include/
│   ├── models/             # 数据模型
│   │   ├── TimeSlot.h      # 时间槽
│   │   ├── Classroom.h     # 教室
│   │   ├── Course.h        # 课程
│   │   ├── ClassGroup.h    # 班级
│   │   ├── Teacher.h       # 教师（含偏好）
│   │   ├── ScheduledClass.h # 排课条目
│   │   └── TimetableData.h # 数据容器
│   ├── constraints/        # 约束系统
│   │   ├── IConstraint.h   # 约束接口
│   │   ├── HardConstraints.h  # 硬约束
│   │   └── SoftConstraints.h  # 软约束
│   ├── algorithm/          # 优化算法
│   │   ├── Chromosome.h    # 染色体编码
│   │   └── NSGAII.h        # NSGA-II算法
│   ├── engine/             # 排课引擎
│   │   └── TimetableEngine.h
│   ├── config/             # 配置管理
│   │   └── ConfigManager.h
│   ├── interpreter/        # 结果解释
│   │   └── ResultInterpreter.h
│   └── utils/              # 工具
│       └── SampleDataGenerator.h
└── src/                    # 实现文件
    ├── main.cpp
    ├── models/
    ├── constraints/
    ├── algorithm/
    ├── engine/
    ├── config/
    ├── interpreter/
    └── utils/
```

## 编译运行

### 方法一：CMake（推荐）
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### 方法二：直接用 g++
```bash
g++ -std=c++17 -O2 -I include src/main.cpp \
    src/models/*.cpp src/constraints/*.cpp \
    src/algorithm/*.cpp src/engine/*.cpp \
    src/config/*.cpp src/interpreter/*.cpp \
    src/utils/*.cpp -o timetabling -lpthread
```

### 方法三：使用构建脚本
- Windows: 双击 `build.bat`
- Linux/Mac: `./build.sh`

## 使用方式

### 命令行交互模式
```bash
./timetabling
```

### 直接运行示例
```bash
./timetabling -s    # 小型示例 (16门课)
./timetabling -m    # 中型示例 (40门课)
```

## 配置说明

配置文件格式（INI风格）：
```ini
# 算法参数
algorithm.population_size = 80
algorithm.max_generations = 150
algorithm.crossover_rate = 0.85
algorithm.mutation_rate = 0.15
algorithm.num_objectives = 3

# 权重配置
weight.time_preference = 1.5
weight.intensity_preference = 1.2
weight.distribution_preference = 1.0
weight.compactness = 0.8
weight.utilization = 0.6

# 课表参数
timetable.slots_per_day = 5
timetable.days_per_week = 5
```

## 算法设计说明

### 编码方式
每个染色体由一组基因组成，每个基因对应一个待排课程：
- `scheduledClassId`: 课程ID
- `timeSlotId`: 分配的时间槽ID
- `classroomId`: 分配的教室ID

### 多目标优化
三个优化目标（均为最小化）：
1. **教师综合不满意值**：时间偏好+教学强度+课程分布的加权惩罚
2. **班级课表碎片化程度**：班级课表中空档的总数量
3. **教室利用不均衡度**：各教室使用频率的标准差

### 约束处理
- 硬约束违反：目标值叠加大惩罚项（1000 × 违反数）
- 软约束违反：直接计入对应目标函数

### 帕累托最优
输出非支配解集合，每个解在不同目标间各有取舍，供教务人员根据实际需求选择。

## 扩展方向

- 支持更多优化目标（教师工作量均衡、跨校区通勤成本等）
- 增加启发式局部搜索提升解质量
- 支持数据导入导出（Excel/CSV格式）
- 满意度反馈闭环机制（根据历史反馈动态调整权重）
- GUI可视化界面
- 并行计算加速大规模排课

## 技术栈

- **语言**：C++17
- **算法**：NSGA-II 多目标进化算法
- **架构**：模块化设计，面向对象
- **依赖**：无第三方依赖，纯标准库实现

## 许可证

MIT License
