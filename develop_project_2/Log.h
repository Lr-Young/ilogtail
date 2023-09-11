#include<string>
#include<vector>

class Content {
public:
    std::string key;
    std::string value;
};

class Log {
public:
    std::vector<Content> contents;
};