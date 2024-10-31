#include "../include/UserInterface.h"
#include <ncurses.h>

UserInterface::UserInterface(PageRenderer& renderer) : pageRenderer(renderer) {}

void UserInterface::processInput(char command) {
    if (command == 'j') pageRenderer.scrollPage(1);
    if (command == 'k') pageRenderer.scrollPage(-1);
    if (command == 'o') pageRenderer.highlightLink(selectedLinkIndex++);
}
