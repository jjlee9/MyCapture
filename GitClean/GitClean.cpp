// GitClean.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <set>
#include <fstream>
#include <regex>
#include <iostream>

int main()
{
    using dir_name = std::string;
    using delete_dir_set = std::set<dir_name>;

    std::ifstream dirs(R"(C:\Users\v-jialee\Documents\xx.txt)", std::ifstream::in);
    std::regex vs(R"(^\s*(.*\.vs)\s*$)");
    std::regex objfre(R"(^\s*(.*\objfre)\s*$)");
    std::regex objchk(R"(^\s*(.*\objchk)\s*$)");

    delete_dir_set delete_dir_s;

    for (std::string line; std::getline(dirs, line);) {
        std::smatch match;
        if (std::regex_search(line, match, vs)) {
            delete_dir_s.insert(match.str(1));
        } else if (std::regex_search(line, match, objfre)) {
            delete_dir_s.insert(match.str(1));
        } else if (std::regex_search(line, match, objchk)) {
            delete_dir_s.insert(match.str(1));
        } else {
        }
    }

    for (const auto& dir : delete_dir_s) {
        std::cout << R"(rd /s /q )" << dir << std::endl;
    }
}
