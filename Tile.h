// ============================================================
//  Tile.h  –  One Cell of the Dungeon Grid
//  The dungeon map is made of tiles.
//  Demonstrates: Encapsulation (private data, public methods)
// ============================================================

#pragma once
#include <string>

// Each tile has a type
enum class TileType
{
    WALL,       // Solid wall  – cannot walk through
    FLOOR,      // Open floor  – can walk on
    STAIRS_DOWN // Staircase   – leads to the next level
};

class Tile
{
private:
    // --- Private Data (Encapsulation) ---
    // Outside code cannot touch these directly
    TileType type;
    bool     visible;   // Has the player seen this tile?
    char     symbol;    // What character to draw on screen

public:
    // Constructor: create a tile of a given type
    Tile(TileType t = TileType::WALL) : type(t), visible(false)
    {
        // Choose the right symbol for each tile type
        if      (t == TileType::WALL)        symbol = '#';
        else if (t == TileType::FLOOR)       symbol = '.';
        else if (t == TileType::STAIRS_DOWN) symbol = '>';
    }

    // --- Public Interface (what outside code can do) ---
    bool isWall()    const { return type == TileType::WALL;  }
    bool isFloor()   const { return type == TileType::FLOOR; }
    bool isStairs()  const { return type == TileType::STAIRS_DOWN; }

    bool isVisible() const { return visible; }
    void setVisible(bool v){ visible = v; }

    char getSymbol() const { return symbol; }
    void setType(TileType t)
    {
        type = t;
        if      (t == TileType::WALL)        symbol = '#';
        else if (t == TileType::FLOOR)       symbol = '.';
        else if (t == TileType::STAIRS_DOWN) symbol = '>';
    }
};
