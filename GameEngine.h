// ============================================================
//  GameEngine.h  –  The Heart of the Game
//  Demonstrates: Composition (owns all subsystems),
//                Game Loop pattern, Win/Lose conditions
//
//  GameEngine owns:
//    - Player          (the hero)
//    - DungeonMap      (the current level)
//    - DungeonGenerator(builds levels)
//    - list of Monsters
//    - items on the ground
//    - CombatEngine
//    - Renderer
//    - SaveManager
// ============================================================

#pragma once
#include "Player.h"
#include "Monster.h"
#include "DungeonMap.h"
#include "DungeonGenerator.h"
#include "CombatEngine.h"
#include "Renderer.h"
#include "SaveManager.h"
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>   // unique_ptr
#include <algorithm> // remove_if

class GameEngine
{
private:
    // --- All game systems (Composition) ---
    Player           player;
    DungeonMap       dungeon;
    DungeonGenerator generator;
    CombatEngine     combat;
    Renderer         renderer;
    SaveManager      saves;

    // Monsters are owned by the engine via unique_ptr (auto memory cleanup)
    std::vector<std::unique_ptr<Monster>> monsters;

    // Items lying on the ground: position + pointer to the item
    std::vector<std::pair<Position, std::unique_ptr<Item>>> groundItems;

    // Game state
    bool        running;
    bool        playerWon;
    int         dungeonLevel;   // Current dungeon floor
    std::string message;        // Last event message shown to player

    // -------------------------------------------------------
    //  Setup a fresh dungeon level
    // -------------------------------------------------------
    void generateLevel()
    {
        // Each level is wider/taller and has more rooms
        int w    = 40 + dungeonLevel * 2;
        int h    = 20 + dungeonLevel;
        int nRooms = 5 + dungeonLevel * 2;

        // Use current time as seed so every run is different
        unsigned int seed = (unsigned int)time(nullptr) + dungeonLevel * 100;

        dungeon = generator.generate(w, h, nRooms, seed);

        // Place the player in the first room
        player.setPos(generator.getPlayerStart());

        // Clear old monsters and items
        monsters.clear();
        groundItems.clear();

        // Reveal starting area
        dungeon.revealAround(player.getPos().x, player.getPos().y, 5);

        // Spawn monsters – more on deeper levels
        spawnMonsters();

        // Scatter some items on the ground
        spawnGroundItems();

        message = "Welcome to dungeon level " + std::to_string(dungeonLevel) + "!";
    }

    // -------------------------------------------------------
    //  Spawn monsters for this level
    // -------------------------------------------------------
    void spawnMonsters()
    {
        int numGoblins = 2 + dungeonLevel;
        int numOrcs    = dungeonLevel;   // No orcs on level 1
        bool spawnBoss = (dungeonLevel == 3);  // Boss on level 3

        for (int i = 0; i < numGoblins; ++i)
        {
            Position p = generator.getRandomFloorPos();
            // Don't spawn on the player's starting tile
            if (p != player.getPos())
                monsters.push_back(std::make_unique<Goblin>(p));
        }

        for (int i = 0; i < numOrcs; ++i)
        {
            Position p = generator.getRandomFloorPos();
            if (p != player.getPos())
                monsters.push_back(std::make_unique<Orc>(p));
        }

        if (spawnBoss)
        {
            // Boss goes into the back of the dungeon
            Position p = generator.getRandomFloorPos();
            monsters.push_back(std::make_unique<Boss>(p));
            message = "You sense a terrible presence ahead...";
        }
    }

