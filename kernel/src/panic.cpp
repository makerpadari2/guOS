#include "panic.h"
#include "BasicRenderer.h"

/*
Panic screen for system errors
*/

void Panic(const char* panicMessage) {

    GlobalRenderer->ClearColour = 0x00ff0000;
    // Set screen to red
    GlobalRenderer->Clear();

    // Set cursor position to 0
    GlobalRenderer->CursorPosition = {0, 0};

    GlobalRenderer->Colour = 0;

    GlobalRenderer->Print("Kernel Panic");

    GlobalRenderer->Next();
    GlobalRenderer->Next();

    GlobalRenderer->Print(panicMessage);
}