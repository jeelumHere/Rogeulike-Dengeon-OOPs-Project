// ============================================================
//  Inventory.h  –  Generic Item Container (Template)
//  Demonstrates: Templates (generic programming)
//
//  A template lets ONE class work for ANY item type.
//  Inventory<Weapon> stores weapons.
//  Inventory<Item>   stores any item.
//  No code duplication!
// ============================================================

#pragma once
#include <vector>
#include <memory>   // for unique_ptr
#include "Item.h"

template <typename T>   // T = the type of item to store
class Inventory
{
private:
    // vector of unique_ptr = automatic memory management
    // unique_ptr deletes items when inventory is destroyed
    std::vector<std::unique_ptr<T>> items;

    int maxCapacity;   // Max number of items we can carry

public:
    // Create inventory with a maximum slot count
    Inventory(int cap = 10) : maxCapacity(cap) {}

    // --- Add an item (takes ownership of the raw pointer) ---
    bool addItem(T* item)
    {
        if (isFull())
            return false;   // Cannot add – bag is full
        items.push_back(std::unique_ptr<T>(item));
        return true;
    }

    // --- Remove item at a position (0-based index) ---
    // Returns raw pointer so caller can use it; caller takes ownership
    T* removeItem(int index)
    {
        if (index < 0 || index >= (int)items.size())
            return nullptr;

        T* raw = items[index].release();  // Release ownership
        items.erase(items.begin() + index);
        return raw;                       // Caller now owns it
    }

    // --- Look at an item without removing it ---
    T* getItem(int index) const
    {
        if (index < 0 || index >= (int)items.size())
            return nullptr;
        return items[index].get();
    }

    // --- Utility ---
    int  size()     const { return (int)items.size(); }
    bool isEmpty()  const { return items.empty(); }
    bool isFull()   const { return (int)items.size() >= maxCapacity; }
    int  getCapacity() const { return maxCapacity; }

    // Find item by name – returns index, or -1 if not found
    int findByName(const std::string& name) const
    {
        for (int i = 0; i < (int)items.size(); ++i)
            if (items[i]->getName() == name)
                return i;
        return -1;
    }
};
