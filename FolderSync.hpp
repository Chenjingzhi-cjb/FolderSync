#ifndef FOLDERSYNC_HPP
#define FOLDERSYNC_HPP

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <io.h>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <windows.h>


class FolderObj {
public:
    explicit FolderObj(std::string path)
            : m_path(std::move(path)) {
        if (m_path.c_str()[m_path.length() - 1] != '\\') m_path.append("\\");  // FolderObj::m_path ��'\'��β
        auto _path = m_path.substr(0, m_path.length() - 1);
        auto name_pos = _path.find_last_of('\\');
        m_name = _path.substr(name_pos + 1, _path.length() - 2);
    }

    ~FolderObj() = default;

public:
    std::string getPath() const {
        return m_path;
    }

    std::string getName() const {
        return m_name;
    }

public:
    std::vector<FolderObj> m_sub_folders{};
    std::unordered_map<std::string, unsigned long> m_files{};

private:
    std::string m_path{};  // ��������'\'��β
    std::string m_name{};
};


class FolderSync {
public:
    FolderSync(std::string src_path, std::string dst_path)
            : m_src_path(std::move(src_path)),
              m_dst_paths({std::move(dst_path)}),
              m_src_folder(""),
              m_dst_folder("") {
        initSrcFolder();
    };

    FolderSync(std::string src_path, std::vector<std::string> dst_paths)
            : m_src_path(std::move(src_path)),
              m_dst_paths(std::move(dst_paths)),
              m_src_folder(""),
              m_dst_folder("") {
        initSrcFolder();
    }

    ~FolderSync() = default;

public:
    /**
     * @brief ���� Դ�ļ��� �� Ŀ���ļ��� �Ĳ��죬����ӡ�����Ϣ�����޸��ļ���
     *
     * @return None
     */
    void findDiff() {
        for (const auto &dst_path : m_dst_paths) {
            if (initDstFolder(dst_path)) {
                std::cout << "-------- dst path: " << dst_path << " --------" << std::endl;
                findFilesDiff(m_src_folder, m_dst_folder, false);
                std::cout << "---------------- check succeeded ----------------\n" << std::endl;
            }
        }

        std::cout << "All check completed!\n" << std::endl;
    }

    /**
     * @brief ���� Դ�ļ��� �� Ŀ���ļ��� �Ĳ��죬�������ļ���
     *
     * @return None
     */
    void update() {
        for (const auto &dst_path : m_dst_paths) {
            if (initDstFolder(dst_path)) {
                std::cout << "-------- dst path: " << dst_path << " --------" << std::endl;
                findFilesDiff(m_src_folder, m_dst_folder, true);
                std::cout << "---------------- update succeeded ----------------\n" << std::endl;
            }
        }

        std::cout << "All update completed!\n" << std::endl;
    }

private:
    /**
     * @brief ��ʼ�� Դ�ļ��ж���
     *
     * @return None
     */
    void initSrcFolder() {
        // ����û������Ŀ¼��ַ����'\'��β������'\'��β
        // FolderSync::m_path �� FolderObj::m_path ����'\'��β
        if (m_src_path.c_str()[m_src_path.length() - 1] != '\\') m_src_path.append("\\");

        m_src_folder = FolderObj(m_src_path);

        std::cout << "\n" << "-> src path: " << m_src_path << "\n" << std::endl;

        buildFolderTreeW(m_src_folder);
    }

    /**
     * @brief ��ʼ�� Ŀ���ļ��ж���
     *
     * @param dst_path Ŀ���ļ�·��
     * @return init status
     */
    bool initDstFolder(std::string dst_path) {
        if (dst_path.c_str()[dst_path.length() - 1] != '\\') dst_path.append("\\");

        if (m_src_path == dst_path) {
            std::cout << "initFolderSync() Error: The source and destination addresses must be different!" << std::endl;
            std::cout << "    error folder: " << dst_path << "\n" << std::endl;
            return false;
        }

        m_dst_folder = FolderObj(dst_path);

        buildFolderTreeW(m_dst_folder);

        return true;
    }

