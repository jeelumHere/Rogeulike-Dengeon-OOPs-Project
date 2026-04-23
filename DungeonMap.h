// ============================================================
//  DungeonMap.h  –  The Dungeon Grid
//  Holds a 2D grid of Tiles.
//  Demonstrates: Encapsulation (hides the raw grid),
//                Composition (DungeonMap CONTAINS Tiles)
// ============================================================

#pragma once
#include "Tile.h"
#include <vector>

class DungeonMap
{
private:
    // The actual grid – a 2D vector of Tile objects
    // grid[row][col] or grid[y][x]
    std::vector<std::vector<Tile>> grid;

    int width;   // Number of columns
    int height;  // Number of rows

public:
    // Create a map filled with walls
    DungeonMap(int w = 40, int h = 20)
        : width(w), height(h)
    {
        // Fill every cell with a WALL tile
        grid.assign(height, std::vector<Tile>(width, Tile(TileType::WALL)));
    }

    // --- Bounds Check ---
    // Always call this before accessing a tile to avoid crashes
    bool inBounds(int x, int y) const
    {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    // --- Tile Access (read-only version) ---
    const Tile& getTile(int x, int y) const
    {
        return grid[y][x];
    }

    // --- Tile Access (writable version) ---
    Tile& getTileRef(int x, int y)
    {
        return grid[y][x];
    }

    // --- Convenience Checks ---
    bool isWalkable(int x, int y) const
    {
        if (!inBounds(x, y)) return false;
        return !grid[y][x].isWall();
    }

    bool isFloor(int x, int y) const
    {
        if (!inBounds(x, y)) return false;
        return grid[y][x].isFloor();
    }

    // --- Set a tile's type ---
    void setTile(int x, int y, TileType t)
    {
        if (inBounds(x, y))
            grid[y][x].setType(t);
    }

    // --- Make a rectangular area into floor tiles ---
    void carveRoom(int x, int y, int w, int h)
    {
        for (int row = y; row < y + h; ++row)
            for (int col = x; col < x + w; ++col)
                setTile(col, row, TileType::FLOOR);
    }

    // --- Carve a horizontal corridor ---
    void carveHorizontalCorridor(int x1, int x2, int y)
    {
        int startX = (x1 < x2) ? x1 : x2;
        int endX   = (x1 < x2) ? x2 : x1;
        for (int x = startX; x <= endX; ++x)
            setTile(x, y, TileType::FLOOR);
    }

    // --- Carve a vertical corridor ---
    void carveVerticalCorridor(int x, int y1, int y2)
    {
        int startY = (y1 < y2) ? y1 : y2;
        int endY   = (y1 < y2) ? y2 : y1;
        for (int y = startY; y <= endY; ++y)
            setTile(x, y, TileType::FLOOR);
    }

    // --- Visibility: reveal tiles around a point ---
    void revealAround(int cx, int cy, int radius)
    {
        for (int dy = -radius; dy <= radius; ++dy)
            for (int dx = -radius; dx <= radius; ++dx)
                if (inBounds(cx + dx, cy + dy))
                    grid[cy + dy][cx + dx].setVisible(true);
    }

    // --- Getters ---
    int getWidth()  const { return width; }
    int getHeight() const { return height; }
};
