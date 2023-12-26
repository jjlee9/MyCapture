// GitCommits.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <vector>
#include <set>
#include <utility>
#include <fstream>
#include <regex>
#include <iostream>
#include <algorithm>
#include <iomanip>

int main()
{
    using length_t = unsigned int;
    using rename_vector = std::vector<std::pair<std::string, std::string>>;
    using rename_length = std::pair<length_t, length_t>;
    using dir_name = std::string;
    using file_name = std::string;
    using modify_vector = std::vector<std::pair<dir_name, file_name>>;
    using modify_length = length_t;
    using delete_vector = std::vector<file_name>;
    using delete_length = length_t;
    using untracked_vector = std::vector<std::pair<dir_name, file_name>>;
    using untracked_length = length_t;

    std::ifstream input(R"(E:\Doc\test.txt)", std::ifstream::in);
    std::regex renamed(R"(^\s+renamed:\s*(\S+)\s*->\s*(\S+)\s*$)");
    std::regex modified(R"(^\s+modified:\s*(\S+)\s*$)");
    std::regex new_file(R"(^\s+new file:\s*(\S+)\s*$)");
    std::regex deleted(R"(^\s+deleted:\s*(\S+)\s*$)");
    std::regex untracked(R"(^Untracked files:\s*$)");
    std::regex untracked_files(R"(^\s+(\S+)\s*$)");
    std::regex others(R"(^\S+)");

    rename_vector rename_v;
    rename_length rename_l = { 0, 0 };
    modify_vector modify_v;
    modify_length modify_l = 0;
    delete_vector delete_v;
    delete_length delete_l = 0;
    bool untracked_start = false;
    untracked_vector untracked_v;
    untracked_length untracked_l = 0;

    for (std::string line; std::getline(input, line);) {
        std::smatch match;
        if (untracked_start) {
            if (std::regex_search(line, match, others)) {
                std::cout << "others " << line << std::endl;
                untracked_start = false;
                continue;
            }

            if (std::regex_search(line, match, untracked_files)) {
                auto str = match.str(1);
                std::replace(str.begin(), str.end(), '/', '\\');

                auto found = str.find_last_of(R"(\)");
                if (found > untracked_l) {
                    untracked_l = static_cast<length_t>(found);
                }

                untracked_v.emplace_back(str.substr(0, found), str.substr(found + 1));
                std::cout << "untracked " << line << std::endl;
            }
        }

        if (std::regex_search(line, match, renamed)) {
            auto oldStr = match.str(1);
            std::replace(oldStr.begin(), oldStr.end(), '/', '\\');
            if (oldStr.size() > rename_l.first) {
                rename_l.first = static_cast<length_t>(oldStr.size());
            }

            auto newStr = match.str(2);
            std::replace(newStr.begin(), newStr.end(), '/', '\\');
            if (newStr.size() > rename_l.second) {
                rename_l.second = static_cast<length_t>(newStr.size());
            }

            rename_v.emplace_back(oldStr, newStr);
            std::cout << "rename from " << oldStr << " to "<< newStr << std::endl;
        } else if (std::regex_search(line, match, modified)) {
            auto str = match.str(1);
            std::replace(str.begin(), str.end(), '/', '\\');

            auto found = str.find_last_of(R"(\)");
            if (found != std::string::npos)
            {
                if (found > modify_l) {
                    modify_l = static_cast<length_t>(found);
                }

                modify_v.emplace_back(str.substr(0, found), str.substr(found + 1));
            }
            else
            {
                modify_l = static_cast<length_t>(0);
                modify_v.emplace_back(std::string(""), str);
            }
            std::cout << "modified " << str << std::endl;
        }
        else if (std::regex_search(line, match, new_file)) {
            auto str = match.str(1);
            std::replace(str.begin(), str.end(), '/', '\\');

            auto found = str.find_last_of(R"(\)");
            if (found > modify_l) {
                modify_l = static_cast<length_t>(found);
            }

            modify_v.emplace_back(str.substr(0, found), str.substr(found + 1));
            std::cout << "new file " << str << std::endl;
        }
        else if (std::regex_search(line, match, deleted)) {
            auto str = match.str(1);
            std::replace(str.begin(), str.end(), '/', '\\');
            delete_v.emplace_back(str);
            std::cout << "deleted " << str << std::endl;
        } else if (std::regex_search(line, match, untracked)) {
            untracked_start = true;
            std::cout << "untracked " << line << std::endl;
        } else {
            std::cout << "unmatched " << line << std::endl;
        }
    }
    std::cout << std::endl << std::endl;

    for (const auto& e : rename_v) {
        std::cout << "git mv " << std::left << std::setw(rename_l.first) << e.first << " " << e.second << std::endl;
    }
    std::cout << std::endl << std::endl;

//  std::cout << R"(set src_dir=g:\m1\src)" << std::endl;
    std::cout << R"(set src_dir=g:\sp1)" << std::endl;
//  std::cout << R"(set dst_dir=g:\o6\src)" << std::endl;
    std::cout << R"(set dst_dir=h:\sp1)" << std::endl;
//  std::cout << R"(set dst_dir=C:\Users\v-jialee\OneDrive - Microsoft\multiple_join_meeting_2\src)" << std::endl;
//  std::cout << R"(set dst_dir=C:\Users\v-jialee\OneDrive - Microsoft\productivity_1\pssrc)" << std::endl;
//  std::cout << R"(set dst_dir=C:\Users\v-jialee\OneDrive - Microsoft\multiple_join_meeting_2\atsrc)" << std::endl;
    std::cout << R"(c:)" << std::endl;
    std::cout << R"(subst z: /d)" << std::endl;
    std::cout << R"(md "%dst_dir%")" << std::endl;
    std::cout << R"(subst z: "%dst_dir%")" << std::endl;
    std::cout << std::endl;

    for (const auto& e : modify_v) {
        std::cout << R"(robocopy %src_dir%\)" << std::left << std::setw(modify_l) << e.first <<
            R"( z:\)" << std::left << std::setw(modify_l) << e.first << " " << e.second << std::endl;
    }
    std::cout << std::endl;

    for (const auto& e : delete_v) {
        std::cout << R"(git rm %src_dir%\)" << std::left << std::setw(modify_l) << e << std::endl;
    }
    std::cout << std::endl;

    for (const auto& e : untracked_v) {
        if (e.second.empty()) {
            std::cout << R"(robocopy %src_dir%\)" << std::left << std::setw(untracked_l) << e.first <<
                R"( z:\)" << std::left << std::setw(untracked_l) << e.first << " " << "/e" << std::endl;
        } else {
            std::cout << R"(robocopy %src_dir%\)" << std::left << std::setw(untracked_l) << e.first <<
                R"( z:\)" << std::left << std::setw(untracked_l) << e.first << " " << e.second << std::endl;
        }
    }

    // cleanup task
    std::cout << std::endl;
    std::cout << R"(z:)" << std::endl;
    std::set<dir_name> clean_up_dirs;

    for (const auto& e : modify_v) {
        bool already_in = false;
        for (const auto& m: clean_up_dirs) {
            if (e.first.starts_with(m)) {
                already_in = true;
            }
        }
        if (!already_in)
        {
            clean_up_dirs.insert(e.first);
        }
    }

    for (const auto& e: untracked_v) {
        bool already_in = false;
        for (const auto& m : clean_up_dirs) {
            if (e.first.starts_with(m)) {
                already_in = true;
            }
        }
        if (!already_in)
        {
            clean_up_dirs.insert(e.first);
        }
    }

    std::cout << R"(del E:\Doc\xx.txt)" << std::endl;

    for (const auto& e: clean_up_dirs) {
        std::cout << R"(cd z:\)" << e << std::endl;
        std::cout << R"(del /s /q buildfre*)" << std::endl;
        std::cout << R"(del /s /q buildchk*)" << std::endl;
        std::cout << R"(del /s /q SourcesCop*)" << std::endl;
        std::cout << R"(dir /ad /s /b >> E:\Doc\xx.txt)" << std::endl;
    }

    std::cout << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
