// ============================================================
//  CombatEngine.h  –  Combat Resolution System
//  Demonstrates: Encapsulation (hides combat math),
//                Polymorphism (calls virtual takeDamage)
//
//  CombatEngine is stateless: it doesn't store anything.
//  It just receives two Characters and resolves the fight.
// ============================================================

#pragma once
#include "Entity.h"
#include "Player.h"
#include "Monster.h"
#include <string>
#include <cstdlib>  // rand()

class CombatEngine
{
private:
    // Last combat message (shown in the UI)
    mutable std::string lastMessage;

public:
    CombatEngine() : lastMessage("") {}

    // --- Player attacks a Monster ---
    // Returns the damage dealt
    int playerAttacks(Player& player, Monster& monster)
    {
        // Calculate damage: player attack + some randomness (±2)
        int dmg = player.getTotalDamage() + (rand() % 5) - 2;
        if (dmg < 1) dmg = 1;

        monster.takeDamage(dmg);   // Virtual call: correct takeDamage() runs

        lastMessage = "You hit the " + monster.getName()
                    + " for " + std::to_string(dmg) + " damage!";

        if (!monster.isAlive())
        {
            lastMessage += " The " + monster.getName() + " is defeated!";
        }

        return dmg;
    }

    // --- Monster attacks the Player ---
    // Returns the damage dealt
    int monsterAttacks(Monster& monster, Player& player)
    {
        int dmg = monster.getAttack() + (rand() % 4) - 1;
        if (dmg < 1) dmg = 1;

        player.takeDamage(dmg);

        lastMessage = "The " + monster.getName()
                    + " hits you for " + std::to_string(dmg) + " damage!";

        if (!player.isAlive())
        {
            lastMessage += " You have been defeated...";
        }

        return dmg;
    }

    // --- Boss Special Attack (phase 2, bypasses defense) ---
    int bossSpecialAttack(Boss& boss, Player& player)
    {
        int dmg = boss.specialAttack();
        player.heal(-dmg);  // Direct HP reduction (no defense calc)
        if (player.getHp() <= 0)
            player.setAlive(false);

        lastMessage = "The Boss unleashes a SHADOW BLAST for "
                    + std::to_string(dmg) + " direct damage!!";
        return dmg;
    }

    // --- Get the description of the last combat action ---
    std::string getLastMessage() const { return lastMessage; }

    // --- Handle death rewards: give XP and gold to player ---
    void handleDeath(Monster& monster, Player& player)
    {
        int xp   = monster.getRewardXP();
        int gold = monster.getRewardGold();

        player.gainExperience(xp);
        player.addGold(gold);
    }
};
