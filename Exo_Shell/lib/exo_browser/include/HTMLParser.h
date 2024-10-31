#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <string>
#include <vector>

class HTMLParser {
public:
    explicit HTMLParser(const std::string& html);
    std::vector<std::string> getLinks();

private:
    std::string htmlContent;
};

#endif // HTMLPARSER_H
