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
    FolderObj() = default;

    explicit FolderObj(std::string path)
            : m_path(std::move(path)) {
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
    std::string m_path{};  // 该属性以'\'结尾
    std::string m_name{};
};


class FolderSync {
public:
    FolderSync(std::string src_path, std::string dst_path, bool W = false)
            : m_src_path(std::move(src_path)),
              m_W(W) {
        m_dst_paths.emplace_back(std::move(dst_path));
        initSrcFolder();
    };

    FolderSync(std::string src_path, std::vector<std::string> dst_paths, bool W = false)
            : m_src_path(std::move(src_path)),
              m_dst_paths(std::move(dst_paths)),
              m_W(W) {
        initSrcFolder();
    }

    ~FolderSync() = default;

public:
    /**
     * @brief 查找 源文件夹 与 目标文件夹 的差异，并打印相关信息，不修改文件夹
     *
     * @return None
     */
    void findDiff() {
        for (const auto &dst_path : m_dst_paths) {
            initDstFolder(dst_path);
            std::cout << "-------- " << dst_path << " --------" << std::endl;
            findFilesDiff(m_src_folder, m_dst_folder, false);
            std::cout << "--------------------------------" << std::endl;
        }
    }

    /**
     * @brief 查找 源文件夹 与 目标文件夹 的差异，并更新文件夹
     *
     * @return None
     */
    void update() {
        for (const auto &dst_path : m_dst_paths) {
            initDstFolder(dst_path);
            std::cout << "-------- " << dst_path << " --------" << std::endl;
            findFilesDiff(m_src_folder, m_dst_folder, true);
            std::cout << "--------------------------------" << std::endl;
        }
    }

private:
    /**
     * @brief 初始化 源文件夹对象
     *
     * @return None
     */
    void initSrcFolder() {
        // 如果用户输入的目录地址不以'\'结尾，增加'\'结尾，FolderObj::m_path 均以'\'结尾
        if (m_src_path.c_str()[-1] != '\\') m_src_path.append("\\");

        m_src_folder = FolderObj(m_src_path);

        std::cout << m_src_path << std::endl;

        if (m_W) {
            buildFolderTreeW(m_src_folder);
        } else {  // !W
            buildFolderTree(m_src_folder);
        }
    }

    /**
     * @brief 初始化 目标文件夹对象
     *
     * @param dst_path 目标文件路径
     * @return None
     */
    void initDstFolder(std::string dst_path) {
        if (dst_path.c_str()[-1] != '\\') dst_path.append("\\");

        if (m_src_path == dst_path) {
            std::cout << "initFolderSync() Error: The source and destination addresses must be different!" << std::endl;
            std::cout << "    error folder: " << dst_path << std::endl;
        }

        m_dst_folder = FolderObj(dst_path);

        if (m_W) {
            buildFolderTreeW(m_dst_folder);
        } else {  // !W
            buildFolderTree(m_dst_folder);
        }
    }

    /**
     * @brief 以树形结构构建文件夹对象
     *
     * @param folder 文件夹对象
     * @return None
     */
    static void buildFolderTree(FolderObj &folder) {
        long file_handle;  // 文件句柄
        struct _finddata_t file_info{};  // 文件信息

        std::string folder_path = folder.getPath();
        std::string p;
        if ((file_handle = _findfirst(p.assign(folder_path).append("*").c_str(), &file_info)) != -1) {
            do {
                if ((file_info.attrib & _A_SUBDIR)) {  // 表示子文件夹
                    if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
                        FolderObj sub_folder(p.assign(folder_path).append(file_info.name).append("\\"));
                        buildFolderTree(sub_folder);  // 文件夹递归搜索
                        folder.m_sub_folders.emplace_back(std::move(sub_folder));
                    }
                } else {  // (file_info.attrib & _A_SUBDIR) == 0，表示文件
                    // 存储 <文件名，文件大小> 键值对
                    folder.m_files.emplace(file_info.name, file_info.size);
                }
            } while (_findnext(file_handle, &file_info) == 0);  // 处理下一个，存在则返回 0，否则返回 -1
            _findclose(file_handle);
        }

