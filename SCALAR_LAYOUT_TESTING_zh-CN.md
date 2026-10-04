# linnstrument-firmware-GZver 2.3.4-x7 实机测试清单

本清单适用于非官方 LinnStrument 200 自定义固件。LinnStrument 200 已进行实机测试；LinnStrument 128 尚未全面验证。

## 版本与安装

1. 确认设备显示 234-x7。
2. 使用官方 LinnStrument Updater 安装 BIN 文件。
3. 测试前保留官方固件恢复副本。

## 3×4 Scalar Layout

1. 进入 Global Settings，触摸第 19 列最下方。
2. 确认关闭时随 Tap Tempo 蓝色闪烁，开启时常亮白色。
3. 确认横向每格 ±3 半音、纵向每格 ±4 半音。
4. 确认开启时使用低八度虚拟音域，关闭时恢复普通音域，且不改写保存的普通 Octave 设置。
5. 在 C 大调下向各方向滑过一个或多个触控边界。
6. 确认调外格仍可发声但不亮。
7. 确认红灯只覆盖实际 MIDI 音高完全相同（含八度）的格子。
8. 连续演奏与滑动时，检查其他纵列是否出现意外闪烁。

## Dynamic Strum

1. 开启 Split。
2. 在任一 Split 的 Per-Split Settings → Special → Strum 中操作。
3. 确认短按循环为关闭 → Classic → Dynamic → 关闭。
4. 确认 Dynamic 设置格为红色、Dynamic 演奏区为白色。
5. 确认对侧保留正常音符灯光并作为静音和弦输入区。
6. 分别测试左侧和右侧 Dynamic。
7. 测试转位、开放排列、快照、持续音、重触发和 Legato。
8. 关闭 Split 或 Dynamic，确认音符全部释放且普通灯光恢复。

## Harpejji 模式

1. 进入 Global Settings，触摸第 20 列最下方。
2. 确认开启常亮白色、关闭为 Tap Tempo 同步蓝色闪烁。
3. 功能键一侧朝向演奏者纵向摆放。
4. 确认音高从左向右、从下向上升高。
5. 在 One Channel 模式确认 Pitch/X 临时关闭时纵向移动会重触发。
6. 手动开启 Pitch/X，确认 X 轴行为符合原系统；退出 Harpejji 后确认普通模式恢复。
7. 测试最靠近演奏者一行的 Low Row 行为。
8. 确认进入 Split 设置预览时音高布局不闪回普通排列。
9. 确认 Harpejji 开启时 Switch 1/2 的 Split 操作覆盖两侧、全局单次操作仍只执行一次；关闭 Harpejji 后恢复原 Switch 行为。

如遇漏触发、意外 MIDI 事件、卡音、灯光残留或不安全手感，请停止测试并记录复现步骤。
