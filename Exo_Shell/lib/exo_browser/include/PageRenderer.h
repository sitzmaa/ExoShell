#ifndef PAGERENDERER_H
#define PAGERENDERER_H

#include <string>
#include "WebManager.h"

class PageRenderer {
public:
    explicit PageRenderer(WebManager& manager);
    void renderPage(const std::string& content);
    void highlightLink(int linkIndex);
    void scrollPage(int lines);

private:
    WebManager& webManager;
};

#endif // PAGERENDERER_H
