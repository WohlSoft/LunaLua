#include "CollisionMatrix.h"
#include <algorithm>
#include <cstring>
#include <tuple>
#include "../../Globals.h"

CollisionMatrix gCollisionMatrix;

// === PRIVATE METHODS ===

CollisionMatrix::GroupOrIndex::GroupOrIndex(unsigned int i): index(i), is_allocated(true) {}

CollisionMatrix::GroupOrIndex::GroupOrIndex(char const* n): name(n), is_allocated(false) {}

bool CollisionMatrix::GroupOrIndex::isAllocated() {
    return is_allocated;
}

unsigned int CollisionMatrix::GroupOrIndex::getIndex() {
    return index;
}

char const* CollisionMatrix::GroupOrIndex::getGroup() {
    return name;
}

void CollisionMatrix::cleanupMatrix() { 
    // Search last nonempty group index
    unsigned int last_nonempty_group = matrix.size() - 1;
    for (; last_nonempty_group < matrix.size() && matrix[last_nonempty_group].empty(); last_nonempty_group--) {}

    // Resize collision matrix
    matrix.resize(last_nonempty_group + 1);
}

unsigned int CollisionMatrix::allocateGroupIndex(char const* collisionGroup) {
    //printBoxA("Entering allocateGroupIndex(collisionGroup = '%s')", collisionGroup);
    unsigned int new_group;

    if (deallocated_groups.empty()) { // If there's no available deallocated group index, create a new one.
        //printBoxA("No available unallocated index");
        new_group = next_group;
        next_group++;

        reference_count.push_back(0u);
        index_to_string.push_back(collisionGroup);

        //printBoxA("index_to_string.size() = %u, new_group = %u", index_to_string.size(), new_group);
    } else { // Otherwise, take one from the set.
        //printBoxA("Unallocated index available");
        new_group = deallocated_groups.top();
        deallocated_groups.pop();

        // No need to set the reference count to 0 since a deallocated group index always has a reference count of 0.
        index_to_string[new_group - 1] = collisionGroup;
    }
    //printBoxA("string_to_index['%s'] = %u", index_to_string[new_group - 1].c_str(), new_group);
    string_to_index[index_to_string[new_group - 1]] = new_group;

    //printBoxA("Allocated index %u for group '%s'", new_group, collisionGroup);

    return new_group;
}

CollisionMatrix::GroupOrIndex CollisionMatrix::tryGetGroupIndex(char const* collisionGroup) {
    //printBoxA("Entering tryGetGroupIndex(collisionGroup = '%s')", collisionGroup);
    if (*collisionGroup == '\0') { // Check for empty string
        //printBoxA("Empty string, return index 0");
        return GroupOrIndex(0u);
    }

    // Try to search for an already allocated index
    auto foundIndex = string_to_index.find(collisionGroup);

    // Return it if it was found
    if (foundIndex != string_to_index.end()) {
        //printBoxA("Found index %u associated to '%s' in string_to_index", foundIndex->second, collisionGroup);
        return GroupOrIndex(foundIndex->second);
    }

    //printBoxA("Didn't find any index associated to '%s' in string_to_index", collisionGroup);
    // Return collision group
    return GroupOrIndex(collisionGroup);
}

unsigned int CollisionMatrix::getOrAllocateGroupIndex(char const* collisionGroup) {
    GroupOrIndex resolved = tryGetGroupIndex(collisionGroup);

    if (resolved.isAllocated()) {
        return resolved.getIndex();
    } else {
        return allocateGroupIndex(resolved.getGroup());
    }
}

void CollisionMatrix::deallocateGroupIndex(unsigned int groupIndex) {
    // Put group index in deallocated group indices
    deallocated_groups.push(groupIndex);

    // Remove name <=> index mapping for this group
    string_to_index.erase(index_to_string[groupIndex - 1]);
    index_to_string[groupIndex - 1].clear();
}

