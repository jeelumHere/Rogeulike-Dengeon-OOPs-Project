// ============================================================
//  Monster.h  –  Enemy Classes
//  Demonstrates: Inheritance, Polymorphism, Method Overriding
//
//  Monster (abstract) ← base for all enemies
//    Goblin  : fast, weak, hit-and-run AI
//    Orc     : slow, strong, charging AI
//    Boss    : multi-phase special attack AI
//
//  Each monster overrides ai() differently – that is
//  polymorphism: same function call, different behavior.
// ============================================================

#pragma once
#include "Entity.h"
#include "Item.h"
#include <cstdlib>  // for abs()

// ============================================================
//  Monster – Abstract Base (cannot create a "generic monster")
// ============================================================
class Monster : public Character
{
protected:
    int  aggroRange;  // How far it can "see" the player
    int  rewardXP;    // XP given to player on death
    int  rewardGold;  // Gold dropped on death
    Item* lootDrop;   // Item left behind when defeated

public:
    Monster(char sym, const std::string& n,
            int hp, int atk, int def,
            int xp, int gold, int range)
        : Character(sym, n, hp, atk, def),
          aggroRange(range),
          rewardXP(xp),
          rewardGold(gold),
          lootDrop(nullptr)
    {}

    virtual ~Monster()
    {
        // We do NOT delete lootDrop here –
        // it gets moved to the dungeon floor when the monster dies
    }

    // --- Pure Virtual: each monster must define its own AI ---
    virtual void runAI(Position playerPos) = 0;

    // --- Can the monster see the player? ---
    bool canSeePlayer(const Position& playerPos) const
    {
        int dx = abs(pos.x - playerPos.x);
        int dy = abs(pos.y - playerPos.y);
        int dist = dx + dy;   // Manhattan distance
        return dist <= aggroRange;
    }

    // --- Move one step toward a target ---
    void moveToward(const Position& target)
    {
        int dx = target.x - pos.x;
        int dy = target.y - pos.y;

        // Move whichever direction is farther first
        if (abs(dx) >= abs(dy))
            pos.x += (dx > 0) ? 1 : -1;
        else
            pos.y += (dy > 0) ? 1 : -1;
    }

    // --- Getters ---
    int   getRewardXP()   const { return rewardXP; }
    int   getRewardGold() const { return rewardGold; }
    Item* getLootDrop()   const { return lootDrop; }

    void  setLootDrop(Item* item) { lootDrop = item; }
};


// ============================================================
//  Goblin – Fast, weak, hit-and-run
// ============================================================
class Goblin : public Monster
{
private:
    bool retreating;  // Is goblin running away to regroup?

public:
    Goblin(Position startPos = Position())
        : Monster('G', "Goblin", 10, 3, 0, 10, 5, 5),
          retreating(false)
    {
        pos = startPos;
        // Goblins sometimes drop a small potion
        setLootDrop(new Potion("Small Potion", 8));
    }

    // Goblin AI: chase if HP high, flee if HP low
    void runAI(Position playerPos) override
    {
        if (!canSeePlayer(playerPos)) return;  // Cannot see player, stay still

        // If HP below 30%, flee away from player
        if (hp < maxHp * 0.3f)
        {
            retreating = true;
        }

        if (retreating)
        {
            // Move AWAY from player (opposite direction)
            int dx = pos.x - playerPos.x;
            int dy = pos.y - playerPos.y;
            if (abs(dx) >= abs(dy))
                pos.x += (dx > 0) ? 1 : -1;
            else
                pos.y += (dy > 0) ? 1 : -1;
        }
        else
        {
            // Move toward player to attack
            moveToward(playerPos);
        }
    }
};


// ============================================================
//  Orc – Slow, strong, holds ground
// ============================================================
class Orc : public Monster
{
public:
    Orc(Position startPos = Position())
        : Monster('O', "Orc", 25, 7, 3, 25, 15, 4)
    {
        pos = startPos;
        // Orcs drop a weapon or armor
        setLootDrop(new Weapon("Rusty Sword", 3));
    }

    // Orc AI: slow and steady, charges straight at player
    void runAI(Position playerPos) override
    {
        if (!canSeePlayer(playerPos)) return;

        // Orcs move every other turn (slow) – simulated by 50% chance
        if (rand() % 2 == 0) return;

        moveToward(playerPos);
    }
};


// ============================================================
//  Boss – Multi-phase, special attacks
// ============================================================
class Boss : public Monster
{
private:
    int  turnCount;     // Counts turns for special attack timing
    bool phase2;        // Becomes true when HP drops below 50%

public:
    Boss(Position startPos = Position())
        : Monster('B', "Shadow Wraith (BOSS)", 80, 12, 5, 100, 50, 8),
          turnCount(0),
          phase2(false)
    {
        pos = startPos;
        setLootDrop(new Weapon("Shadow Blade", 10));
    }

    // Special attack deals bonus damage directly (bypasses defense)
    // Returns the bonus damage so GameEngine can apply it
    int specialAttack()
    {
        return 8;  // AOE fire blast – 8 direct damage
    }

    // Boss AI: two phases
    void runAI(Position playerPos) override
    {
        if (!canSeePlayer(playerPos)) return;

        turnCount++;

        // Enter phase 2 when HP drops below 50%
        if (hp < maxHp / 2) phase2 = true;

        if (phase2)
        {
            // Phase 2: faster, uses special attacks every 3 turns
            moveToward(playerPos);
        }
        else
        {
            // Phase 1: slow approach
            if (turnCount % 2 == 0)
                moveToward(playerPos);
        }
    }

    bool isInPhase2()     const { return phase2; }
    int  getTurnCount()   const { return turnCount; }
};