    // -------------------------------------------------------
    //  Place random items on the ground
    // -------------------------------------------------------
    void spawnGroundItems()
    {
        // A few potions always available
        for (int i = 0; i < 2; ++i)
        {
            Position p = generator.getRandomFloorPos();
            groundItems.push_back({p, std::make_unique<Potion>("Health Potion", 15)});
        }

        // Occasionally a weapon or armor
        if (rand() % 2 == 0)
        {
            Position p = generator.getRandomFloorPos();
            groundItems.push_back({p, std::make_unique<Weapon>("Iron Sword", 4 + dungeonLevel)});
        }
        if (rand() % 2 == 0)
        {
            Position p = generator.getRandomFloorPos();
            groundItems.push_back({p, std::make_unique<Armor>("Leather Armor", 2 + dungeonLevel)});
        }

        // Key needed to use stairs
        Position p = generator.getRandomFloorPos();
        groundItems.push_back({p, std::make_unique<Key>()});
    }

    // -------------------------------------------------------
    //  Try to move the player in a direction
    //  Returns true if the player moved (for turn advancement)
    // -------------------------------------------------------
    bool tryMovePlayer(int dx, int dy)
    {
        Position newPos(player.getPos().x + dx, player.getPos().y + dy);

        if (!dungeon.inBounds(newPos.x, newPos.y)) return false;
        if (!dungeon.isWalkable(newPos.x, newPos.y)) return false;

        // Check if a monster is at the new position
        Monster* target = getMonsterAt(newPos);
        if (target)
        {
            // Attack instead of move
            message = "";
            combat.playerAttacks(player, *target);
            message = combat.getLastMessage();

            if (!target->isAlive())
            {
                // Give rewards
                combat.handleDeath(*target, player);
                message += " (+" + std::to_string(target->getRewardXP()) + " XP, +"
                         + std::to_string(target->getRewardGold()) + " gold)";

                // Drop loot on the ground
                if (target->getLootDrop())
                {
                    Item* loot = target->getLootDrop();
                    groundItems.push_back({target->getPos(),
                                           std::unique_ptr<Item>(loot->clone())});
                }

                // Check win: is this the boss?
                Boss* boss = dynamic_cast<Boss*>(target);
                if (boss)
                {
                    playerWon = true;
                    running   = false;
                    message  += " *** YOU DEFEATED THE BOSS! YOU WIN! ***";
                }
            }
            return true;  // Still counts as a turn
        }

        // No monster – actually move
        player.setPos(newPos);

        // Reveal tiles around new position
        dungeon.revealAround(newPos.x, newPos.y, 5);

        // Check for item on the ground
        checkPickUp();

        // Check for stairs
        if (dungeon.getTile(newPos.x, newPos.y).isStairs())
        {
            if (player.getHasKey())
            {
                message = "You descend to the next level!";
                player.setHasKey(false);
                dungeonLevel++;
                generateLevel();
            }
            else
            {
                message = "The stairs are locked. Find the dungeon key first!";
            }
        }

        return true;
    }

    // -------------------------------------------------------
    //  Automatically pick up items when player steps on them
    // -------------------------------------------------------
    void checkPickUp()
    {
        Position pp = player.getPos();
        for (int i = (int)groundItems.size() - 1; i >= 0; --i)
        {
            if (groundItems[i].first == pp)
            {
                Item* item = groundItems[i].second.get();

                // Special: key is picked up automatically
                Key* k = dynamic_cast<Key*>(item);
                if (k)
                {
                    player.setHasKey(true);
                    message = "You picked up the Dungeon Key!";
                    groundItems.erase(groundItems.begin() + i);
                    return;
                }

                // Other items: add to backpack
                if (!player.getBackpack().isFull())
                {
                    // Release unique_ptr ownership, move to backpack
                    Item* raw = groundItems[i].second.release();
                    player.pickUp(raw);
                    message = "Picked up: " + raw->getName();
                    groundItems.erase(groundItems.begin() + i);
                }
                else
                {
                    message = "Inventory full! Can't pick up " + item->getName();
                }
                return;
            }
        }
    }

