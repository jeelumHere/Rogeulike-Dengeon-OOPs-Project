// ============================================================
//  Position.h  –  2D Coordinate
//  Stores an x,y position on the dungeon map.
//  Demonstrates: Operator Overloading
// ============================================================

#pragma once   // Prevents this file from being included twice

struct Position
{
    int x;   // Column (left-right)
    int y;   // Row    (up-down)

    // Default constructor – starts at (0,0)
    Position() : x(0), y(0) {}

    // Constructor that lets us write  Position p(3, 5);
    Position(int x, int y) : x(x), y(y) {}

    // --- Operator Overloading ---
    // Lets us write:  pos1 + pos2   instead of   Position(pos1.x+pos2.x, ...)
    Position operator+(const Position& other) const
    {
        return Position(x + other.x, y + other.y);
    }

    // Lets us write:  pos1 == pos2
    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y;
    }

    // Lets us write:  pos1 != pos2
    bool operator!=(const Position& other) const
    {
        return !(*this == other);
    }
};
