# 用户指南

固件：linnstrument-firmware-GZver 2.3.4-x7

本指南介绍本项目新增的演奏模式。除特别说明外，LinnStrument 原有操作仍遵循上游 2.3.4 的行为。

## 3×4 Scalar Layout（音阶布局）

在 Global Settings（全局设置）中触摸第 19 列最下方的自定义格（LinnStrument 200）。常亮白色表示开启；与 Tap Tempo 同步的蓝色闪烁表示关闭。此模式是全局模式，并与 Harpejji 模式互斥。

水平方向每跨一格变化 3 个半音，垂直方向每跨一格变化 4 个半音。继续使用原有 Global Root、Scale/Mode、移调和 Split 设置。调内音按原有音阶级数灯光显示；调外音不亮，但仍可演奏。Main 和 Accent 灯光继续遵循 LinnStrument 原有设置。

手指跨越触控格边界时，音高按当前音阶逐级移动；快速跨过多格也会补发经过的级进音。触发反馈只点亮实际 MIDI 音高完全相同（含八度）的格子；手指滑到的物理格不会额外变红。

## Dynamic Strum（动态扫弦）

在 Per-Split Settings → Special → Strum 中操作。短按循环为：关闭 → Classic Strum → Dynamic Strum → 关闭。Classic Strum 保持原版行为。

设为 Dynamic 的 Split 是扫弦输出区；另一侧成为不直接发声的 Voicing Input（和弦排列输入区）。Dynamic 设置格显示红色，演奏输出区域显示白色；Voicing 一侧保留原有音符灯光。

| 设置 | Voicing 输入 | 扫弦输出 |
| --- | --- | --- |
| 左侧 Dynamic | 右侧 | 左侧 |
| 右侧 Dynamic | 左侧 | 右侧 |

同一时间只能有一个 Dynamic Split。在另一侧开启 Dynamic 会安全转移角色。Dynamic 依赖 Split 模式；关闭 Split 会安全结束正在发声的 Dynamic 音符。

系统按实际 MIDI 音高收集并排序按住的和弦音，保留转位、重复八度、低音和开放排列。每次扫弦手势开始时取得一份音符快照，再将其映射为从下到上严格递增的 8 行音高。没有按住的和弦音时不会发声。例如：

~~~text
C3 E3 G3 -> C3 E3 G3 C4 E4 G4 C5 E5
E3 G3 C4 -> E3 G3 C4 E4 G4 C5 E5 G5
C2 G2 E4 -> C2 G2 E3 C4 G4 E5 C6 G6
~~~

本轮手势的快照可避免中途更换和弦时改变已经发出的音。只要 Voicing 或 Strum 一侧仍有触摸，音符就可以持续。再次扫到仍在发声的音高会先正确结束旧音再重新触发，不会无控制地叠加 Note On。原有每 Split 的 Legato 设置会影响不同手势之间的衔接。

## Harpejji 模式

在 LinnStrument 200 的 Global Settings 中触摸第 20 列最下方的格子，也就是 Scalar Layout 入口右侧一格。常亮白色表示开启；与 Tap Tempo 同步的蓝色闪烁表示关闭。它与 Scalar Layout 互斥。

将机器旋转到功能键一侧朝向演奏者。按此方向摆放时，音高从左向右、从下向上升高：每列增加一个半音，每行增加两个半音。Y 轴用于表现性弯音。Low Row 延续原系统设置；在该摆放方向下，它对应最靠近演奏者的一行。

Harpejji 使用原有 Main/Accent 音符灯光设置。在 One Channel 模式下进入 Harpejji 时，Pitch/X 会临时关闭，使纵向移动可以重新触发音符。仍可在 Per-Split Settings → Pitch/X 手动重新开启；开启后 X 轴处理遵循原系统。退出 Harpejji 后恢复普通模式行为。

Harpejji 开启期间，Switch 1/2 中作用于 Split 的操作会同时应用到两个 Split。Tap Tempo 等原本只应全局执行一次的操作仍只执行一次。一次按下/松开的目标会保持一致。退出 Harpejji 后，Switch 恢复原有行为。

## 固件与支持范围

本版本面向 LinnStrument 200，并已在该型号测试。LinnStrument 128 尚未完成全面验证。刷机请参阅 INSTALLATION_zh-CN.md；测试范围请参阅 HARDWARE_TESTING_zh-CN.md。
