# FolderSync：文件夹同步

## 功能介绍

本项目的功能是使 “一个或一组目标文件夹” 与 “一个源文件夹” 保持内容的一致。



## 示例

```cpp
// sample_folder 文件夹下有三个文件夹，
// 其中 src/ 用于示例的源文件夹，dst/ 用于示例的目标文件夹，dst_copy/ 用于恢复 dst/。

// main.cpp
#include "FolderSync.hpp"

std::string src_path = R"(..\sample_folder\src)";
std::string dst_path = R"(..\sample_folder\dst)";

FolderSync folder_sync(src_path, dst_path);
folder_sync.findDiff();  // 查找 源文件夹 与 目标文件夹 的差异，并打印相关信息，不修改文件夹
folder_sync.update();    // 查找 源文件夹 与 目标文件夹 的差异，并更新文件夹
```



## 问题

- 如果只是文件夹的名字改了，则只修改文件夹名，不需要删除旧文件夹再复制新文件夹