    // -------------------------------------------------------
    //  Run all monster AI turns
    // -------------------------------------------------------
    void updateMonsters()
    {
        Position playerPos = player.getPos();

        for (auto& mPtr : monsters)
        {
            Monster* m = mPtr.get();
            if (!m->isAlive()) continue;

            // Save old position before AI move
            Position oldPos = m->getPos();

            // Run the monster's AI (polymorphic: correct ai() called)
            m->runAI(playerPos);

            // Did the monster walk onto the player?
            if (m->getPos() == playerPos)
            {
                // Undo the move (monster attacks instead)
                m->setPos(oldPos);

                // Boss phase 2 special attack every 3 turns
                Boss* boss = dynamic_cast<Boss*>(m);
                if (boss && boss->isInPhase2() && boss->getTurnCount() % 3 == 0)
                {
                    combat.bossSpecialAttack(*boss, player);
                }
                else
                {
                    combat.monsterAttacks(*m, player);
                }

                message = combat.getLastMessage();

                if (!player.isAlive())
                {
                    running = false;  // Player is dead
                }
            }

            // Check if monster walked into another monster – if so, undo
            Monster* collision = getMonsterAt(m->getPos());
            if (collision && collision != m)
                m->setPos(oldPos);
        }

        // Remove dead monsters
        monsters.erase(
            std::remove_if(monsters.begin(), monsters.end(),
                [](const std::unique_ptr<Monster>& m){ return !m->isAlive(); }),
            monsters.end());
    }

    // -------------------------------------------------------
    //  Find a monster at a position (returns nullptr if none)
    // -------------------------------------------------------
    Monster* getMonsterAt(const Position& p)
    {
        for (auto& mPtr : monsters)
        {
            if (mPtr->isAlive() && mPtr->getPos() == p)
                return mPtr.get();
        }
        return nullptr;
    }

    // -------------------------------------------------------
    //  Build a raw pointer list for the Renderer
    //  (Renderer doesn't need ownership)
    // -------------------------------------------------------
    std::vector<Monster*> getMonsterPtrs() const
    {
        std::vector<Monster*> v;
        for (const auto& m : monsters)
            v.push_back(m.get());
        return v;
    }

    std::vector<std::pair<Position, Item*>> getGroundItemPtrs() const
    {
        std::vector<std::pair<Position, Item*>> v;
        for (const auto& pair : groundItems)
            v.push_back({pair.first, pair.second.get()});
        return v;
    }

    // -------------------------------------------------------
    //  Handle the Inventory screen
    // -------------------------------------------------------
    void openInventory()
    {
        renderer.renderInventory(player);

        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) return;

        try
        {
            int index = std::stoi(input);
            if (index >= 0 && index < player.getBackpack().size())
            {
                player.useItem(index);
                message = "Used item from inventory.";
            }
        }
        catch (...) { /* ignore bad input */ }
    }

    // -------------------------------------------------------
    //  Show the welcome / main menu screen
    // -------------------------------------------------------
    void showMainMenu()
    {
        std::cout << "\033[2J\033[H";  // Clear screen
        std::cout << "==============================\n";
        std::cout << "     ROGUELIKE DUNGEON\n";
        std::cout << "==============================\n";
        std::cout << "  An OOP C++ Adventure\n\n";
        std::cout << "[N] New Game\n";
        if (saves.exists())
            std::cout << "[L] Load Save\n";
        std::cout << "[Q] Quit\n\n";
        std::cout << "Your choice: ";
    }

    // -------------------------------------------------------
    //  Apply loaded save data back onto the player
    // -------------------------------------------------------
    void applySaveData(const SaveData& data)
    {
        player.setPos(Position(data.playerX, data.playerY));
        // Directly set stats (a bit of a shortcut for simplicity)
        player.setAttack(data.playerAttack);
        player.setDefense(data.playerDefense);
        player.setMaxHp(data.playerMaxHp);
        player.heal(data.playerHp - player.getHp());
        player.addGold(data.playerGold);
        player.setHasKey(data.playerHasKey);
        dungeonLevel = data.dungeonLevel;
    }

