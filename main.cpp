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

#else  // ����·��

    // ǧ��ע�ⲻҪд���ˣ�����dst path �е��ļ����ܻᱻɾ��������
    string src_path = R"(..\sample_folder\Դ)";
    string dst_path = R"(..\sample_folder\Ŀ��)";
    FolderSync folder_sync(src_path, dst_path);

#endif

    folder_sync.findDiff();
//    folder_sync.update();

    return 0;
}

