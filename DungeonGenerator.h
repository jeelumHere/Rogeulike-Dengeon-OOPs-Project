// ============================================================
//  DungeonGenerator.h  –  Procedural Dungeon Builder
//  Demonstrates: Abstraction (hides messy algorithm),
//                Encapsulation (clients call generate() only)
//
//  Algorithm:
//  1. Fill map with walls
//  2. Place random rectangular rooms
//  3. Connect each room to the previous with corridors
//  4. Place stairs somewhere on the last room
// ============================================================

#pragma once
#include "DungeonMap.h"
#include "Position.h"
#include <vector>
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()

// Simple struct to remember where each room is
struct Room
{
    int x, y;        // Top-left corner
    int width;
    int height;

    // Center of this room
    Position center() const
    {
        return Position(x + width / 2, y + height / 2);
    }
};

class DungeonGenerator
{
private:
    std::vector<Room> rooms;  // All rooms we placed this run

    // Check if a new room overlaps any existing room (with 1-tile gap)
    bool overlaps(const Room& newRoom) const
    {
        for (const Room& r : rooms)
        {
            // Add 1 padding so rooms don't touch each other
            if (newRoom.x < r.x + r.width  + 1 &&
                newRoom.x + newRoom.width  + 1 > r.x &&
                newRoom.y < r.y + r.height + 1 &&
                newRoom.y + newRoom.height + 1 > r.y)
            {
                return true;   // They overlap
            }
        }
        return false;
    }

public:
    // --- Main Generation Function ---
    // Clients only ever call this. The messy details stay inside.
    DungeonMap generate(int mapWidth, int mapHeight,
                        int numRooms, unsigned int seed)
    {
        srand(seed);   // Seed RNG so same seed = same dungeon
        rooms.clear();

        DungeonMap map(mapWidth, mapHeight);  // Starts all walls

        // Try to place numRooms rooms
        for (int attempt = 0; attempt < numRooms * 10; ++attempt)
        {
            if ((int)rooms.size() >= numRooms) break;

            // Random room dimensions (5-12 wide, 4-8 tall)
            Room r;
            r.width  = 5 + rand() % 8;
            r.height = 4 + rand() % 5;
            r.x      = 1 + rand() % (mapWidth  - r.width  - 2);
            r.y      = 1 + rand() % (mapHeight - r.height - 2);

            if (!overlaps(r))
            {
                // Carve this room into the map
                map.carveRoom(r.x, r.y, r.width, r.height);
                rooms.push_back(r);

                // Connect to the previous room with a corridor
                if (rooms.size() > 1)
                {
                    Position prev = rooms[rooms.size()-2].center();
                    Position curr = r.center();

                    // 50/50: go horizontal-then-vertical OR vertical-then-horizontal
                    if (rand() % 2 == 0)
                    {
                        map.carveHorizontalCorridor(prev.x, curr.x, prev.y);
                        map.carveVerticalCorridor(curr.x, prev.y, curr.y);
                    }
                    else
                    {
                        map.carveVerticalCorridor(prev.x, prev.y, curr.y);
                        map.carveHorizontalCorridor(prev.x, curr.x, curr.y);
                    }
                }
            }
        }

        // Place stairs in the last room placed
        if (!rooms.empty())
        {
            Position stairPos = rooms.back().center();
            map.setTile(stairPos.x, stairPos.y, TileType::STAIRS_DOWN);
        }

        return map;   // Return the fully built dungeon
    }

    // --- Spawn position = center of the first room ---
    Position getPlayerStart() const
    {
        if (rooms.empty()) return Position(1, 1);
        return rooms.front().center();
    }

    // --- Random floor position inside a random room ---
    Position getRandomFloorPos() const
    {
        if (rooms.empty()) return Position(2, 2);
        const Room& r = rooms[rand() % rooms.size()];
        int x = r.x + 1 + rand() % (r.width  - 2);
        int y = r.y + 1 + rand() % (r.height - 2);
        return Position(x, y);
    }

    // --- Number of rooms actually placed ---
    int getRoomCount() const { return (int)rooms.size(); }
};
