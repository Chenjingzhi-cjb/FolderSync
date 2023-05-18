#include <string>
#include "FolderSync.hpp"


using namespace std;


// Is used to build an easy-to-use command-line "this folder synchronization" tool.
// Copy "FolderSyncThisTool.exe" and "libstdc++-6.dll" from the /bin to your source folder
// and run the following command from the command line: ./FolderSyncThisTool.exe "dst_path1" "dst_path2" ...
int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Error: Please enter the path to the destination folder!" << endl;
        return -1;
    }

    vector<string> dst_paths;
    for (int i = 1; i < argc; i++) {
        dst_paths.emplace_back(argv[i]);
    }

    FolderSync folder_sync(".", dst_paths);

    folder_sync.findDiff();

    char flag;
    cout << "If you need to update, enter [y/n]:" << endl;
    cin >> flag;
    if (flag == 'y') folder_sync.update();

    return 0;
}

