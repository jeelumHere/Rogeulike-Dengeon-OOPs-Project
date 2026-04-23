# Roguelike Dungeon — C++ OOP Project

A complete, beginner-friendly dungeon crawler game written in C++,
demonstrating every major Object-Oriented Programming concept.

---

## How to Build & Run

```bash
make        # compiles everything
make run    # compiles and launches the game
make clean  # deletes the binary and save file
```

Requires: `g++` with C++17 support (any modern Linux/Mac/Windows with MinGW).

---

## How to Play

```
W / A / S / D   →  Move Up / Left / Down / Right
I               →  Open Inventory (type a number to use an item)
P               →  Save game to savegame.txt
Q               →  Quit
```

**Goal:** Survive 3 dungeon levels and defeat the **Boss** (`B`) on level 3.

- Find the **Dungeon Key** (`?`) on each floor to unlock the stairs (`>`)
- Walk into enemies to attack them
- Items are picked up automatically when you step on them
- Use potions from your inventory (press `I`) to heal

### Map Symbols
```
@   You (the player)
#   Wall
.   Floor (explored)
>   Stairs to the next level (needs key)
?   Item on the ground
G   Goblin   (fast, weak)
O   Orc      (slow, strong)
B   Boss     (multi-phase, deadly)
    (blank)  Unexplored area
```

---

## Project Structure

```
roguelike/
├── main.cpp            Entry point (3 lines)
├── Item.cpp            Item use() implementations
├── Makefile            Build script
│
├── Position.h          2D coordinate (operator overloading)
├── Tile.h              One dungeon cell (encapsulation)
├── Entity.h            Abstract base + Character parent class
├── Item.h              Abstract Item + Weapon/Armor/Potion/Key
├── Inventory.h         Template generic item container
├── Player.h            Hero class (composition + inheritance)
├── Monster.h           Goblin / Orc / Boss (polymorphism)
├── DungeonMap.h        The 2D grid of tiles
├── DungeonGenerator.h  Procedural dungeon builder (abstraction)
├── CombatEngine.h      Combat resolver (encapsulation)
├── SaveManager.h       Save/load to file (file I/O + exceptions)
├── Renderer.h          ASCII console drawing (encapsulation)
└── GameEngine.h        Master orchestrator + game loop
```

---

## OOP Concepts — Where Each One Appears

| Concept               | File(s)                              | What it does                                          |
|-----------------------|--------------------------------------|-------------------------------------------------------|
| **Encapsulation**     | Every class                          | Private data, public methods only                     |
| **Abstraction**       | `Entity.h`, `Item.h`, `Monster.h`    | Pure virtual methods hide implementation details      |
| **Inheritance**       | `Entity.h` → `Player.h`, `Monster.h` | Shared behaviour inherited from parent classes        |
| **Polymorphism**      | `Monster.h`, `GameEngine.h`          | `runAI()` / `use()` behave differently per subclass  |
| **Composition**       | `Player.h`, `GameEngine.h`           | Player *owns* an Inventory; Engine *owns* all systems |
| **Templates**         | `Inventory.h`                        | `Inventory<T>` works for any item type                |
| **Operator Overload** | `Position.h`                         | `+`, `==`, `!=` on positions                         |
| **File I/O**          | `SaveManager.h`                      | `ofstream`/`ifstream` save & load                    |
| **Exception Handling**| `SaveManager.h`                      | `try/catch` for corrupt save files                    |
| **STL Containers**    | Throughout                           | `vector`, `unique_ptr` for automatic memory           |
| **Virtual Destructor**| `Entity.h`, `Item.h`                 | Prevents memory leaks when deleting via base pointer  |

---

## Class Hierarchy

```
Entity  (abstract)
└── Character  (abstract)
    ├── Player
    └── Monster  (abstract)
        ├── Goblin
        ├── Orc
        └── Boss

Item  (abstract)
├── Weapon
├── Armor
├── Potion
└── Key

Inventory<T>          (template — stores any Item subclass)
DungeonMap            (contains Tile objects)
DungeonGenerator      (builds DungeonMap procedurally)
CombatEngine          (stateless — resolves attacks)
Renderer              (draws to console)
SaveManager           (reads/writes savegame.txt)
GameEngine            (owns everything — runs the game loop)
```

---

## Game Flow

```
START
  │
  ▼
GameEngine::run()
  │
  ├─ Show main menu (New Game / Load / Quit)
  │
  ▼
generateLevel()
  ├─ DungeonGenerator builds a fresh DungeonMap
  ├─ Monsters are spawned into the map
  └─ Items are scattered on the ground
  │
  ▼
┌─────────────────── GAME LOOP ──────────────────┐
│                                                  │
│  1. Renderer draws map + entities + UI           │
│  2. Read player key press                        │
│  3. tryMovePlayer() → move OR attack monster     │
│     └─ CombatEngine::playerAttacks() if enemy    │
│        └─ Monster::takeDamage() [polymorphic]    │
│           └─ handleDeath() → give XP + gold      │
│  4. checkPickUp() → auto-collect items           │
│  5. updateMonsters() → each monster runs ai()    │
│     └─ Monster::runAI() [polymorphic per type]   │
│        └─ if on player → monsterAttacks()        │
│  6. Check: player dead? → Game Over              │
│     Check: boss dead?  → Victory!                │
│     Check: on stairs + has key? → next level     │
│                                                  │
└──────────────────────────────────────────────────┘
  │
  ▼
Win screen  OR  Game Over screen
```
