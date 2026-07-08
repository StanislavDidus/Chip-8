#pragma once
#include <string>
#include <vector>

struct logger
{
    void log(const std::string& text)
    {
        std::cout << text << std::endl;
        logs.push_back(text);
    }

    const std::vector<std::string>& get_logs()
    {
        return logs;
    }

    void clear()
    {
        logs.clear();
    }

    void copy()
    {
        log("Copy function is not working currently");
    }
private:
    std::vector<std::string> logs;
};
