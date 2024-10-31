#include "../include/PageRenderer.h"
#include <ncurses.h>  // Ncurses library for terminal control

PageRenderer::PageRenderer(WebManager& manager) : webManager(manager) {}

void PageRenderer::renderPage(const std::string& content) {
    initscr();
    printw("Rendering Page:\n%s", content.c_str());
    refresh();
}

void PageRenderer::highlightLink(int linkIndex) {
    // Code to highlight selected link
}

void PageRenderer::scrollPage(int lines) {
    // Code to scroll content
}
