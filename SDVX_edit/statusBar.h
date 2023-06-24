#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "editWindow.h"
#include "fileDialog/ImGuiFileDialog.h"
#include "scrubBar.h"
#include <string>

static uint8_t laserSelection = 0x01;
static const uint8_t NORMAL = 0x01;
static const uint8_t FINE = 0x02;
static const uint8_t VERY_FINE = 0x04;
static uint8_t playbackSpeed = 0x01;


class StatusBar
{
private:
    ImFont* font;

public:
    StatusBar();
    ~StatusBar() = default;
    void update();
};