        // 对子文件夹对象进行排序
        std::sort(folder.m_sub_folders.begin(), folder.m_sub_folders.end(), [](FolderObj &a, FolderObj &b) {
            return a.getName() < b.getName();
        });
    }

    /**
     * @brief 以树形结构构建文件夹对象（宽字符）
     *
     * @param folder 文件夹对象
     * @return None
     */
    static void buildFolderTreeW(FolderObj &folder) {
        HANDLE file_handle;  // 文件句柄
        WIN32_FIND_DATAW file_info;  // 文件信息

        std::wstring folder_path = string2wstring(folder.getPath());
        std::wstring p;
        if ((file_handle = FindFirstFileW(p.assign(folder_path).append(L"*").c_str(), &file_info)) !=
            INVALID_HANDLE_VALUE) {
            do {
                if ((file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  // 表示子文件夹
                    if (wcscmp(file_info.cFileName, L".") != 0 && wcscmp(file_info.cFileName, L"..") != 0) {
                        FolderObj sub_folder(
                                wstring2string(p.assign(folder_path).append(file_info.cFileName).append(L"\\")));
                        buildFolderTreeW(sub_folder);  // 文件夹递归搜索
                        folder.m_sub_folders.emplace_back(std::move(sub_folder));
                    }
                } else {  // (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0，表示文件
                    // 存储 <文件名，文件大小> 键值对
                    folder.m_files.emplace(wstring2string(file_info.cFileName), file_info.nFileSizeLow);
                }
            } while (FindNextFileW(file_handle, &file_info) != 0);  // 处理下一个，存在则返回值不为 0
            FindClose(file_handle);
        }

        // 对子文件夹对象进行排序
        std::sort(folder.m_sub_folders.begin(), folder.m_sub_folders.end(), [](FolderObj &a, FolderObj &b) {
            return a.getName() < b.getName();
        });
    }

    /**
     * @brief 查找 两个文件夹对象中的 文件 和 子文件夹 的差异，并对 两个文件夹 的 同名子文件夹 进行递归查找
     *
     * 调用 "del" 删除文件，调用 "copy" 复制文件
     *
     * @param src_folder 源文件夹对象
     * @param dst_folder 目标文件夹对象
     * @param is_operate 是否对 文件/文件夹 进行操作
     * @return None
     */
    static void findFilesDiff(FolderObj &src_folder, FolderObj &dst_folder, bool is_operate) {
        // 1. 查找旧文件
        auto _src_files = src_folder.m_files;
        for (auto &dst_file : dst_folder.m_files) {
            auto src_file = _src_files.find(dst_file.first);
            if (src_file != src_folder.m_files.end() && src_file->second == dst_file.second) {
                _src_files.erase(src_file);  // 去除相同文件
                continue;
            }

            if (is_operate) {  // update()
                system(("del \"" + dst_folder.getPath() + dst_file.first + "\"").c_str());
            } else {  // findDiff()
                std::cout << "Old files: " << dst_folder.getPath() << dst_file.first << std::endl;
            }
        }

        // 2. 查找新文件
        for (auto &i : _src_files) {
            if (is_operate) {  // update()
                system(("copy \"" + src_folder.getPath() + i.first + "\" \"" + dst_folder.getPath() + i.first +
                        "\"").c_str());
            } else {  // findDiff()
                std::cout << "New files: " << src_folder.getPath() << i.first << std::endl;
            }
        }
        _src_files.clear();

        // 3. 子文件夹处理
        // 使用双队列指针，利用 m_sub_folders 中的元素是按照 子文件夹名 从小到大排列的特点，
        // 如果分别指向 src 和 dst 的子文件夹名不同，则必有 新子文件夹 或 旧子文件夹;
        // 如果相同，则递归查找。
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
                } else {  // src_iter->getName() == dst_iter->getName() 递归
                    findFilesDiff(*src_iter, *dst_iter, is_operate);
                    src_iter++;
                    dst_iter++;
                }
            }
        }
    }

    /**
     * @brief 新文件夹更改操作
     *
     * 调用 "md" 创建文件夹，调用 "xcopy /S /E /Y" 复制文件夹内容
     *
     * @param src_folder 源文件夹对象
     * @param dst_folder 目标文件夹对象
     * @param src_iter 源文件夹中的 新子文件夹对象 的迭代器
     * @param is_operate 是否对 文件/文件夹 进行操作
     * @return None
     */
    inline static void
    newFolder(FolderObj &src_folder, FolderObj &dst_folder, std::vector<FolderObj>::iterator &src_iter,
              bool is_operate) {
        if (is_operate) {  // update()
            system(("md \"" + dst_folder.getPath() + src_iter->getName() + "\"").c_str());
            system(("xcopy \"" + src_folder.getPath() + src_iter->getName() + "\" \"" + dst_folder.getPath() +
                    src_iter->getName() + "\"" + "/S /E /Y").c_str());
        } else {  // findDiff()
            std::cout << "New folder: " << src_iter->getPath() << std::endl;
        }
    }

    /**
     * @brief 旧文件夹更改操作
     *
     * 调用 "re /s /q" 删除文件夹
     *
     * @param dst_folder 目标文件夹对象
     * @param dst_iter 目标文件夹中的 旧子文件夹对象 的迭代器
     * @param is_operate 是否对 文件/文件夹 进行操作
     * @return None
     */
    inline static void oldFolder(FolderObj &dst_folder, std::vector<FolderObj>::iterator &dst_iter, bool is_operate) {
        if (is_operate) {  // update()
            system(("rd /s /q \"" + dst_folder.getPath() + dst_iter->getName() + "\"").c_str());
        } else {  // findDiff()
            std::cout << "Old folder: " << dst_iter->getPath() << std::endl;
        }
    }

    /**
     * @brief string 转 wstring (gbk)
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
     * @brief wstring 转 string (gbk)
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

private:
    std::string m_src_path;
    std::vector<std::string> m_dst_paths;

    FolderObj m_src_folder;
    FolderObj m_dst_folder;

    bool m_W;
};


#endif //FOLDERSYNC_HPP
