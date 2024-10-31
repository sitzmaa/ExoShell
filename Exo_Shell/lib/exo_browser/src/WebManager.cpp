#include "../include/WebManager.h"
#include <iostream>  // For printing and debugging

WebManager::WebManager() {}

std::string WebManager::fetchPage(const std::string& url) {
    // Fetch the page content, add to history
    std::string htmlContent = "<html>Example HTML content with links</html>";

    // Push old URL and save new one as current
    backStack.push(currentURL);
    currentURL = url;
    //history.push_back(url);
    return htmlContent;
}

// forward and back through history
// REQUIRED call refresh() to render the new page
void WebManager::goBack() {
    // Logic to go back
    forwardStack.push(currentURL);
    currentURL = backStack.pop();
}

void WebManager::goForward() {
    // Logic to go forward
    backStack.push(currentURL);
    currentURL = forwardStack.pop()
}

// Refresh the page
std::string refresh(){
    // fetch content from currentURL
    std::string htmlContent = "refresh sucess";
    return htmlContent;
}



