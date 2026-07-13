# TES_01 C++重构 + 战斗系统 + 联机系统 策划案

> **版本**: v1.1  
> **创建日期**: 2026-07-13  
> **引擎**: Unreal Engine 5.2  
> **目标**: 纯蓝图项目 → C++架构化项目，新增即时动作战斗系统，新增2-4人P2P合作联机

---

## 0. 仓库开源策略 ⚠️

### 0.1 版权隔离原则

**GitHub 仓库仅包含原创代码**，不包含任何第三方版权内容。

| 目录 | GitHub追踪 | 说明 |
|------|-----------|------|
| `Source/` | ✅ 追踪 | C++ 源代码（原创） |
| `Config/` | ✅ 追踪 | 引擎/项目配置文件 |
| `DesignDocs/` | ✅ 追踪 | 项目策划文档 |
| `*.uproject` | ✅ 追踪 | 项目描述文件 |
| `.gitignore` / `.gitattributes` | ✅ 追踪 | Git 配置 |
| `Content/` | ❌ gitignore | **版权资产目录**（Fantastic Village Pack、PaperZD动画等市场资源） |
| `Binaries/` `Intermediate/` `Saved/` `Build/` | ❌ gitignore | 构建产物 |

### 0.2 Content/ 本地管理策略

`Content/` 目录包含以下版权敏感内容，**仅保留在本地，不提交到 GitHub**：

- **Fantastic Village Pack** — 市场购买的中世纪村庄资源包（500+资产）
- **PaperZD 动画资产** — 通过 PaperZD 插件创建的动画序列/精灵表
- **临时/占位贴图** — 开发过程中的测试素材

**协作方案**: 团队成员各自从原始项目拷贝或通过内部文件共享获取 `Content/` 目录。后续可搭建 Perforce/内部 NAS 管理二进制资产。

### 0.3 当前 Git 跟踪状态

```
已跟踪 (8个文件):
  .gitattributes
  .gitignore
  Config/DefaultEditor.ini
  Config/DefaultEngine.ini
  Config/DefaultGame.ini
  Config/DefaultInput.ini
  Platforms/HoloLens/Config/HoloLensEngine.ini
  TES_01.uproject

未来跟踪:
  Source/          (C++ 源代码，重构后创建)
  DesignDocs/      (策划文档)
```

---

## 目录

