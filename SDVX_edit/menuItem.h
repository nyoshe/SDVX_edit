#pragma once
#include "imgui.h"
#include "imgui-SFML.h"

class menuItem
{
private:
    static menuItem* instance;
    menuItem() = default;
    ~menuItem() = default;

public:
    menuItem(const menuItem&) = delete;
    menuItem& operator=(const menuItem&) = delete;
    static menuItem* getInstance() {
        if (!instance) {
            instance = new menuItem();
        }
        return instance;
    }
};

