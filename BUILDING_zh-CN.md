# 构建说明

目标平台：Arduino Due，FQBN 为 **arduino:sam:arduino_due_x**。已验证构建使用 Arduino SAM Boards **1.6.11** 和仓库内的 **scripts/compile-firmware.sh** 脚本。仓库还包含 DueFlashStorage 依赖。

## 使用项目构建流程

使用项目准备好的离线 Arduino CLI/工具链，在仓库根目录运行：

~~~sh
scripts/compile-firmware.sh release-2.3.4-x7
~~~

脚本会暂存固件源码和头文件，针对 Arduino Due 编译，并将生成文件放入 **build/**。可分发固件是生成的 **.bin** 文件。构建缓存和工具链属于本机环境，已由 Git 忽略，不包含在源码发布中。

## 构建核验

- 确认编译器报告成功。
- 确认输出 BIN 文件存在且非空。
- 确认源码版本标识为 234-x7。
- 分发修改后的构建前，运行仓库测试和 **git diff --check**。

上游源码可能产生编译警告，应检查警告；只有构建命令成功退出，才能报告构建成功。