1. [项目现状分析](#1-项目现状分析)
2. [C++ 重构方案](#2-c-重构方案)
3. [战斗系统设计](#3-战斗系统设计)
4. [联机系统设计](#4-联机系统设计)
5. [数据架构](#5-数据架构)
6. [开发阶段划分](#6-开发阶段划分)
7. [风险与对策](#7-风险与对策)

---

## 1. 项目现状分析

### 1.1 基本信息

| 项目 | 详情 |
|------|------|
| 引擎版本 | Unreal Engine 5.2 |
| 项目类型 | **纯蓝图项目** (WITH_CPP_MODULES=0) |
| 图形API | DirectX 12 (SM5 + SM6) |
| 视角 | 2.5D 俯视角 |
| 动画系统 | PaperZD (2D骨骼动画/Flipbook) |
| 输入系统 | Enhanced Input |
| 打包配置 | Shipping, IoStore, Oodle+Kraken压缩 |

### 1.2 现有蓝图资产盘点

#### 角色系统 (Content/BluePrin/Character/)

```
PaperZDCharacter (引擎插件C++类)
  └── BP_masterZD (蓝图) ─── 所有角色的蓝图父类
       ├── BP_Girl → GirlA (玩家角色)
       │    ├── 动画: 4方向行走、3方向耕种、多方向待机
       │    ├── 动画蓝图: ABP_GirlA
       │    └── 精灵: 32x32帧行走/铲子精灵表
       ├── BP_Monster → Monster (怪物)
       │    ├── 动画: 4方向行走、4方向出拳、死亡
       │    ├── 动画蓝图: ABP_Monster
       │    └── AI控制器: AI_Monster
       ├── VillagerA (村民A)
       │    ├── 动画: 4方向行走、待机
       │    └── 组件: BPC_Villager
       ├── VillagerB (村民B) - 农场任务NPC
       │    └── 组件: BPC_VillagerB
       └── VillagerC (村民C) - 战斗任务NPC
            └── 组件: BPC_VillagerC
```

#### 地图 (6个)

| 地图 | 路径 | 用途 |
|------|------|------|
| MainMenu | Content/UMG/MainMenu.umap | 主菜单/标题界面 |
| map_village_day | Content/BluePrin/maps/map_village_day.umap | 白天村庄 |
| map_village_night | Content/BluePrin/maps/map_village_night.umap | 夜晚村庄 |
| map_village_overview | Content/BluePrin/maps/map_village_overview.umap | 村庄总览 |
| NewVillageMap | Content/BluePrin/maps/NewVillageMap.umap | 新村庄地图 |
| RoomMap | Content/BluePrin/maps/Room/RoomMap.umap | 室内房间 |

#### 主要系统

| 系统 | 核心蓝图 | 功能 |
|------|---------|------|
| **库存系统** | InventoryComponent, S_ItemStructure, BP_Item | 物品拾取、存储、UI显示，支持容器(Chest/Food) |
| **对话系统** | DialogComponent, DialogTree, E_DialogState | 基于行为树的任务对话，支持农场/战斗两条任务线 |
| **农场系统** | BP_Farm, BP_MasterSeed, BPC_FarmVeg | 种植、浇水、生长阶段、收获、烹饪 |
| **天气系统** | WeatherSys, C_DayLength | 天气变化、昼夜循环 |
| **存档系统** | BP_SaveGame | 游戏存档/读档 |
| **小地图** | UI_MiniMap, MiniMapRenderTarget2D | 场景实时小地图 |
| **音乐系统** | BGmusic_Cue, MemuMusic | 背景音乐和UI音效 |

#### 现有输入映射

| 动作 | 按键 | 系统 |
|------|------|------|
| Movement | WASD/手柄 | Enhanced Input (IMC_25D) |
| Jump | 跳跃键 | Enhanced Input (IA_Jump) |
| PickUp | P | Legacy Action |
| Inventory | B | Legacy Action |
| FreeLook | LeftAlt | Legacy Action |

#### 第三方内容

| 资源 | 类型 | 说明 |
|------|------|------|
| Fantastic Village Pack | 市场资源包 | 中世纪村庄套件(~500+资产: 建筑、道具、食物、环境) |
| PaperZD | 市场插件 | 2D精灵骨骼动画系统(核心依赖) |
| FreeSound.org | 音效 | 门开关音效 |

### 1.3 技术债务识别

| 问题 | 严重度 | 说明 |
|------|--------|------|
| 无C++代码 | 🔴 高 | 所有逻辑在蓝图中，性能受限、难以版本控制、不便多人协作 |
| 无模块架构 | 🔴 高 | 蓝图间耦合松散但缺乏统一框架，代码复用性差 |
| 硬编码数据 | 🟡 中 | 物品/作物/敌人属性分散在各蓝图中，缺少DataAsset集中管理 |
| 农场系统双版本 | 🟡 中 | FarmSystem和FarmSystem_1并存，需合并 |
| 命名不一致 | 🟡 中 | Monstor/Monster拼写混用，WAlkS/WalkS大小写不一致 |
| 多张临时贴图 | 🟢 低 | QQ/WX截图作为占位UI贴图 |
| 无联机架构 | 🔴 高 | 所有逻辑为单机设计，需从零搭建网络层 |

---

## 2. C++ 重构方案

### 2.1 模块架构设计

```
Source/
├── TES_01.Target.cs              # 游戏打包目标
├── TES_01Editor.Target.cs        # 编辑器目标
└── TES_01/                       # 主模块
    ├── TES_01.Build.cs           # 模块构建配置
    ├── TES_01.h                  # 模块头文件
    ├── TES_01.cpp                # 模块实现
    ├── Public/                   # 公开头文件
    │   ├── Framework/            # 框架类
    │   │   ├── TES_GameMode25D.h
    │   │   ├── TES_GameState25D.h
    │   │   ├── TES_PlayerController25D.h
    │   │   ├── TES_PlayerState25D.h
    │   │   └── TES_HUD25D.h
    │   ├── Characters/           # 角色类
    │   │   ├── TES_PaperZDCharacter.h      # 基础角色
    │   │   ├── TES_PlayerCharacter.h       # 玩家角色
    │   │   ├── TES_NPCCharacter.h          # NPC基类
    │   │   └── TES_EnemyCharacter.h        # 敌人基类
    │   ├── Components/           # 功能组件
    │   │   ├── TES_HealthComponent.h       # 生命值组件
    │   │   ├── TES_CombatComponent.h       # 战斗组件
    │   │   ├── TES_InventoryComponent.h    # 库存组件
    │   │   ├── TES_DialogComponent.h       # 对话组件
    │   │   └── TES_InteractionComponent.h  # 交互组件
    │   ├── Systems/              # 子系统
    │   │   ├── TES_SessionSubsystem.h      # 会话管理(联机)
    │   │   ├── TES_SaveGameSubsystem.h     # 存档管理
    │   │   ├── TES_GameDataSubsystem.h     # 数据注册表
    │   │   └── TES_FarmingSubsystem.h      # 农场World子系统
    │   ├── Data/                 # 数据资产
    │   │   ├── TES_ItemData.h
    │   │   ├── TES_CropData.h
    │   │   ├── TES_EnemyData.h
    │   │   └── TES_SkillData.h
    │   ├── UI/                   # UI C++基类
    │   │   ├── TES_InventoryWidget.h
    │   │   ├── TES_DialogWidget.h
    │   │   └── TES_HUDWidget.h
    │   ├── AI/                   # AI系统
    │   │   ├── TES_AIController.h
    │   │   ├── TES_BTTask_Attack.h
    │   │   └── TES_BTService_Combat.h
    │   └── Network/              # 网络类型定义
    │       └── TES_NetworkTypes.h
    └── Private/                  # 实现文件(对应上述.h)
        └── (按相同目录结构)
```

### 2.2 C++ 类继承层次

```
PaperZDCharacter (引擎插件)
  └── ATES_PaperZDCharacter (新增C++基类)
       ├── ATES_PlayerCharacter (玩家)
       └── ATES_NPCCharacter (NPC基类)
            └── ATES_EnemyCharacter (敌人/怪物)

AGameModeBase
  └── ATES_GameMode25D

AGameStateBase
  └── ATES_GameState25D

APlayerController
  └── ATES_PlayerController25D

APlayerState
  └── ATES_PlayerState25D

AHUD
  └── ATES_HUD25D

UGameInstanceSubsystem
  ├── UTES_SessionSubsystem (联机会话)
  ├── UTES_SaveGameSubsystem (存档管理)
  └── UTES_GameDataSubsystem (数据注册表)

UWorldSubsystem
  └── UTES_FarmingSubsystem (农场系统)

UActorComponent
  ├── UTES_HealthComponent
  ├── UTES_CombatComponent
  ├── UTES_InventoryComponent
  └── UTES_InteractionComponent

UPaperZDAnimNotify
  ├── UTES_AnimNotify_AttackHitWindow
  └── UTES_AnimNotify_CombatRecovery

USaveGame
  └── UTES_SaveGame

UPrimaryDataAsset
  ├── UTES_ItemDataAsset
  ├── UTES_EnemyDataAsset
  ├── UTES_CropDataAsset
  └── UTES_SkillDataAsset
```

### 2.3 蓝图迁移策略

#### 迁移优先级

| 优先级 | 内容 | 原因 |
|--------|------|------|
| **P0-立即** | 框架类(GameMode/GameState/Controller/Character基类) | 为所有后续迁移提供C++基础 |
| **P1-高** | 战斗组件、生命值组件 | 全新系统，直接在C++编写 |
| **P2-高** | 库存组件、物品数据结构 | 核心循环，涉及网络复制 |
| **P3-中** | 农场World子系统 | 逻辑复杂，适合C++ |
| **P4-中** | 对话组件、AI行为树任务 | 需保留蓝图编辑灵活性 |
| **P5-低** | UI Widget基类 | 逻辑转移到C++，布局保留蓝图 |

#### 保留在蓝图的内容

| 内容 | 原因 |
|------|------|
| 动画蓝图 (ABP_GirlA, ABP_Monster) | PaperZD状态机可视化编辑效率高 |
| Widget布局和样式 | UMG Designer更适合视觉设计 |
| DataTable/DataAsset实例 | 资产引用由美术/策划直接编辑 |
| 行为树结构 | AI策划可视调参 |
| 特效/音效触发 | 美术直接控制的反馈层 |

#### 迁移步骤示例 (BP_masterZD → ATES_PaperZDCharacter)

1. 创建 `ATES_PaperZDCharacter` C++类，继承 `APaperZDCharacter`
2. 在C++中实现：Input绑定框架、方向追踪、动画实例访问器
3. 打开 `BP_masterZD` → Class Settings → 将父类从 `PaperZDCharacter` 改为 `ATES_PaperZDCharacter`
4. 编译验证：所有子蓝图(BP_Girl, BP_Monster)自动继承新父类
5. 逐个将BP_masterZD中的逻辑节点迁移到C++函数

### 2.4 Build.cs 模块依赖

```csharp
// TES_01.Build.cs
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "EnhancedInput",
    "Paper2D",
    "PaperZD",
    "UMG",
    "Slate",
    "SlateCore",
    "AIModule",
    "GameplayTasks",
    "NavigationSystem",
    // 联机依赖
    "OnlineSubsystem",
    "OnlineSubsystemUtils",
    "NetCore",
    // 数据处理
    "Json",
    "JsonUtilities"
});
```

### 2.5 Target.cs 配置

```csharp
// TES_01.Target.cs
public class TES_01Target : TargetRules
{
    public TES_01Target(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
        ExtraModuleNames.Add("TES_01");
    }
}

// TES_01Editor.Target.cs
public class TES_01EditorTarget : TargetRules
{
    public TES_01EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
        ExtraModuleNames.Add("TES_01");
    }
}
```

---

## 3. 战斗系统设计

### 3.1 战斗系统总览

**类型**: 即时动作战斗（2D/2.5D俯视角）

**核心循环**:
```
遭遇敌人 → 锁定/接近 → 轻攻击连击 → 重攻击破防
                      → 闪避躲避 → 反击窗口
                      → 击败敌人 → 获得掉落/经验
```

**设计参考**: 星露谷物语(动作手感) + 哈迪斯(连击节奏) 的简化融合

### 3.2 战斗属性系统

#### 玩家/敌人基础战斗属性

| 属性 | 说明 | 初始值 |
|------|------|--------|
| HP (生命值) | 降至0则死亡 | 100 |
| Attack (攻击力) | 基础伤害 | 10 |
| Defense (防御力) | 减少受到的伤害 | 0 |
| AttackSpeed (攻击速度) | 影响攻击动画播放速度 | 1.0x |
| MoveSpeed (移动速度) | 战斗中的移动速度 | 300 |
| DodgeCount (闪避次数) | 连续闪避上限 | 2 |
| DodgeRecovery (闪避恢复) | 闪避次数恢复间隔 | 2秒/次 |
| CritChance (暴击率) | 暴击概率 | 5% |
| CritDamage (暴击伤害) | 暴击伤害倍率 | 1.5x |
| KnockbackResist (击退抗性) | 抵抗击退的能力 | 0% |

#### 伤害计算公式

```
原始伤害 = 攻击者攻击力 × 技能伤害倍率
暴击判定 = Random(0, 100) < 暴击率
最终伤害 = 原始伤害 × (暴击? 暴击倍率 : 1.0) × (100 / (100 + 目标防御力))
实际伤害 = Max(1, 最终伤害)  // 保底1点伤害
```

### 3.3 攻击系统

#### 轻攻击连击 (3段)

```
轻击1 (横斩)   → 轻击2 (上挑)   → 轻击3 (重击结束)
伤害: 100%      伤害: 120%       伤害: 180%
速度: 0.4s      速度: 0.35s       速度: 0.6s
范围: 扇形60°   范围: 扇形45°     范围: 圆形
可取消: 闪避    可取消: 闪避      可取消: 无(硬直)
```

**连击窗口**: 连续按攻击键，在动画结束前0.3秒内输入，即可连入下一段攻击。超过1.5秒未输入则重置连击。

#### 重攻击

```
重击 (蓄力下劈)
伤害: 250%
速度: 1.0s (蓄力0.3s + 攻击0.7s)
范围: 扇形30°前方
特性: 破防、击退、可打断敌人轻攻击
体力消耗: 30
```

#### 闪避

```
闪避
方向: 8方向
无敌帧: 0.3秒
距离: 150单位
冷却: 1.0秒
体力消耗: 15
```

### 3.4 攻击判定方案

采用**碰撞盒+动画通知**的2D命中检测方案：

```
PaperZD动画序列
  ├─ AnimNotify: "Combat_HitWindowBegin"  → 启用命中碰撞盒
  ├─ AnimNotify: "Combat_HitWindowEnd"    → 禁用碰撞盒，清零已命中列表
  └─ AnimNotify: "Combat_RecoveryDone"    → 恢复Idle状态
```

**碰撞盒配置**:
- 使用 `UBoxComponent`，挂载在角色根组件
- 位置跟随角色面朝向动态偏移
- 仅在攻击命中窗口期间启用碰撞
- 每个攻击窗口只对同一目标命中一次（防穿透重复判定）

**为什么用碰撞盒而非射线**:
- 2D角色在同一Z平面，攻击判定区域可以用矩形明确界定
- 碰撞Overlap天然支持"持续一段时间的攻击判定区域"
- 更符合2D动作游戏的习惯做法

### 3.5 战斗状态机

```
                    ┌─────────────┐
                    │    Idle     │ ←──────────────────┐
                    └──┬──┬──┬───┘                    │
         轻攻击/重攻击│  │  │ 闪避(有次数)            │
                    │  │  └──────────┐                │
                    ▼  │             ▼                │
            ┌──────────┐│     ┌───────────┐           │
            │Attacking ││     │  Dodging  │           │
            └────┬─────┘│     └─────┬─────┘           │
                 │      │           │                  │
            命中/结束   │ 被击中    │ 无敌帧结束       │
                 │      ▼           │                  │
                 │  ┌──────────┐    │                  │
                 └─→│Staggered │    │                  │
                    └────┬─────┘    │                  │
                         │ 硬直结束 │                  │
                         └──────────┴──────────────────┘

            Idle状态下被击杀 → Dead (不可恢复，等待重生/游戏结束)
```

**状态枚举**:
```cpp
UENUM(BlueprintType)
enum class ETES_CombatState : uint8
{
    Idle        UMETA(DisplayName = "待机"),
    Attacking   UMETA(DisplayName = "攻击中"),
    Dodging     UMETA(DisplayName = "闪避中"),
    Staggered   UMETA(DisplayName = "硬直"),
    Dead        UMETA(DisplayName = "死亡")
};
```

### 3.6 敌人AI设计

#### 敌人类型

| 类型 | 行为模式 | 示例 |
|------|---------|------|
| **近战型** | 接近→攻击→后退→巡逻 | 史莱姆、骷髅兵 |
| **远程型** | 保持距离→射击→重新定位 | 幽灵法师 |
| **冲锋型** | 锁定→直线冲锋→转身再冲 | 野猪 |
| **BOSS型** | 多阶段、技能循环、召唤小怪 | 地牢守护者 |

#### 行为树结构

```
Root (Selector)
├── [优先级] 死亡处理 (IsDead? → PlayDeath → Wait → Remove)
├── [优先级] 受击反应 (WasHit? → PlayStagger → Wait)
├── [Combat] 战斗行为 (Selector)
│   ├── [Sequence] 攻击序列
│   │   ├── 检查攻击范围内有目标
│   │   ├── 面向目标
│   │   └── 执行攻击任务(C++ BTTask)
│   └── [Sequence] 追击序列
│       ├── 检查感知范围内有目标
│       └── 移动到目标(C++ BTTask + EQS)
└── [Idle] 待机行为
    └── 巡逻/原地待机
```

#### AI感知配置

| 感知类型 | 配置 |
|----------|------|
| **视觉** | 扇形视角120°，半径600单位(2D投影) |
| **听觉** | 半径400单位，响应战斗音效刺激 |
| **受伤** | 被攻击后立即获取攻击者位置 |

### 3.7 与PaperZD动画集成

```
战斗输入 → C++ CombatComponent::LightAttack()
  ├─ 验证: 可以攻击? (状态检查/体力检查)
  ├─ 选择: 当前连击段的攻击数据
  ├─ 播放: PaperZDAnimInstance->PlayAnimation(AttackData.Animation)
  ├─ 状态: CurrentCombatState = Attacking
  └─ 等待动画通知回调...

动画通知 → C++ AnimNotify_AttackHitWindow::OnReceiveNotify()
  ├─ 启用碰撞盒
  └─ OnComponentBeginOverlap → 对重叠敌人造成伤害

动画通知 → C++ AnimNotify_CombatRecovery::OnReceiveNotify()
  ├─ 禁用碰撞盒
  ├─ 清零已命中列表
  └─ CurrentCombatState = Idle
```

### 3.8 UI与反馈

| 元素 | 实现 |
|------|------|
| 生命条 | UMG ProgressBar，绑定UTes_HealthComponent::GetHealthPercent |
| 体力条 | UMG ProgressBar，绑定UTes_CombatComponent::GetStaminaPercent |
| 伤害数字 | 浮动UMG Text，池化复用，0.8秒渐隐上飘 |
| 受击闪白 | 角色的Sprite材质切换为白色闪烁0.1秒 |
| 击杀特效 | Niagara粒子爆发 + 掉落物弹出 |
| 屏幕震动 | 重攻击命中时触发CameraShake |

---

## 4. 联机系统设计

### 4.1 网络拓扑

**选择**: **Listen Server (监听服务器)** — 一个玩家做主机，其他人加入。

```
┌──────────┐
│  Host PC │ ← 既是服务器也是玩家1
│ (Server) │
└────┬─────┘
     │ (网络)
┌────┴─────┐     ┌──────────┐     ┌──────────┐
│ Player 2  │    │ Player 3  │    │ Player 4  │
│ (Client)  │    │ (Client)  │    │ (Client)  │
└───────────┘    └──────────┘    └──────────┘
```

**选择理由**:
- 2-4人规模，Listen Server完全能胜任
- 好友自建房间，无需专用服务器成本
- UE5的Iris网络系统原生支持此模式
- Farming游戏节奏不快，网络延迟容忍度较高

### 4.2 网络权威模型

**Server-Authoritative (主机权威)**:

| 系统 | 权威方 | 防作弊策略 |
|------|--------|-----------|
| 生命值/伤害 | 服务端 | 客户端发送攻击请求，服务端计算实际伤害 |
| 战斗命中 | 服务端 | 碰撞盒在服务端Tick，客户端仅做表现层预测 |
| 库存 | 服务端 | 物品增删均由服务端执行并同步 |
| 作物生长 | 服务端 | WorldSubsystem在服务端Tick生长逻辑 |
| 任务进度 | 服务端 | 任务状态变更由服务端驱动 |
| 金币/货币 | 服务端 | 所有交易由服务端验证 |

**Client-Authoritative (客户端本地)**:

| 系统 | 权威方 | 说明 |
|------|--------|------|
| 移动 | 客户端预测+服务端校正 | 流畅移动体验 |
| UI | 客户端本地 | 不涉及游戏逻辑 |
| 特效/音效 | 客户端本地 | 通过Multicast RPC触发 |
| 动画播放 | 客户端本地 | 视觉表现，不影响游戏逻辑 |

### 4.3 Actor复制策略

#### 复制属性 (Automatic Replication)

```cpp
// ATES_PlayerCharacter
UPROPERTY(Replicated)
bool bIsAlive;

UPROPERTY(Replicated)
ETES_CombatState CurrentAction;

UPROPERTY(Replicated)
FVector2D LastMoveDirection;

// UTES_HealthComponent
UPROPERTY(ReplicatedUsing = OnRep_Health)
float CurrentHealth;

UPROPERTY(ReplicatedUsing = OnRep_IsDead)
bool bIsDead;

// UTES_CombatComponent
UPROPERTY(Replicated)
ETES_CombatState CurrentCombatState;

UPROPERTY(Replicated)
int32 CurrentComboIndex;

// UTES_InventoryComponent
UPROPERTY(Replicated)
TArray<FTES_InventorySlotData> InventoryItems;

// ATES_GameState25D
UPROPERTY(Replicated)
float GameTimeOfDay;

UPROPERTY(Replicated)
ETES_WeatherType CurrentWeather;

UPROPERTY(Replicated)
TArray<FTES_CropPlotData> CropPlots;  // 所有农场作物状态
```

#### 远程过程调用 (RPC)

```cpp
// --- 服务端RPC (Client → Server) ---

// 战斗
UFUNCTION(Server, Reliable)
void ServerLightAttack();
UFUNCTION(Server, Reliable)
void ServerHeavyAttack();
UFUNCTION(Server, Reliable)
void ServerDodge(FVector2D Direction);

// 交互
UFUNCTION(Server, Reliable)
void ServerPickupItem(AActor* Item);
UFUNCTION(Server, Reliable)
void ServerUseItem(FName ItemID);
UFUNCTION(Server, Reliable)
void ServerSelectDialogChoice(FName ChoiceID);

// 农场
UFUNCTION(Server, Reliable)
void ServerPlantSeed(int32 PlotID, FName SeedType);
UFUNCTION(Server, Reliable)
void ServerWaterCrop(int32 PlotID);

// --- 多播RPC (Server → All Clients) ---

UFUNCTION(NetMulticast, Reliable)
void MulticastPlayAttackEffect(FName EffectID, FVector Location);
UFUNCTION(NetMulticast, Reliable)
void MulticastPlayAttackAnimation(int32 AttackIndex);
UFUNCTION(NetMulticast, Reliable)
void MulticastUpdateCropStage(int32 PlotID, int32 Stage);
UFUNCTION(NetMulticast, Reliable)
void MulticastOnPlayerJoined(const FString& PlayerName);
UFUNCTION(NetMulticast, Reliable)
void MulticastOnPlayerLeft(const FString& PlayerName);
```

#### 不复制的内容

| 内容 | 原因 |
|------|------|
| PaperZD动画状态机 | 视觉表现，各客户端独立播放 |
| UMG Widget状态 | 每个客户端有自己的UI实例 |
| 静态场景/建筑 | 通过关卡加载同步 |
| AI行为树内部状态 | AI在服务端运行，结果通过角色状态同步 |
| 粒子特效 | 通过Multicast RPC触发，不需持续同步 |

### 4.4 会话管理 (OnlineSubsystem)

#### 实现步骤

```cpp
UCLASS()
class UTES_SessionSubsystem : public UGameInstanceSubsystem
{
public:
    // 创建房间(主机)
    UFUNCTION(BlueprintCallable)
    void HostSession(FName SessionName, int32 MaxPlayers = 4);
    
    // 搜索房间
    UFUNCTION(BlueprintCallable)
    void FindSessions();
    
    // 加入房间
    UFUNCTION(BlueprintCallable)
    void JoinSession(int32 SessionIndex);
    
    // 离开房间
    UFUNCTION(BlueprintCallable)
    void LeaveSession();
    
    // 事件委托
    UPROPERTY(BlueprintAssignable)
    FOnSessionsFound OnSessionsFound;     // 搜索到房间列表
    
    UPROPERTY(BlueprintAssignable)
    FOnSessionCreated OnSessionCreated;   // 房间创建成功
    
    UPROPERTY(BlueprintAssignable)
    FOnSessionJoined OnSessionJoined;     // 加入成功
    
    UPROPERTY(BlueprintAssignable)
    FOnSessionLeft OnSessionLeft;         // 离开房间
};
```

#### 分阶段上线策略

| 阶段 | 平台 | 实现 |
|------|------|------|
| Phase 1 (开发测试) | LAN | OnlineSubsystemNull |
| Phase 2 (内测) | Steam | OnlineSubsystemSteam |
| Phase 3 (公测) | 跨平台 | Epic Online Services (EOS) |

### 4.5 玩家加入/离开流程

```
[主机] 主菜单 → 点击"创建房间"
  → SessionSubsystem::HostSession()
  → 加载关卡 (Listen Server启动)
  → 主机角色生成
  → 等待其他玩家...

[客户端] 主菜单 → 点击"加入房间"
  → SessionSubsystem::FindSessions()
  → 显示房间列表
  → 点击房间 → SessionSubsystem::JoinSession()
  → 连接主机 → 加载同步关卡
  → GameMode::PostLogin() → 生成玩家角色
  → MulticastOnPlayerJoined() 通知所有人

[玩家离开]
  → 客户端主动离开 or 网络超时
  → GameMode::Logout() → 销毁角色
  → 保存该玩家的数据到主机存档
  → MulticastOnPlayerLeft() 通知

[主机断开]
  → 所有客户端返回主菜单
  → 显示"主机已断开连接"
  → (后续版本: 主机迁移)
```

### 4.6 多人存档系统

```
存档策略: 存档存在于主机(Host)端，主机是数据的唯一权威源

┌──────────┐
│ Host存档  │ ← 包含:
│           │    1. 世界状态 (作物、天气、时间)
│           │    2. Player1 数据 (背包、属性、任务)
│           │    3. Player2 数据
│           │    4. Player3 数据
│           │    5. Player4 数据
└──────────┘

玩家加入时:
  主机从存档读取该玩家的数据 → 设置到PlayerState → 同步给客户端

玩家离开时:
  主机收集该玩家的最新数据 → 写入存档

存档文件格式:
  Saved/SaveGames/<SessionName>_Host.sav  (主机)
  Saved/SaveGames/<PlayerID>_Cache.sav    (客户端的本地缓存副本)
```

### 4.7 网络优化

| 优化项 | 方法 |
|--------|------|
| 复制条件 | `DOREPLIFETIME_CONDITION` + `COND_OwnerOnly` (库存只同步给物主) |
| 相关性裁剪 | 2D地图中，远离玩家的敌人/作物不复制 |
| 网络更新频率 | 战斗角色30Hz，NPC/作物10Hz |
| RPC聚合 | 快速连续的攻击输入合并为一次RPC |
| 带宽预算 | 预留上行512Kbps、下行1Mbps（2-4人足够） |

---

## 5. 数据架构

### 5.1 DataAsset 资产体系

所有游戏数据使用 `UPrimaryDataAsset` 派生类，在编辑器中可视化编辑：

```
物品数据 (UTES_ItemDataAsset)
├── ItemID: FName
├── DisplayName: FText
├── Description: FText
├── Icon: UTexture2D*
├── ItemType: ETES_ItemType (消耗品/武器/种子/材料/任务)
├── MaxStackSize: int32
├── BuyPrice/SellPrice: int32
├── [条件] WeaponStats: FTES_WeaponStats
├── [条件] ConsumableEffect: FTES_ConsumableEffect
└── [条件] SeedData: FTES_SeedData

敌人数据 (UTES_EnemyDataAsset)
├── EnemyID: FName
├── DisplayName: FText
├── CharacterClass: TSubclassOf<ATES_EnemyCharacter>
├── MaxHealth: float
├── AttackDamage: float
├── MoveSpeed: float
├── DetectionRange: float
├── AttackData: FTES_AttackData
├── LootTable: TArray<FTES_LootDrop>
└── BehaviorTree: UBehaviorTree*

作物数据 (UTES_CropDataAsset)
├── SeedID: FName
├── HarvestItemID: FName
├── GrowthDays: int32
├── GrowthStageSprites: TArray<UTexture2D*>
├── bRegrowsAfterHarvest: bool
├── RegrowDays: int32
└── ValidSeasons: TArray<FName>
```

### 5.2 GameInstanceSubsystem 数据注册表

```
UTES_GameDataSubsystem (随游戏实例存在)
├── ItemRegistry: TMap<FName, UTES_ItemDataAsset*>     // 启动时扫描加载所有物品
├── EnemyRegistry: TMap<FName, UTES_EnemyDataAsset*>   // 启动时扫描加载所有敌人
└── CropRegistry: TMap<FName, UTES_CropDataAsset*>     // 启动时扫描加载所有作物
```

**访问方式**: `GetGameInstance()->GetSubsystem<UTES_GameDataSubsystem>()->GetItemData("Pumpkin")`

### 5.3 存档数据结构 (UTES_SaveGame)

```cpp
UCLASS()
class UTES_SaveGame : public USaveGame
{
    // === 世界状态 ===
    UPROPERTY()
    FName CurrentMap;
    UPROPERTY()
    float GameTimeOfDay;
    UPROPERTY()
    int32 DayNumber;
    UPROPERTY()
    int32 SeasonIndex;
    UPROPERTY()
    int32 GameYear;
    UPROPERTY()
    ETES_WeatherType CurrentWeather;
    
    // === 农场状态 ===
    UPROPERTY()
    TArray<FTES_CropPlotSaveData> CropPlots;
    
    // === 玩家状态 (多人) ===
    UPROPERTY()
    TMap<FString, FTES_PlayerSaveData> PlayerDataMap;
    
    // === 世界物品 ===
    UPROPERTY()
    TArray<FTES_WorldItemSaveData> WorldItems;
    
    // === 任务 ===
    UPROPERTY()
    TArray<FTES_QuestSaveData> ActiveQuests;
    UPROPERTY()
    TArray<FName> CompletedQuests;
    
    // === 元数据 ===
    UPROPERTY()
    FDateTime SaveDateTime;
    UPROPERTY()
    int32 SaveVersion = 1;
};
```

---

## 6. 开发阶段划分

### Phase A: C++ 项目转换 (预计 1-2 周)

**目标**: 项目成功编译C++模块，所有现有蓝图功能正常运行。

| 任务 | 产出 |
|------|------|
| 创建 Source/ 目录结构 | 完整的目录树 |
| 编写 TES_01.Build.cs | 模块依赖配置 |
| 编写 TES_01.Target.cs / TES_01Editor.Target.cs | 构建目标 |
| 更新 TES_01.uproject (添加Modules) | 项目识别为C++项目 |
| 生成VS工程文件并编译 | 编译通过，无错误 |
| PIE测试：所有现有功能正常 | 100%功能兼容 |

### Phase B: 核心框架 (预计 2-3 周)

**目标**: C++框架类就位，原有蓝图重新挂接到C++父类。

| 任务 | 涉及文件 |
|------|---------|
| ATES_PaperZDCharacter | 角色基类 |
| ATES_PlayerCharacter | 玩家角色 |
| ATES_NPCCharacter / ATES_EnemyCharacter | NPC/敌人基类 |
| ATES_GameMode25D / GameState / Controller / State / HUD | 框架五件套 |
| 蓝图重挂接: BP_masterZD → ATES_PaperZDCharacter | 所有角色蓝图 |
| 蓝图重挂接: BP_GameMode25D → ATES_GameMode25D | 游戏模式 |
| UTES_InteractionComponent | 交互组件 |
| **里程碑**: 框架编译通过，游戏可运行，行为不变 |

### Phase C: 战斗系统 (预计 3-4 周)

**目标**: 完整的单机即时战斗体验。

| 任务 | 涉及文件 |
|------|---------|
| UTES_HealthComponent | 生命值/受伤/死亡/回血 |
| UTES_CombatComponent | 连击/重击/闪避/命中检测 |
| FTES_AttackData / FTES_DamageEvent | 数据结构 |
| ETES_CombatState 枚举 | 状态机定义 |
| PaperZD AnimNotify子类 | AttackHitWindow / CombatRecovery |
| 玩家输入绑定 (Attack/Dodge) | IMC_25D扩展 |
| 敌人AI: BT Tasks (攻击/追击/巡逻) | C++行为树任务 |
| 敌人AI: BT Services (目标检测/距离判断) | C++行为树服务 |
| UTES_EnemyDataAsset | 敌人数据资产 |
| UI: 伤害数字、生命条、体力条 | UMG Widget |
| 击杀/掉落/重生系统 | 敌人生命周期 |
| **里程碑**: 单机战斗完全可玩，3种以上敌人类型 |

### Phase D: 联机系统 (预计 4-5 周)

**目标**: 2-4人联机合作流畅运行。

| 任务 | 涉及文件 |
|------|---------|
| UTES_SessionSubsystem | 会话管理 |
| 组件Replication配置 | 所有战斗/库存组件的网络属性 |
| RPC实现 | Server/Client/Multicast函数 |
| 主机/加入UI界面 | MainMenu扩展 |
| GameMode: PostLogin/Logout | 玩家生成/清理 |
| GameState: 世界状态复制 | 时间/天气/作物 |
| 服务端战斗验证 | 防作弊 |
| 多人存档: 主机数据分发 | SaveGameSubsystem |
| OnlineSubsystemNull LAN测试 | 本地多窗口测试 |
| OnlineSubsystemSteam集成 | Steam好友联机 |
| 网络优化: 相关性/更新频率 | 带宽优化 |
| **里程碑**: 2-4人联机合作可玩，LAN/Steam均可 |

### Phase E: 蓝图迁移与打磨 (预计 2-4 周+)

**目标**: 将剩余关键蓝图逻辑迁移到C++，整体优化。

| 任务 | 说明 |
|------|------|
| 库存组件蓝图逻辑 → C++ | 物品增删、排序、堆叠 |
| 对话组件核心 → C++ | 状态机核心逻辑 |
| 农场子系统 → UTES_FarmingSubsystem | 生长Tick、季节校验 |
| 天气系统 → C++ | 昼夜循环核心 |
| DataAsset填充 | 策划工具制作 |
| 性能优化 | 60FPS目标 |
| 打包测试 | Shipping配置验证 |
| 主机迁移 (可选) | 高难度，视时间决定 |

---

## 7. 风险与对策

| 风险 | 概率 | 影响 | 对策 |
|------|------|------|------|
| **蓝图→C++迁移导致功能回退** | 中 | 高 | 每迁移一个系统后立即PIE全功能回归测试，Git分支策略保证可回退 |
| **PaperZD与C++动画通知兼容性** | 低 | 中 | PaperZD支持标准AnimNotify，已验证；如遇问题可降级为蓝图桥接 |
| **网络延迟影响战斗手感** | 中 | 高 | 客户端预测+服务端校正；闪避使用客户端先行的宽容策略 |
| **Iris网络系统不成熟(5.2)** | 低 | 中 | Iris在5.2为Beta；如有问题降级使用传统Replication |
| **OnlineSubsystemSteam审核周期** | 中 | 低 | 先用Null子系统完成所有逻辑和测试，Steam只是上线层切换 |
| **PaperZD不支持网络同步** | 低 | 中 | 动画不同步，只同步状态(Attacking/Dodging)，客户端各自播放本地动画 |
| **Listen Server主机性能压力** | 中 | 中 | 2D游戏Tick开销低，4人压力可控；预留性能预算 |
| **农场系统两个版本合并复杂** | 中 | 低 | Phase E处理，先保持两个版本共存，后续用Feature Flag切换 |
| **Git LFS的.uasset合并冲突** | 高 | 低 | 制定资产锁策略：角色动画/Blueprint用单人编辑+通知机制 |

---

## 附录

### A. 文件命名规范

| 前缀 | 类型 | 示例 |
|------|------|------|
| `ATES_` | C++ Actor类 | ATES_PlayerCharacter |
| `UTES_` | C++ UObject/Component类 | UTES_HealthComponent |
| `FTES_` | C++ 结构体 | FTES_AttackData |
| `ETES_` | C++ 枚举 | ETES_CombatState |
| `BP_` | 蓝图类 | BP_Girl |
| `BPC_` | 蓝图组件 | BPC_Pumkin |
| `PF_` | PaperFlipbook动画 | PF_GirlA_Walk_A |
| `ABP_` | 动画蓝图 | ABP_GirlA |
| `AS_` | 动画状态机 | AS_Monstor |
| `PB_` | PaperZD角色实例 | PB_VillagerB |

### B. 关键PaperZD动画序列清单（战斗系统需要新增）

| 角色 | 新增动画 | 方向数 |
|------|---------|--------|
| GirlA (玩家) | Attack_Light1/2/3 | 4方向 × 3段 = 12 |
| GirlA (玩家) | Attack_Heavy | 4方向 = 4 |
| GirlA (玩家) | Dodge | 4方向 = 4 |
| GirlA (玩家) | Hurt | 4方向 = 4 |
| GirlA (玩家) | Dead | 1 |
| Monster | Hurt | 4方向 = 4 |
| 新敌人类型 | 全套战斗动画 | 视敌人类型而定 |

### C. 参考资源

- PaperZD官方文档: [Critical Failure Studio]
- UE5 Iris网络文档: `UE_WITH_IRIS` 相关引擎文档
- OnlineSubsystem Steam: Epic Games文档
- 战斗设计参考: Stardew Valley战斗、Hades动作系统

---

> **文档维护**: 本文档随开发进度更新。每个Phase完成后更新对应章节的实施记录。