    /**
     * @brief �����νṹ�����ļ��ж��󣨿��ַ���
     *
     * @param folder �ļ��ж���
     * @return None
     */
    static void buildFolderTreeW(FolderObj &folder) {
        HANDLE file_handle;  // �ļ����
        WIN32_FIND_DATAW file_info;  // �ļ���Ϣ

        std::wstring folder_path = string2wstring(folder.getPath());
        std::wstring p;
        if ((file_handle = FindFirstFileW(p.assign(folder_path).append(L"*").c_str(), &file_info)) !=
            INVALID_HANDLE_VALUE) {
            do {
                if ((file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  // ��ʾ���ļ���
                    if (wcscmp(file_info.cFileName, L".") != 0 && wcscmp(file_info.cFileName, L"..") != 0) {
                        FolderObj sub_folder(
                                wstring2string(p.assign(folder_path).append(file_info.cFileName).append(L"\\")));
                        buildFolderTreeW(sub_folder);  // �ļ��еݹ�����
                        folder.m_sub_folders.emplace_back(std::move(sub_folder));
                    }
                } else {  // (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0����ʾ�ļ�
                    // �洢 <�ļ������ļ���С> ��ֵ��
                    folder.m_files.emplace(wstring2string(file_info.cFileName), file_info.nFileSizeLow);
                }
            } while (FindNextFileW(file_handle, &file_info) != 0);  // ������һ���������򷵻�ֵ��Ϊ 0
            FindClose(file_handle);
        }

        // �����ļ��ж����������
        std::sort(folder.m_sub_folders.begin(), folder.m_sub_folders.end(), [](FolderObj &a, FolderObj &b) {
            return a.getName() < b.getName();
        });
    }

    /**
     * @brief ���� �����ļ��ж����е� �ļ� �� ���ļ��� �Ĳ��죬���� �����ļ��� �� ͬ�����ļ��� ���еݹ����
     *
     * @param src_folder Դ�ļ��ж���
     * @param dst_folder Ŀ���ļ��ж���
     * @param is_operate �Ƿ�� �ļ�/�ļ��� ���в���
     * @return None
     */
    static void findFilesDiff(FolderObj &src_folder, FolderObj &dst_folder, bool is_operate) {
        // 1. ���Ҿ��ļ�
        auto _src_files = src_folder.m_files;
        for (auto &dst_file : dst_folder.m_files) {
            auto src_file = _src_files.find(dst_file.first);
            if (src_file != src_folder.m_files.end() && src_file->second == dst_file.second) {
                _src_files.erase(src_file);  // ȥ����ͬ�ļ�
                continue;
            }

            if (is_operate) {  // update()
                DeleteFileW(string2wstring(dst_folder.getPath() + dst_file.first).c_str());
                std::cout << "Deleted file: " << dst_folder.getPath() << dst_file.first << std::endl;
            } else {  // findDiff()
                std::cout << "Old file: " << dst_folder.getPath() << dst_file.first << std::endl;
            }
        }

        // 2. �������ļ�
        for (auto &i : _src_files) {
            if (is_operate) {  // update()
                CopyFileW(string2wstring(src_folder.getPath() + i.first).c_str(),
                          string2wstring(dst_folder.getPath() + i.first).c_str(), FALSE);
                std::cout << "Added file: " << dst_folder.getPath() << i.first << std::endl;
            } else {  // findDiff()
                std::cout << "New file: " << src_folder.getPath() << i.first << std::endl;
            }
        }
        _src_files.clear();

        // 3. ���ļ��д���
        // ʹ��˫����ָ�룬���� m_sub_folders �е�Ԫ���ǰ��� ���ļ����� ��С�������е��ص㣬
        // ����ֱ�ָ�� src �� dst �����ļ�������ͬ������� �����ļ��� �� �����ļ���;
        // �����ͬ����ݹ���ҡ�
        auto src_iter = src_folder.m_sub_folders.begin();
        auto dst_iter = dst_folder.m_sub_folders.begin();
        for (; src_iter != src_folder.m_sub_folders.end() || dst_iter != dst_folder.m_sub_folders.end();) {
            if (src_iter == src_folder.m_sub_folders.end()) {
                oldFolder(dst_folder, dst_iter, is_operate);
                dst_iter++;
            } else if (dst_iter == dst_folder.m_sub_folders.end()) {
                newFolder(src_folder, dst_folder, src_iter, is_operate);
                src_iter++;
            } else {  // src_iter != src_folder.m_sub_folders.end() && dst_iter != dst_folder.m_sub_folders.end()
                if (src_iter->getName() > dst_iter->getName()) {
                    oldFolder(dst_folder, dst_iter, is_operate);
                    dst_iter++;
                } else if (src_iter->getName() < dst_iter->getName()) {
                    newFolder(src_folder, dst_folder, src_iter, is_operate);
                    src_iter++;
                } else {  // src_iter->getName() == dst_iter->getName() �ݹ�
                    findFilesDiff(*src_iter, *dst_iter, is_operate);
                    src_iter++;
                    dst_iter++;
                }
            }
        }
    }

    /**
     * @brief ���ļ��и��Ĳ���
     *
     * @param src_folder Դ�ļ��ж���
     * @param dst_folder Ŀ���ļ��ж���
     * @param src_iter Դ�ļ����е� �����ļ��ж��� �ĵ�����
     * @param is_operate �Ƿ�� �ļ�/�ļ��� ���в���
     * @return None
     */
    inline static void
    newFolder(FolderObj &src_folder, FolderObj &dst_folder, std::vector<FolderObj>::iterator &src_iter,
              bool is_operate) {
        if (is_operate) {  // update()
            copyFolderW(string2wstring(src_folder.getPath() + src_iter->getName() + "\\"),
                        string2wstring(dst_folder.getPath() + src_iter->getName() + "\\"));
            std::cout << "Added folder: " << dst_folder.getPath() << src_iter->getName() << "\\" << std::endl;
        } else {  // findDiff()
            std::cout << "New folder: " << src_iter->getPath() << std::endl;
        }
    }

    /**
     * @brief ���ļ��и��Ĳ���
     *
     * @param dst_folder Ŀ���ļ��ж���
     * @param dst_iter Ŀ���ļ����е� �����ļ��ж��� �ĵ�����
     * @param is_operate �Ƿ�� �ļ�/�ļ��� ���в���
     * @return None
     */
    inline static void oldFolder(FolderObj &dst_folder, std::vector<FolderObj>::iterator &dst_iter, bool is_operate) {
        if (is_operate) {  // update()
            deleteFolderW(string2wstring(dst_folder.getPath() + dst_iter->getName() + "\\"));
            std::cout << "Deleted folder: " << dst_iter->getPath() << std::endl;
        } else {  // findDiff()
            std::cout << "Old folder: " << dst_iter->getPath() << std::endl;
        }
    }

    /**
     * @brief string ת wstring (gbk)
     *
     * @param str src string (gbk)
     * @return dst wstring (gbk)
     */
    static std::wstring string2wstring(const std::string &str) {
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, const_cast<wchar_t *>(wstr.data()), len);
        wstr.resize(wcslen(wstr.c_str()));
        return wstr;
    }

