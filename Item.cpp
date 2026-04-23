// ============================================================
//  Item.cpp  –  Item use() Implementations
//  These are in a .cpp (not .h) because they need the full
//  definition of Player which includes Inventory which includes
//  Item – including everything in Item.h would be circular.
//  So we forward-declare Character in Item.h and implement here.
// ============================================================

#include "Item.h"
#include "Player.h"  // Full definition available here
#include <iostream>

// --- Weapon::use() ---
// When a player "uses" a weapon, they equip it.
void Weapon::use(Character& user)
{
    // Dynamic cast: check if user is actually a Player
    Player* player = dynamic_cast<Player*>(&user);
    if (player)
    {
        player->equipWeapon(this);
        std::cout << "Equipped " << name << "!\n";
    }
}

// --- Armor::use() ---
void Armor::use(Character& user)
{
    Player* player = dynamic_cast<Player*>(&user);
    if (player)
    {
        player->equipArmor(this);
        std::cout << "Equipped " << name << "!\n";
    }
}

// --- Potion::use() ---
// Potions heal the character who uses them (works for any Character)
void Potion::use(Character& user)
{
    user.heal(healAmount);
    std::cout << "Used " << name << "! Restored " << healAmount << " HP.\n";
}

// --- Key::use() ---
void Key::use(Character& /*user*/)
{
    // The GameEngine handles key logic (checking stairs etc.)
    // Just print a message here
    std::cout << "The key unlocks the stairs to the next level.\n";
}
