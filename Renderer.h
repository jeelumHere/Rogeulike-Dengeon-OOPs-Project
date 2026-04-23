// ============================================================
//  Renderer.h  –  Console Drawing System
//  Draws the dungeon map, entities, and UI to the terminal.
//  Demonstrates: Encapsulation (all rendering in one place)
//
//  If we ever wanted a graphical version, we'd only change
//  this one class – everything else stays the same.
// ============================================================

#pragma once
#include "DungeonMap.h"
#include "Player.h"
#include "Monster.h"
#include <iostream>
#include <string>
#include <vector>

class Renderer
{
public:
    // --- Draw the full game screen ---
    void render(const DungeonMap& map,
                const Player& player,
                const std::vector<Monster*>& monsters,
                const std::vector<std::pair<Position, Item*>>& groundItems,
                const std::string& message,
                int dungeonLevel) const
    {
        clearScreen();

        // --- Top border ---
        std::cout << "=== ROGUELIKE DUNGEON  |  Level " << dungeonLevel << " ===\n";

        // Build a character buffer from the map
        // We use a 2D array of chars, then overlay entities on top
        int w = map.getWidth();
        int h = map.getHeight();

        // Copy map tiles into a char grid
        std::vector<std::string> screen(h, std::string(w, ' '));

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const Tile& t = map.getTile(x, y);
                if (t.isVisible())
                    screen[y][x] = t.getSymbol();
                else
                    screen[y][x] = ' ';  // Unexplored = blank
            }
        }

        // Draw items on the ground
        for (const auto& pair : groundItems)
        {
            const Position& p = pair.first;
            if (map.inBounds(p.x, p.y) && map.getTile(p.x, p.y).isVisible())
                screen[p.y][p.x] = '?';   // '?' = item on floor
        }

        // Draw monsters
        for (const Monster* m : monsters)
        {
            if (!m->isAlive()) continue;
            Position p = m->getPos();
            if (map.inBounds(p.x, p.y) && map.getTile(p.x, p.y).isVisible())
                screen[p.y][p.x] = m->getSymbol();
        }

        // Draw player (always on top)
        Position pp = player.getPos();
        if (map.inBounds(pp.x, pp.y))
            screen[pp.y][pp.x] = player.getSymbol();

        // Output the screen
        for (const std::string& row : screen)
            std::cout << row << "\n";

        // --- Bottom UI Panel ---
        renderUI(player, message);
    }

    // --- Draw player stats and last message ---
    void renderUI(const Player& player, const std::string& message) const
    {
        std::cout << "----------------------------------------\n";
        std::cout << "HP: " << player.getHp() << "/" << player.getMaxHp()
                  << "  ATK: " << player.getAttack()
                  << "  DEF: " << player.getDefense()
                  << "  LVL: " << player.getLevel()
                  << "  XP: "  << player.getExperience()
                  << "  Gold: " << player.getGold();
        if (player.getHasKey()) std::cout << "  [KEY]";
        std::cout << "\n";

        // Equipped items
        std::cout << "Weapon: ";
        if (player.getWeapon()) std::cout << player.getWeapon()->getName();
        else                    std::cout << "(none)";
        std::cout << "  |  Armor: ";
        if (player.getArmor()) std::cout << player.getArmor()->getName();
        else                   std::cout << "(none)";
        std::cout << "\n";

        std::cout << "----------------------------------------\n";
        if (!message.empty())
            std::cout << "> " << message << "\n";

        std::cout << "WASD=Move  I=Inventory  P=Save  Q=Quit  ?=item on floor  >=Stairs\n";
    }

    // --- Show inventory screen ---
    void renderInventory(const Player& player) const
    {
        clearScreen();
        std::cout << "=== INVENTORY (max " << player.getBackpack().getCapacity()
                  << " items) ===\n";

        const Inventory<Item>& pack = player.getBackpack();

        if (pack.isEmpty())
        {
            std::cout << "(empty)\n";
        }
        else
        {
            for (int i = 0; i < pack.size(); ++i)
            {
                Item* item = pack.getItem(i);
                std::cout << "[" << i << "] " << item->getDescription() << "\n";
            }
        }

        std::cout << "----------------------------------------\n";
        std::cout << "Enter item number to USE, or press ENTER to close: ";
    }

private:
    // Clear the terminal screen (cross-platform-ish)
    void clearScreen() const
    {
        // Works on Linux/Mac. On Windows use system("cls")
        std::cout << "\033[2J\033[H";
    }
};