public:
    GameEngine()
        : running(false), playerWon(false), dungeonLevel(1)
    {
        srand((unsigned int)time(nullptr));  // Seed random number generator
    }

    // -------------------------------------------------------
    //  MAIN ENTRY POINT – called from main()
    // -------------------------------------------------------
    void run()
    {
        showMainMenu();

        char choice;
        std::cin >> choice;
        std::cin.ignore();  // Discard the newline after the character

        if (choice == 'Q' || choice == 'q')
            return;

        if ((choice == 'L' || choice == 'l') && saves.exists())
        {
            // Load saved game
            SaveData data = saves.load();
            if (data.valid)
            {
                dungeonLevel = data.dungeonLevel;
                generateLevel();
                applySaveData(data);
                message = "Save loaded! Welcome back.";
            }
            else
            {
                message = "Could not load save. Starting new game.";
                dungeonLevel = 1;
                generateLevel();
            }
        }
        else
        {
            // New game
            dungeonLevel = 1;
            generateLevel();
        }

        // =============================================
        //  MAIN GAME LOOP
        // =============================================
        running = true;
        while (running)
        {
            // 1. Render everything
            renderer.render(dungeon, player, getMonsterPtrs(),
                            getGroundItemPtrs(), message, dungeonLevel);

            // Clear message so it doesn't repeat
            message = "";

            // 2. Get player input (single key, no Enter needed on most systems)
            //    For simplicity we use cin which requires Enter
            char key;
            std::cin.get(key);

            bool tookTurn = false;

            // 3. Process input
            //    Movement : W=Up  A=Left  S=Down  D=Right
            //    Actions  : I=Inventory  P=Save  Q=Quit
            switch (key)
            {
                // --- Movement (WASD) ---
                case 'w': case 'W': tookTurn = tryMovePlayer( 0, -1); break;
                case 's': case 'S': tookTurn = tryMovePlayer( 0,  1); break;
                case 'a': case 'A': tookTurn = tryMovePlayer(-1,  0); break;
                case 'd': case 'D': tookTurn = tryMovePlayer( 1,  0); break;

                // --- Open Inventory ---
                case 'i': case 'I':
                    openInventory();
                    tookTurn = true;   // Using inventory costs a turn
                    break;

                // --- Save Game (P for preserve) ---
                case 'p': case 'P':
                    saves.save(player, dungeonLevel);
                    message = "Game saved!";
                    break;

                // --- Quit ---
                case 'q': case 'Q':
                    running = false;
                    break;

                // --- Ignore Enter / newline ---
                case '\n': case '\r':
                    break;

                // --- Unknown key: show help ---
                default:
                    message = "Keys: WASD=Move  I=Inventory  P=Save  Q=Quit";
                    break;
            }

            // 4. Monsters take their turn after the player moves
            if (tookTurn && running)
            {
                updateMonsters();
            }
        }

        // =============================================
        //  GAME OVER SCREEN
        // =============================================
        std::cout << "\033[2J\033[H";
        if (playerWon)
        {
            std::cout << "╔══════════════════════════════╗\n";
            std::cout << "║   *** VICTORY! YOU WIN! ***  ║\n";
            std::cout << "╚══════════════════════════════╝\n";
            std::cout << "You defeated the Shadow Wraith and escaped the dungeon!\n";
        }
        else
        {
            std::cout << "╔══════════════════════════════╗\n";
            std::cout << "║        GAME OVER...          ║\n";
            std::cout << "╚══════════════════════════════╝\n";
            std::cout << "You were slain in the dungeon depths.\n";
        }

        std::cout << "\nFinal Stats:\n";
        std::cout << "  Level: " << player.getLevel() << "\n";
        std::cout << "  Gold:  " << player.getGold()  << "\n";
        std::cout << "  Floor: " << dungeonLevel       << "\n";
        std::cout << "\nPress Enter to exit...\n";
        std::cin.ignore();
        std::cin.get();
    }
};
