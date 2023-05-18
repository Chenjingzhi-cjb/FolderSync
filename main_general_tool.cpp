#include <string>
#include "FolderSync.hpp"


using namespace std;


// Is used to build an easy-to-use command-line "general folder synchronization" tool.
// Run the following command from the command line:
// ./FolderSyncGeneralTool.exe "src_path" "dst_path1" "dst_path2" ...
int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Error: Please enter the path to the destination folder!" << endl;
        return -1;
    }

    string src_path = argv[1];

    vector<string> dst_paths;
    for (int i = 2; i < argc; i++) {
        dst_paths.emplace_back(argv[i]);
    }

    FolderSync folder_sync(src_path, dst_paths);

    folder_sync.findDiff();

    char flag;
    cout << "If you need to update, enter [y/n]:" << endl;
    cin >> flag;
    if (flag == 'y') folder_sync.update();

    return 0;
}

