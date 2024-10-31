#include "include/WebManager.h"
#include "include/PageRenderer.h"
#include "include/HTMLParser.h"
#include "include/UserInterface.h"
#include <ncurses.h>  // For terminal input handling

int main() {
    WebManager webManager;
    std::string initialPage = webManager.fetchPage("http://startpage.com");

    HTMLParser parser(initialPage);
    std::vector<std::string> links = parser.getLinks();

    PageRenderer renderer(webManager);
    renderer.renderPage(initialPage);

    UserInterface ui(renderer);

    char input;
    while ((input = getch()) != 'q') {
        ui.processInput(input);
    }
    
    endwin();  // End ncurses mode
    return 0;
}
