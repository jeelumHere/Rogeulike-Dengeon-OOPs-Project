// ============================================================
//  Entity.h  –  Abstract Base Class for Everything in the World
//  Demonstrates: Abstraction, Polymorphism, Virtual Methods
//
//  Entity is the ROOT of the entire class hierarchy.
//  Player, Monster, and items on the ground are all Entities.
// ============================================================

#pragma once
#include "Position.h"
#include <string>

// ============================================================
//  Entity – Abstract Base (cannot be created directly)
// ============================================================
class Entity
{
protected:
    Position    pos;      // Where is this entity on the map?
    char        symbol;   // What character represents it? '@', 'G', 'O' …
    bool        alive;    // Is it still in the game?
    int         id;       // Unique ID number
    std::string name;

public:
    Entity(char sym, const std::string& n, Position p = Position())
        : pos(p), symbol(sym), alive(true), id(0), name(n) {}

    // Virtual destructor – REQUIRED whenever you have virtual methods
    // Without this, deleting via base pointer could skip subclass cleanup
    virtual ~Entity() {}

    // --- Pure Virtual Methods ---
    // Every subclass MUST implement these
    virtual char getSymbol() const { return symbol; }

    // --- Getters & Setters ---
    Position    getPos()    const { return pos; }
    void        setPos(Position p){ pos = p; }
    bool        isAlive()   const { return alive; }
    void        setAlive(bool a)  { alive = a; }
    int         getId()     const { return id; }
    void        setId(int i)      { id = i; }
    std::string getName()   const { return name; }
};


// ============================================================
//  Character.h  –  Parent for Player and Monster
//  Adds combat stats: HP, attack, defense, level, XP
//  Demonstrates: Inheritance (Character extends Entity)
// ============================================================

class Character : public Entity
{
protected:
    int hp;        // Current hit points
    int maxHp;     // Maximum hit points
    int attack;    // Base attack power
    int defense;   // Base damage reduction
    int level;     // Character level
    int experience;// XP collected so far

public:
    Character(char sym, const std::string& n,
              int hp, int atk, int def)
        : Entity(sym, n),
          hp(hp), maxHp(hp),
          attack(atk), defense(def),
          level(1), experience(0)
    {}

    // --- Combat Methods ---

    // Take damage from an attack (defense reduces it)
    virtual void takeDamage(int dmg)
    {
        int actualDamage = dmg - defense;
        if (actualDamage < 1) actualDamage = 1;  // Always at least 1 damage
        hp -= actualDamage;
        if (hp <= 0)
        {
            hp    = 0;
            alive = false;  // Character is dead
        }
    }

    // Restore HP (cannot go above maxHp)
    void heal(int amount)
    {
        hp += amount;
        if (hp > maxHp) hp = maxHp;
    }

    // --- Getters ---
    int getHp()         const { return hp; }
    int getMaxHp()      const { return maxHp; }
    int getAttack()     const { return attack; }
    int getDefense()    const { return defense; }
    int getLevel()      const { return level; }
    int getExperience() const { return experience; }

    // --- Setters (used by equipment) ---
    void setAttack(int a)  { attack  = a; }
    void setDefense(int d) { defense = d; }
    void setMaxHp(int m)   { maxHp   = m; }

    // Gain XP and check for level-up
    void gainExperience(int xp)
    {
        experience += xp;
        int threshold = level * 20;   // Need 20 * current level to level up
        if (experience >= threshold)
        {
            level++;
            experience -= threshold;
            // Level up bonuses
            maxHp  += 5;
            hp      = maxHp;          // Full heal on level up
            attack  += 2;
            defense += 1;
        }
    }
};
