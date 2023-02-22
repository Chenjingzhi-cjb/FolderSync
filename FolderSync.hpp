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

    FolderObj(const FolderObj &) = default;

    FolderObj &operator=(const FolderObj &) = default;

    FolderObj(FolderObj &&) = default;

    FolderObj &operator=(FolderObj &&) = default;

public:
    std::string getPath() {
        return m_path;
    }

    std::string getName() {
        return m_name;
    }

public:
    std::vector<FolderObj> m_sub_folders{};
    std::unordered_map<std::string, unsigned long> m_files{};

private:
    std::string m_path{};
    std::string m_name{};
};


class FolderSync {
public:
    FolderSync(const std::string &src_path, const std::string &dst_path) {
        initFolderSync(src_path, dst_path);
    };

    ~FolderSync() = default;

    FolderSync(const FolderSync &) = delete;

    FolderSync &operator=(const FolderSync &) = delete;

    FolderSync(FolderSync &&) = default;

    FolderSync &operator=(FolderSync &&) = default;

public:
    void findDiff() {
        findFilesDiff(m_src_folder, m_dst_folder, false);
    }

    void update() {
        findFilesDiff(m_src_folder, m_dst_folder, true);
    }

private:
    void initFolderSync(std::string src_path, std::string dst_path) {
        if (src_path == dst_path) {
            throw std::invalid_argument("initFolderSync(): The source and destination addresses must be different!");
        }

        if (src_path.c_str()[-1] != '\\') src_path.append("\\");
        if (dst_path.c_str()[-1] != '\\') dst_path.append("\\");

        m_src_folder = FolderObj(src_path);
        m_dst_folder = FolderObj(dst_path);

        buildFolderTree(m_src_folder);
        buildFolderTree(m_dst_folder);
    }

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
                    // 存储 <文件名，修改时间> 键值对
                    folder.m_files.emplace(file_info.name, file_info.size);
                }
            } while (_findnext(file_handle, &file_info) == 0);  // 处理下一个，存在则返回 0，否则返回 -1
            _findclose(file_handle);
        }
    }

    static void findFilesDiff(FolderObj &src_folder, FolderObj &dst_folder, bool is_operate) {
        auto _src_files = src_folder.m_files;

        // 1. 找出旧文件
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

        // 2. 找出新文件
        for (auto &i : _src_files) {
            if (is_operate) {  // update()
                system(("copy \"" + src_folder.getPath() + i.first + "\" \"" + dst_folder.getPath() + i.first + "\"").c_str());
            } else {  // findDiff()
                std::cout << "New files: " << src_folder.getPath() << i.first << std::endl;
            }
        }
        _src_files.clear();

        // 递归
        auto src_iter = src_folder.m_sub_folders.begin();
        auto dst_iter = dst_folder.m_sub_folders.begin();
        for (; src_iter != src_folder.m_sub_folders.end() && dst_iter != dst_folder.m_sub_folders.end(); ) {
            if (src_iter->getName() < dst_iter->getName()) {
                if (is_operate) {  // update()
                    system(("md \"" + dst_folder.getPath() + src_iter->getName() + "\"").c_str());
                    system(("copy \"" + src_folder.getPath() + src_iter->getName() + "\" \"" + dst_folder.getPath() + src_iter->getName() + "\"").c_str());
                } else {  // findDiff()
                    std::cout << "New folder: " << src_iter->getPath() << std::endl;
                }
                src_iter++;
            } else if (src_iter->getName() > dst_iter->getName()) {
                if (is_operate) {  // update()
                    system(("rd /s /q \"" + dst_folder.getPath() + dst_iter->getName() + "\"").c_str());
                } else {  // findDiff()
                    std::cout << "Old folder: " << dst_iter->getPath() << std::endl;
                }
                dst_iter++;
            } else {  // src_iter->getName() == dst_iter->getName()
                findFilesDiff(*src_iter, *dst_iter, is_operate);
                src_iter++;
                dst_iter++;
            }
        }
    }

private:
    FolderObj m_src_folder;
    FolderObj m_dst_folder;
};


#endif //FOLDERSYNC_HPP
