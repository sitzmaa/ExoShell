#include "../include/HTMLParser.h"

HTMLParser::HTMLParser(const std::string& html) : htmlContent(html) {}

std::vector<std::string> HTMLParser::getLinks() {
    return {"http://example.com", "http://example.org"};
}
