// ============================================================
//  Item.h  –  Item Base Class + All Item Subclasses
//  Items the player can pick up: Weapon, Armor, Potion, Key.
//  Demonstrates: Abstraction (pure virtual), Inheritance,
//                Polymorphism
// ============================================================

#pragma once
#include <string>

// Forward declare Character so Item can reference it
class Character;

// ============================================================
//  ABSTRACT BASE CLASS – Item
//  Cannot be created directly. Only Weapon/Armor/etc. can.
// ============================================================
class Item
{
protected:
    std::string name;   // e.g. "Short Sword", "Health Potion"
    int         weight; // For future carry-weight limits

public:
    Item(const std::string& n, int w)
        : name(n), weight(w) {}

    // Virtual destructor – important for inheritance cleanup
    virtual ~Item() {}

    // --- Pure Virtual Methods (must be implemented by subclasses) ---
    // Every item MUST define what happens when used
    virtual void use(Character& user) = 0;

    // Clone pattern: lets us copy an item without knowing its exact type
    virtual Item* clone() const = 0;

    // --- Regular Getters ---
    std::string getName()   const { return name; }
    int         getWeight() const { return weight; }

    // Each item type will override this to give a description
    virtual std::string getDescription() const { return name; }
};


// ============================================================
//  Weapon – increases damage in combat
// ============================================================
class Weapon : public Item
{
private:
    int baseDamage;  // Bonus damage when equipped

public:
    Weapon(const std::string& n, int dmg)
        : Item(n, 2), baseDamage(dmg) {}

    // Equip this weapon onto the character (implemented in Item.cpp)
    void use(Character& user) override;

    Item* clone() const override
    {
        return new Weapon(*this);  // Copy this weapon
    }

    int getDamage() const { return baseDamage; }

    std::string getDescription() const override
    {
        return name + " (ATK +" + std::to_string(baseDamage) + ")";
    }
};


// ============================================================
//  Armor – reduces damage taken in combat
// ============================================================
class Armor : public Item
{
private:
    int defense;  // Damage reduction when equipped

public:
    Armor(const std::string& n, int def)
        : Item(n, 3), defense(def) {}

    void use(Character& user) override;

    Item* clone() const override
    {
        return new Armor(*this);
    }

    int getDefense() const { return defense; }

    std::string getDescription() const override
    {
        return name + " (DEF +" + std::to_string(defense) + ")";
    }
};


// ============================================================
//  Potion – restores HP when used
// ============================================================
class Potion : public Item
{
private:
    int healAmount;  // How many HP it restores

public:
    Potion(const std::string& n, int heal)
        : Item(n, 1), healAmount(heal) {}

    void use(Character& user) override;

    Item* clone() const override
    {
        return new Potion(*this);
    }

    int getHealAmount() const { return healAmount; }

    std::string getDescription() const override
    {
        return name + " (Heals " + std::to_string(healAmount) + " HP)";
    }
};


// ============================================================
//  Key – unlocks the stairs to the next level
// ============================================================
class Key : public Item
{
public:
    Key() : Item("Dungeon Key", 0) {}

    void use(Character& user) override;  // Message: "Used the key"

    Item* clone() const override
    {
        return new Key(*this);
    }

    std::string getDescription() const override
    {
        return "Dungeon Key (opens the way down)";
    }
};
