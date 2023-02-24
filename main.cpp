#include <string>
#include "FolderSync.hpp"

using namespace std;


int main() {
    // TODO: GUI

    // 千万注意不要写反了！！！dst path 中的文件可能会被删除！！！
    string src_path = R"(..\sample_folder\src)";
    string dst_path = R"(..\sample_folder\dst)";

    FolderSync folder_sync(src_path, dst_path);
    folder_sync.update();

    return 0;
}
