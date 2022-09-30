//
// Created by Chenjingzhi on 2022/9/29.
//

#ifndef FOLDERSYNC_HPP
#define FOLDERSYNC_HPP

#include <iostream>
#include <fstream>
#include <io.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <sys/stat.h>


class FolderSync {
public:
    FolderSync(std::string src_path, std::string dst_path)
            : m_src_path(std::move(src_path)),
              m_dst_path(std::move(dst_path)) {
        files_init();
    };

    ~FolderSync() = default;

    FolderSync(const FolderSync &) = delete;

    FolderSync &operator=(const FolderSync &) = delete;

    FolderSync(FolderSync &&) = default;

    FolderSync &operator=(FolderSync &&) = default;

private:
    std::string m_src_path;
    std::unordered_map<std::string, long> m_src_files;

    std::string m_dst_path;
    std::unordered_map<std::string, long> m_dst_files;

    std::unordered_map<std::string, long> m_new_files;
    std::unordered_map<std::string, long> m_old_files;

public:
    void update() {
        if (m_new_files.empty() && m_old_files.empty()) find_files_diff();

        // delete old files
        for (const auto &i : m_old_files) {
            remove((m_dst_path + i.first).c_str());
        }

        // create new files
        for (const auto &i : m_new_files) {
            std::string cmd;
            system(("copy \"" + m_src_path + i.first + "\" \"" + m_dst_path + i.first + "\"").c_str());
        }
    }

    void find_files_diff() {
        if (m_src_files.empty()) {
            std::cout << "Error: source files empty!" << std::endl;
            return;
        }

        m_old_files.insert(m_dst_files.begin(), m_dst_files.end());

        // get new files
        for (const auto &i : m_src_files) {
            auto iter = m_old_files.find(i.first);
            if (iter != m_old_files.end() && i.second == iter->second) {  // already exist
                m_old_files.erase(iter);
            } else {  // not exist
                m_new_files.insert(i);
            }
        }

        // print new and old files
        std::cout << "new files :" << std::endl;
        for (const auto &i : m_new_files) std::cout << i.first << ", " << i.second << std::endl;
        std::cout << "old files :" << std::endl;
        for (const auto &i : m_old_files) std::cout << i.first << ", " << i.second << std::endl;
    }

    void find_empty_sub_folder() {}  // TODO

private:
    void files_init() {
        if (m_src_path == m_dst_path) {
            std::cout << "Error: The source and destination addresses must be different!" << std::endl;
            return;
        }

        // get files list
        get_files(m_src_path, m_src_files, (int) m_src_path.length());
        get_files(m_dst_path, m_dst_files, (int) m_dst_path.length());
    }

    static
    void get_files(const std::string &path, std::unordered_map<std::string, long> &files, int root_path_length) {
        long file_handle;  // 文件句柄
        struct _finddata_t file_info{};  // 文件信息
        struct _stat stat_info{};  // 文件状态信息

        std::string p;
        if ((file_handle = _findfirst(p.assign(path).append("\\*").c_str(), &file_info)) != -1) {
            do {
                if ((file_info.attrib & _A_SUBDIR)) {  // 表示文件夹
                    if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
                        p.assign(path).append("\\").append(file_info.name);
                        get_files(p, files, root_path_length);  // 递归搜索
                    }
                } else {  // (file_info.attrib & _A_SUBDIR) == 0，表示文件
                    p.assign(path).append("\\").append(file_info.name);
                    _stat(p.c_str(), &stat_info);
                    files.emplace(p.substr(root_path_length, p.length() - root_path_length), stat_info.st_size);
                }
            } while (_findnext(file_handle, &file_info) == 0);  // 处理下一个，存在则返回 0，否则返回 -1
            _findclose(file_handle);
        }
    }
};


#endif //FOLDERSYNC_HPP