void CollisionMatrix::incrementReferenceCount(unsigned int groupIndex) {
    if (groupIndex != 0) { // We don't use reference counting for the default group index
        reference_count[groupIndex - 1]++;
    }
}

bool CollisionMatrix::defaultBehavior(GroupOrIndex i, GroupOrIndex j) const {
    if (i.isAllocated() && j.isAllocated()) {
        // Both groups are allocated, perform the check on the collision group indices
        return defaultBehavior(i.getIndex(), j.getIndex());
    } else if (!i.isAllocated() && !j.isAllocated()) {
        // No groups are allocated, perform the check on the collision group strings
        return defaultBehavior(i.getGroup(), j.getGroup());
    } else {
        // Only one of the two collision groups are allocated. Therefore, i and j must be different.
        // Two distinct collision groups always collide with eachother by default.
        return true;
    }   
}

bool CollisionMatrix::defaultBehavior(unsigned int i, unsigned int j) const {
    return i == 0 || i != j;
}

bool CollisionMatrix::defaultBehavior(char const* i, char const* j) const {
    return *i == '\0' || std::strcmp(i, j) != 0;
}

// === PUBLIC METHODS ===

CollisionMatrix::CollisionMatrix() :
    matrix(),
    deallocated_groups(),
    next_group(1),
    reference_count(),
    string_to_index(),
    index_to_string()
{}

void CollisionMatrix::clear() {
    // Clear collision matrix
    matrix.clear();
    
    // Clear deallocated group indices set
    deallocated_groups = queue_type();

    // Default group index (0) is allocated by default so the next one to be allocated is group index 1
    next_group = 1;

    // Clear reference count vector
    reference_count.clear();

    // Clear name <=> index mapping
    string_to_index.clear();
    index_to_string.clear();
}

std::string const& CollisionMatrix::getGroupFromIndex(unsigned int groupIndex) {
    // Empty string
    static const std::string empty;

    if (groupIndex == 0) {
        return empty;
    }
    return index_to_string[groupIndex - 1];
}

unsigned int CollisionMatrix::assignGroup(unsigned int previousGroupIndex, char const* newGroup) {
    //printBoxA("Entering assignGroup(previousGroupIndex = %u, newGroup = '%s')", previousGroupIndex, newGroup);
    unsigned int newIndex = getOrAllocateGroupIndex(newGroup);
    //printBoxA("Index for '%s' is %u", newGroup, newIndex);

    if (newIndex != previousGroupIndex) {
        incrementReferenceCount(newIndex);
        decrementReferenceCount(previousGroupIndex);
    }
    return newIndex;
}

void CollisionMatrix::decrementReferenceCount(unsigned int groupIndex) {
    if (groupIndex != 0) { // We don't use reference counting for the default group index
        reference_count[groupIndex - 1]--;

        if (reference_count[groupIndex - 1] == 0) {
            deallocateGroupIndex(groupIndex);
        }
    }
}

bool CollisionMatrix::getGroupsCollide(char const* i, char const* j) {
    //printBoxA("Entering getGroupsCollide(i = '%s', j = '%s')", i, j);

    // Try to get the collision group indices
    GroupOrIndex iResolved = tryGetGroupIndex(i);
    GroupOrIndex jResolved = tryGetGroupIndex(j);

    //printBoxA("Resolved collision groups: '%s' -> %u, '%s' -> %u", i, iResolved.getGroup(), j, jResolved.getGroup());

    if (iResolved.isAllocated() && jResolved.isAllocated()) {
        //printBoxA("Two groups allocated, int check");
        // Both groups are allocated, perform the check on the collision group indices
        return getGroupsCollide(iResolved.getIndex(), jResolved.getIndex());
    } else if (!iResolved.isAllocated() && !jResolved.isAllocated()) {
        //printBoxA("Zero groups allocated, int check");
        // No groups are allocated, fallback to default behavior
        return defaultBehavior(iResolved.getGroup(), jResolved.getGroup());
    } else {
        //printBoxA("One group allocated, return true");
        // Only one of the two collision groups are allocated. Therefore, i and j must be different.
        // Furthermore (i, j) is not in the collision matrix, so their collision behavior must be the default one.
        // Two distinct collision groups always collide with eachother by default.
        return true;
    }   
}

