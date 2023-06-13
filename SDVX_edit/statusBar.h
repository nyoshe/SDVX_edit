#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include <string>
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