#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "PageRenderer.h"

class UserInterface {
public:
    explicit UserInterface(PageRenderer& renderer);
    void processInput(char command);

private:
    PageRenderer& pageRenderer;
    int selectedLinkIndex = 0;
};

#endif // USERINTERFACE_H