bool CollisionMatrix::getGroupsCollide(unsigned int i, char const* j) {
    // Try to get the collision group index of j
    GroupOrIndex jResolved = tryGetGroupIndex(j);

    if (jResolved.isAllocated()) {
        // Both groups are allocated, perform the check on the collision group indices
        return getGroupsCollide(i, jResolved.getIndex());
    } else {
        // Only one of the two collision groups are allocated. Therefore, i and j must be different.
        // Furthermore (i, j) is not in the collision matrix, so their collision behavior must be the default one.
        // Two distinct collision groups always collide with eachother by default.
        return true;
    }   
}

bool CollisionMatrix::getGroupsCollide(unsigned int i, unsigned int j) {
    unsigned int min, max;
    std::tie(min, max) = std::minmax(i, j);

    // If matrix[max] contains min, returns !default_behavior(min, max). Otherwise, return default_behavior(min, max).
    return (max < matrix.size() && matrix[max].count(min) == 1) != defaultBehavior(min, max);
}

void CollisionMatrix::setGroupsCollide(char const* i, char const* j, bool collide) {
    //printBoxA("Entering setGroupsCollide(i = '%s', j = '%s', collide = %u)", i, j, collide);

    // Try to get the collision group indices
    GroupOrIndex iResolved = tryGetGroupIndex(i);
    GroupOrIndex jResolved = tryGetGroupIndex(j);

    //printBoxA("Resolved collision groups: '%s' -> %u, '%s' -> %u", i, iResolved.getGroup(), j, jResolved.getGroup());

    // Get default behavior for collision group pair
    bool default_collide = defaultBehavior(iResolved, jResolved);

    //printBoxA("Default behavior: ('%s', '%s') -> %u", i, j, default_collide);

    // Return prematurely if at least one group isn't allocated and we don't modify the default value, avoids needless allocations
    if ((!iResolved.isAllocated() || !jResolved.isAllocated()) && (collide == default_collide)) {
        return;
    }

    // Get or allocate collision group indices
    unsigned int iIndex = getOrAllocateGroupIndex(i);
    unsigned int jIndex = getOrAllocateGroupIndex(j);

    //printBoxA("Final indices: '%s' -> %u, '%s' -> %u", i, iIndex, j, jIndex);

    unsigned int min, max;
    std::tie(min, max) = std::minmax(iIndex, jIndex);

    if (max >= matrix.size()) { // If the matrix is too small to contain (min, max)
        if (collide != default_collide) { // If element (min, max) of the matrix is modified
            //printBoxA("Matrix too small, inserted pair (%u, %u)", min, max);

            // Resize the matrix
            matrix.resize(max + 1);

            // Insert (min, max) in the matrix
            matrix[max].insert(min);

            // Update reference counts
            incrementReferenceCount(min);
            incrementReferenceCount(max);
        }
    } else {
        auto current_collide_iter = matrix[max].find(min); // Search collision group index min in set max
        bool contains = current_collide_iter != matrix[max].end(); // Check whether the previous search has succeeded or not

        if (contains && (collide == default_collide)) { // If we have to remove collision group index min from set max
            //printBoxA("Matrix large enough, removed pair (%u, %u)", min, max);

            // Remove it
            matrix[max].erase(current_collide_iter);

            // Update reference counts
            decrementReferenceCount(min);
            decrementReferenceCount(max);

            // Cleanup collision matrix
            cleanupMatrix();
        } else if (!contains && (collide != default_collide)) { // If we have to insert collision group index min in set max
            //printBoxA("Matrix large enough, inserted pair (%u, %u)", min, max);

            // Insert it
            matrix[max].insert(min);

            // Update reference counts
            incrementReferenceCount(min);
            incrementReferenceCount(max);
        }
    }
}