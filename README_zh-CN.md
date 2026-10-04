# linnstrument-firmware-GZver

## 项目简介

本项目是基于 Roger Linn Design LinnStrument 固件 2.3.4 的非官方修改版，当前发布版本为 **2.3.4-x7**。它增加了可选的 3×4 Scalar Layout、Dynamic Strum 和 Harpejji 模式，并尽可能保留 LinnStrument 原有操作与 MIDI 架构。

项目面向希望尝试替代音高布局、和弦排列演奏、表现性演奏和 MPE 的 LinnStrument 用户。本项目不是 Roger Linn Design 官方固件发布。

## 上游固件与兼容性

- 固件基线：LinnStrument OS 2.3.4
- 自定义版本：**2.3.4-x7**
- 目标平台：Arduino Due
- 已测试设备：LinnStrument 200
- LinnStrument 128 尚未完成全面验证

[官方上游固件仓库](https://github.com/rogerlinndesign/linnstrument-firmware)

## 主要功能

- LinnStrument 200 全局 3×4 Scalar Layout。
- 按当前音阶级进的 Scalar Swipe（滑动演奏）。
- 左右对称、按 Split 独立设置的 Dynamic Strum。
- 保留和弦排列、转位与开放排列的八行扫弦映射。
- Dynamic Sustain、手势和弦快照、Retrigger 与原有 Legato 衔接。
- 适合纵向摆放演奏的 Harpejji 模式。
- 非自定义模式下尽量保留 LinnStrument 原有演奏、设置、音序器、琶音器和 MIDI 功能。

详细功能请读[中文用户指南](USER_GUIDE_zh-CN.md)；刷写请读[中文安装指南](INSTALLATION_zh-CN.md)。各类中英文文档均列在下方。

## 项目文档

- 项目介绍：[English README](README.md) / [中文 README](README_zh-CN.md)
- 用户指南：[English](USER_GUIDE.md) / [中文](USER_GUIDE_zh-CN.md)
- 安装指南：[English](INSTALLATION.md) / [中文](INSTALLATION_zh-CN.md)
- 构建说明：[English](BUILDING.md) / [中文](BUILDING_zh-CN.md)
- 实机测试：[English](HARDWARE_TESTING.md) / [中文](HARDWARE_TESTING_zh-CN.md)
- 发布说明：[English](RELEASE_NOTES.md) / [中文](RELEASE_NOTES_zh-CN.md)
- 更新日志：[English](CHANGELOG.md) / [中文](CHANGELOG_zh-CN.md)
- Scalar 实机清单：[English](SCALAR_LAYOUT_TESTING.md) / [中文](SCALAR_LAYOUT_TESTING_zh-CN.md)
- 发布构建基线：[English](BASELINE.md) / [中文](BASELINE_zh-CN.md)

## 3×4 Scalar Layout

在 Global Settings 中触摸第 19 列最下方的自定义格。常亮白色表示开启；与 Tap Tempo 同步的蓝色闪烁表示关闭。

- 横向每格：±3 个半音。
- 纵向每格：±4 个半音。
- 继承原有 Root、Scale/Mode、移调和 Split 音乐设置。
- 调外音灯灭但仍可演奏；Main/Accent 灯光沿用原设置。
- 滑动时按当前音阶级进；红色触发反馈只显示实际 MIDI 音高完全相同（含八度）的格子。

该布局的概念灵感来自 Mike Gao 的 Polyplayground 应用。这只是设计灵感来源，不代表 Mike Gao 参与或认可本项目。

## Dynamic Strum

入口为 Per-Split Settings → Special → Strum。短按循环：关闭 → Classic Strum → Dynamic Strum → 关闭。Dynamic 设置格为红色，扫弦输出区为白色；另一 Split 作为和弦输入并保留原有音符灯光。

开启 Dynamic 的 Split 就是 Strum Output，另一侧自动是 Voicing Input：

| 设置 | 和弦输入 | 扫弦输出 |
| --- | --- | --- |
| 左侧 Dynamic | 右侧 | 左侧 |
| 右侧 Dynamic | 左侧 | 右侧 |

系统保留实际 MIDI 音高顺序及转位，并把当前和弦映射为自下而上递增的八行音高。无和弦输入时静音；每轮扫弦开始时读取和弦快照，支持持续音、重触发和原有 Legato 行为。

## Harpejji 模式

在 Global Settings 触摸第 20 列最下方的自定义格（Scalar Layout 入口右侧一格）。常亮白色表示开启；蓝色 Tap Tempo 闪烁表示关闭；两种自定义布局互斥。

机器应纵向摆放并让功能键一侧靠近演奏者。按此方向，音高从左向右、从下向上升高：每列一个半音，每行两个半音。Y 轴用于揉弦/弯音；Low Row 延续原系统设置，表示最靠近演奏者的一行。

One Channel 模式进入 Harpejji 时会临时关闭 Pitch/X，使纵向移动可以重新触发音符；用户仍可手动开启 Pitch/X。退出模式后恢复普通布局行为。Harpejji 模式下，Switch 1/2 中针对 Split 的操作应用于两个 Split；Tap Tempo 等全局单次操作仍只执行一次。其他模式中的 Switch 行为不变。

## 安装、操作与构建

刷写前请备份重要设置，并确认能够用官方固件恢复。使用官方 LinnStrument Updater 安装 GitHub Release 中的 **linnstrument-firmware-GZver-2.3.4-x7.bin**。更多步骤参阅 [INSTALLATION_zh-CN.md](INSTALLATION_zh-CN.md)。

构建目标为 Arduino Due，使用 Arduino SAM Boards 1.6.11。仓库脚本为：

~~~sh
scripts/compile-firmware.sh release-2.3.4-x7
~~~

## 已知限制

本版本已在 LinnStrument 200 上进行实机测试，项目所有者反馈整体稳定、基本正常；并非每一种 MIDI/MPE 设置和第三方合成器组合都经过穷尽测试。LinnStrument 128 兼容性尚未全面验证。详见 [实机测试说明](HARDWARE_TESTING_zh-CN.md)。

## 致谢

特别感谢 Roger Linn 以及整个 LinnStrument / Roger Linn Design 团队创造 LinnStrument，并开放固件源码供修改与实验。

3×4 Scalar Layout 的概念灵感来自 Mike Gao 的 Polyplayground 应用，不代表其参与或背书本项目。

## 许可证

上游 LinnStrument 固件及本修改版采用 Apache License 2.0。仓库保留上游 LICENSE、源码版权声明及第三方归属信息，详见 [LICENSE.txt](LICENSE.txt)。

## 免责声明

这是非官方固件修改版，与 Roger Linn Design 无隶属关系，也未获其官方支持。使用风险由用户自行承担。刷写前请了解如何恢复官方 LinnStrument 固件。
