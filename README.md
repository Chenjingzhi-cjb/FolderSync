# FolderSync：文件夹同步

本项目的功能是使 “一个或一组目标文件夹” 与 “一个源文件夹” 保持内容的一致。



## 示例

```cpp
// sample_folder 文件夹下有三个文件夹，
// 其中 src/ 用于示例的源文件夹，dst/ 用于示例的目标文件夹，dst_copy/ 用于恢复 dst/。

// main.cpp
#include "FolderSync.hpp"

// 千万注意不要写反了！！！dst path 中的文件可能会被删除！！！
std::string src_path = R"(..\sample_folder\src)";
std::string dst_path = R"(..\sample_folder\dst)";

FolderSync folder_sync(src_path, dst_path);
folder_sync.findDiff();  // 查找 源文件夹 与 目标文件夹 的差异，并打印相关信息，不修改文件夹
folder_sync.update();    // 查找 源文件夹 与 目标文件夹 的差异，并更新文件夹
```

```cpp
// 中文路径测试，在 windows 系统（gbk）下已验证

// sample_folder 文件夹下有三个文件夹，
// 其中 源/ 用于示例的源文件夹，目标/ 用于示例的目标文件夹，目标_副本/ 用于恢复 目标/。

// ...

// 千万注意不要写反了！！！dst path 中的文件可能会被删除！！！
string src_path = R"(..\sample_folder\源)";
string dst_path = R"(..\sample_folder\目标)";

// ...
```



## 构建

本项目提供了以下三种构建（在 CMakeLists 中进行选择）：

- project_demo：示例代码，用于开发者的再开发及测试
- build_general_tool：通用文件夹同步工具，输入源文件夹目录和目标文件夹目录，进行同步操作
- build_this_tool：本文件夹同步工具，源文件夹为该工具所在的文件夹目录，只需输入目标文件夹目录



## 问题

- 如果只是文件夹的名字改了，则只修改文件夹名，不需要删除旧文件夹再复制新文件夹