    /**
     * @brief wstring ת string (gbk)
     *
     * @param wstr src wstring (gbk)
     * @return dst string (gbk)
     */
    static std::string wstring2string(const std::wstring &wstr) {
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string str(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, const_cast<char *>(str.data()), len, nullptr, nullptr);
        str.resize(strlen(str.c_str()));
        return str;
    }

    static void copyFolderW(const std::wstring &src_folder_path, const std::wstring &dst_folder_path) {
        HANDLE file_handle;  // �ļ����
        WIN32_FIND_DATAW file_info;  // �ļ���Ϣ

        CreateDirectoryW((dst_folder_path).c_str(), nullptr);

        std::wstring p;
        if ((file_handle = FindFirstFileW(p.assign(src_folder_path).append(L"*").c_str(), &file_info)) !=
            INVALID_HANDLE_VALUE) {
            do {
                if ((file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  // ��ʾ���ļ���
                    if (wcscmp(file_info.cFileName, L".") != 0 && wcscmp(file_info.cFileName, L"..") != 0) {
                        copyFolderW(src_folder_path + file_info.cFileName + L"\\",
                                    dst_folder_path + file_info.cFileName + L"\\");
                    }
                } else {  // (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0����ʾ�ļ�
                    CopyFileW((src_folder_path + file_info.cFileName).c_str(),
                              (dst_folder_path + file_info.cFileName).c_str(), FALSE);
                }
            } while (FindNextFileW(file_handle, &file_info) != 0);  // ������һ���������򷵻�ֵ��Ϊ 0
            FindClose(file_handle);
        }
    }

    static void deleteFolderW(const std::wstring &folder_path) {
        HANDLE file_handle;  // �ļ����
        WIN32_FIND_DATAW file_info;  // �ļ���Ϣ

        std::wstring p;
        if ((file_handle = FindFirstFileW(p.assign(folder_path).append(L"*").c_str(), &file_info)) !=
            INVALID_HANDLE_VALUE) {
            do {
                if ((file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  // ��ʾ���ļ���
                    if (wcscmp(file_info.cFileName, L".") != 0 && wcscmp(file_info.cFileName, L"..") != 0) {
                        deleteFolderW(folder_path + file_info.cFileName + L"\\");
                    }
                } else {  // (file_info.attrib & _A_SUBDIR) == 0����ʾ�ļ�
                    DeleteFileW((folder_path + file_info.cFileName).c_str());
                }
            } while (FindNextFileW(file_handle, &file_info) != 0);  // ������һ���������򷵻�ֵ��Ϊ 0
            FindClose(file_handle);
        }

        RemoveDirectoryW((folder_path).c_str());
    }

private:
    std::string m_src_path;
    std::vector<std::string> m_dst_paths;

    FolderObj m_src_folder;
    FolderObj m_dst_folder;
};


#endif //FOLDERSYNC_HPP
