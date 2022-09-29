//
// Created by Chenjingzhi on 2022/9/29.
//

// 注：该程序只能以文件为整体进行更新，如文件的新增和删减，文件内部的更新无法同步 TODO: 增加此功能

#ifndef FOLDERSYNC_HPP
#define FOLDERSYNC_HPP

#include <iostream>
#include <fstream>
#include <io.h>
#include <string>
#include <unordered_set>
#include <utility>


class FolderSync {
public:
    FolderSync(std::string src_path, std::string dst_path)
            : m_src_path(std::move(src_path)),
              m_dst_path(std::move(dst_path)) {
        m_src_path_length = (int) m_src_path.length();
        m_dst_path_length = (int) m_dst_path.length();
    };

    ~FolderSync() = default;

    FolderSync(const FolderSync &) = delete;

    FolderSync &operator=(const FolderSync &) = delete;

    FolderSync(FolderSync &&) = default;

    FolderSync &operator=(FolderSync &&) = default;

private:
    std::string m_src_path;
    int m_src_path_length;
    std::unordered_set<std::string> m_src_files;

    std::string m_dst_path;
    int m_dst_path_length;
    std::unordered_set<std::string> m_dst_files;

public:
    void update() {
        // get files list
        get_files(m_src_path, m_src_files, m_src_path_length);
        get_files(m_dst_path, m_dst_files, m_dst_path_length);

        // create new files
        for (const auto &i : m_src_files) {
            auto iter = m_dst_files.find(i);

            if (iter == m_dst_files.end()) {
                copy_files(m_src_path + i, m_dst_path + i);
            } else {  // iter != m_dst_files.end()
                m_dst_files.erase(iter);
            }
        }

        // delete old files
        for (const auto &i : m_dst_files) {
            remove((m_dst_path + i).c_str());
        }
    }

private:
    static void get_files(const std::string &path, std::unordered_set<std::string> &files, int base_path_length) {
        long long int file_handle;  // 文件句柄
        struct _finddata_t file_info{};  // 文件信息

        std::string p;
        if ((file_handle = _findfirst(p.assign(path).append("\\*").c_str(), &file_info)) != -1) {
            do {
                if ((file_info.attrib & _A_SUBDIR)) {  // 表示文件夹
                    if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0) {
                        get_files(p.assign(path).append("\\").append(file_info.name), files, base_path_length);  //递归搜索
                    }
                } else {  // (file_info.attrib & _A_SUBDIR) == 0，表示文件
                    p.assign(path).append("\\").append(file_info.name);
                    files.emplace(p.substr(base_path_length, p.length() - base_path_length));
                }
            } while (_findnext(file_handle, &file_info) == 0);  // 处理下一个，存在则返回 0，否则返回 -1
            _findclose(file_handle);
        }
    }

    static void copy_files(const std::string &src_path, const std::string &dst_path) {
        std::ifstream in;
        in.open(src_path);
        if (!in) {
            std::cout << "Error: open src file < " << src_path << " > failed!" << std::endl;
            return;
        }

        std::ofstream out;
        out.open(dst_path);
        if (!out) {
            std::cout << "Error: create dst file < " << dst_path << " > failed!" << std::endl;
            in.close();
            return;
        }

        out << in.rdbuf();

        out.close();
        in.close();
    }
};


#endif //FOLDERSYNC_HPP
