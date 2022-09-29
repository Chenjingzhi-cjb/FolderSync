#include <string>
#include "FolderSync.hpp"

using namespace std;


int main() {
    string src_path = R"(C:\Users\12865\Desktop\encrypt_001)";
    string dst_path = R"(C:\Users\12865\Desktop\encrypt_002)";

    FolderSync folder_sync(src_path, dst_path);
    folder_sync.update();

    return 0;
}
