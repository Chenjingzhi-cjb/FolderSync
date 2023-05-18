#include <string>
#include "FolderSync.hpp"


using namespace std;


int main() {
    // TODO: GUI

#if 1

    // 千万注意不要写反了！！！dst path 中的文件可能会被删除！！！
    string src_path = R"(..\sample_folder\src)";
    string dst_path = R"(..\sample_folder\dst)";
    FolderSync folder_sync(src_path, dst_path);

    // 注意先创建 dst1\ 和 dst2\ 文件夹
//    string src_path = R"(..\sample_folder\src)";
//    string dst_path1 = R"(..\sample_folder\dst1)";
//    string dst_path2 = R"(..\sample_folder\dst2)";
//    vector<string> dst_paths = { dst_path1, dst_path2 };
//    FolderSync folder_sync(src_path, dst_paths);

#else  // 中文路径

    // 千万注意不要写反了！！！dst path 中的文件可能会被删除！！！
    string src_path = R"(..\sample_folder\源)";
    string dst_path = R"(..\sample_folder\目标)";
    FolderSync folder_sync(src_path, dst_path);

#endif

    folder_sync.findDiff();
//    folder_sync.update();

    return 0;
}

