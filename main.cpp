#include <string>
#include "FolderSync.hpp"


using namespace std;


int main() {
    // TODO: GUI

#if 1

    // ǧ��ע�ⲻҪд���ˣ�����dst path �е��ļ����ܻᱻɾ��������
    string src_path = R"(..\sample_folder\src)";
    string dst_path = R"(..\sample_folder\dst)";
    FolderSync folder_sync(src_path, dst_path);

    // ע���ȴ��� dst1\ �� dst2\ �ļ���
//    string src_path = R"(..\sample_folder\src)";
//    string dst_path1 = R"(..\sample_folder\dst1)";
//    string dst_path2 = R"(..\sample_folder\dst2)";
//    vector<string> dst_paths = { dst_path1, dst_path2 };
//    FolderSync folder_sync(src_path, dst_paths);

#else  // ֧������·��

    // ǧ��ע�ⲻҪд���ˣ�����dst path �е��ļ����ܻᱻɾ��������
    string src_path = R"(..\sample_folder\Դ)";
    string dst_path = R"(..\sample_folder\Ŀ��)";
    FolderSync folder_sync(src_path, dst_path, true);  // W - true����ʾʹ�ÿ��ַ�

#endif

    folder_sync.findDiff();
//    folder_sync.update();

    return 0;
}


/*
// Is used to build an easy-to-use command-line folder synchronization tool.
// Copy the files from the /bin to your source folder and run the following command from the command line:
// ./FolderSync.exe "dst_path1" "dst_path2" ...
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
*/
