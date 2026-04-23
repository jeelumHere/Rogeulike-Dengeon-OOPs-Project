// ============================================================
//  Player.h  –  The Hero (controlled by the user)
//  Demonstrates: Inheritance (Player extends Character),
//                Composition (Player OWNS an Inventory),
//                Encapsulation (private equipped weapon/armor)
// ============================================================

#pragma once
#include "Entity.h"
#include "Inventory.h"
#include "Item.h"

class Player : public Character
{
private:
    // Composition: Player owns an Inventory of Items
    Inventory<Item> backpack;

    // Currently equipped items (pointers into backpack, not owned twice)
    Weapon* equippedWeapon;
    Armor*  equippedArmor;

    int gold;         // Currency collected
    bool hasKey;      // Does the player hold the dungeon key?

public:
    // Constructor: creates player with starting stats
    Player()
        : Character('@', "Hero", 30, 5, 2),
          backpack(10),
          equippedWeapon(nullptr),
          equippedArmor(nullptr),
          gold(0),
          hasKey(false)
    {}

    // --- Inventory Actions ---

    // Pick up an item from the ground
    bool pickUp(Item* item)
    {
        return backpack.addItem(item);  // Returns false if full
    }

    // Drop item at index from backpack onto the ground
    Item* drop(int index)
    {
        Item* item = backpack.removeItem(index);
        // If we drop equipped weapon/armor, un-equip it
        if (item == equippedWeapon)
        {
            attack -= equippedWeapon->getDamage();
            equippedWeapon = nullptr;
        }
        if (item == equippedArmor)
        {
            defense -= equippedArmor->getDefense();
            equippedArmor = nullptr;
        }
        return item;
    }

    // Use item at index (equip or consume it)
    void useItem(int index)
    {
        Item* item = backpack.getItem(index);
        if (item) item->use(*this);   // Polymorphic: correct use() called
    }

    // --- Equipment ---

    // Equip a weapon (called by Weapon::use)
    void equipWeapon(Weapon* w)
    {
        // Remove old weapon bonus
        if (equippedWeapon)
            attack -= equippedWeapon->getDamage();
        equippedWeapon = w;
        // Add new weapon bonus
        attack += w->getDamage();
    }

    // Equip armor (called by Armor::use)
    void equipArmor(Armor* a)
    {
        if (equippedArmor)
            defense -= equippedArmor->getDefense();
        equippedArmor = a;
        defense += a->getDefense();
    }

    // --- Getters ---
    Inventory<Item>&       getBackpack()       { return backpack; }
    const Inventory<Item>& getBackpack() const { return backpack; }
    Weapon*          getWeapon()   const { return equippedWeapon; }
    Armor*           getArmor()    const { return equippedArmor; }
    int              getGold()     const { return gold; }
    bool             getHasKey()   const { return hasKey; }

    void addGold(int g)    { gold += g; }
    void setHasKey(bool k) { hasKey = k; }

    // Total damage = base attack (which already includes weapon bonus)
    int getTotalDamage() const { return attack; }
};
