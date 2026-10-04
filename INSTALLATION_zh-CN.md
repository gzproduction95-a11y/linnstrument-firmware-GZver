# 安装与固件更新

发布版本：linnstrument-firmware-GZver 2.3.4-x7

## 开始之前

1. 确认固件适用于你的 LinnStrument 型号。已测试型号为 LinnStrument 200；128 尚未完成全面验证。
2. 按正常支持的流程备份重要用户设置或 Projects。
3. 从本仓库 GitHub Release 下载 **linnstrument-firmware-GZver-2.3.4-x7.bin**，并确认发布标签为 **v2.3.4-x7**。
4. 准备好官方 LinnStrument 2.3.4 固件和官方更新器，以便需要时恢复。

## 更新步骤

1. 启动官方 LinnStrument Updater。
2. 按更新器提示连接设备并进入更新流程。
3. 在提示时选择下载的 x7 BIN 文件。
4. 更新期间不要断开 USB 或电源。
5. 等待更新器报告完成，再按其提示重启或重新连接设备。
6. 确认设备显示固件标识 **234-x7**，并在演出前测试基本触摸和 MIDI 功能。

不要使用未经验证的 bootloader 或第三方刷写流程。直接通过 Arduino 上传主要用于开发，可能影响已保存的设置、Projects 或校准数据。如果更新失败，请先使用官方恢复流程和官方固件恢复，再尝试其他自定义固件。

## 更新后

测试两个自定义布局开关、普通音符输入、Split 操作，以及你实际依赖的 MIDI/MPE 设置。详见 USER_GUIDE_zh-CN.md 和 HARDWARE_TESTING_zh-CN.md。

这是非官方固件修改版。只有在了解恢复方法并接受风险后再安装。
