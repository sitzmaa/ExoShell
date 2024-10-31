#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <string>
#include <vector>
#include <stack>

class WebManager {
public:
    WebManager();
    std::string fetchPage(const std::string& url);
    void goBack();
    void goForward();
    std::string refresh();

private:
    std::string currentURL;
    std::stack<std::string> backStack;
    std::stack<std::string> forwardStack;
};

#endif // WEBMANAGER_H
