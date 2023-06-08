#pragma once
#include "imgui.h"
#include "imgui-SFML.h"

class statusBar
{
private:
    static statusBar* instance;
    statusBar() = default;
    ~statusBar() = default;

public:
    statusBar(const statusBar&) = delete;
    statusBar(statusBar&&) = delete;
    statusBar& operator=(const statusBar&) = delete;
    statusBar& operator=(statusBar&&) = delete;
    static statusBar& getInstance()
    {
        static statusBar instance;
        return instance;
    }
    void update();
};