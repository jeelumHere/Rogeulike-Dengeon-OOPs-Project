// ============================================================
//  SaveManager.h  –  Save & Load Game State
//  Demonstrates: File I/O, Exception Handling
//
//  Saves a simple text file with player stats so the game
//  can be continued. Kept simple on purpose (text, not binary).
// ============================================================

#pragma once
#include "Player.h"
#include <fstream>
#include <iostream>
#include <string>

// A plain struct holding just what we need to save
struct SaveData
{
    int  playerHp;
    int  playerMaxHp;
    int  playerAttack;
    int  playerDefense;
    int  playerLevel;
    int  playerExperience;
    int  playerGold;
    bool playerHasKey;
    int  playerX;
    int  playerY;
    int  dungeonLevel;
    bool valid;   // Was loading successful?
};

class SaveManager
{
private:
    std::string savePath;   // Path to the save file

public:
    SaveManager(const std::string& path = "savegame.txt")
        : savePath(path) {}

    // --- Save player state to a text file ---
    void save(const Player& player, int dungeonLevel) const
    {
        try
        {
            std::ofstream file(savePath);   // Open file for writing
            if (!file.is_open())
                throw std::runtime_error("Cannot open save file for writing.");

            // Write each value on its own line
            file << player.getHp()         << "\n";
            file << player.getMaxHp()      << "\n";
            file << player.getAttack()     << "\n";
            file << player.getDefense()    << "\n";
            file << player.getLevel()      << "\n";
            file << player.getExperience() << "\n";
            file << player.getGold()       << "\n";
            file << player.getHasKey()     << "\n";
            file << player.getPos().x      << "\n";
            file << player.getPos().y      << "\n";
            file << dungeonLevel           << "\n";

            file.close();
            std::cout << "[Game saved to " << savePath << "]\n";
        }
        catch (const std::exception& e)
        {
            // Show error but don't crash the game
            std::cerr << "Save error: " << e.what() << "\n";
        }
    }

    // --- Load player state from the text file ---
    SaveData load() const
    {
        SaveData data{};
        data.valid = false;

        try
        {
            std::ifstream file(savePath);   // Open file for reading
            if (!file.is_open())
                throw std::runtime_error("No save file found.");

            file >> data.playerHp;
            file >> data.playerMaxHp;
            file >> data.playerAttack;
            file >> data.playerDefense;
            file >> data.playerLevel;
            file >> data.playerExperience;
            file >> data.playerGold;
            file >> data.playerHasKey;
            file >> data.playerX;
            file >> data.playerY;
            file >> data.dungeonLevel;

            if (file.fail())
                throw std::runtime_error("Save file is corrupt.");

            file.close();
            data.valid = true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Load error: " << e.what() << "\n";
        }

        return data;
    }

    // --- Check if a save file exists ---
    bool exists() const
    {
        std::ifstream file(savePath);
        return file.good();
    }

    // --- Delete the save file ---
    void deleteSave() const
    {
        std::remove(savePath.c_str());
        std::cout << "[Save file deleted.]\n";
    }
};
