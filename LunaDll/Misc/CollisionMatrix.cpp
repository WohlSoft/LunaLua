#include "CollisionMatrix.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include "../../Globals.h"

CollisionMatrix gCollisionMatrix("onGroupDeallocationInternal");

CollisionMatrix::CollisionMatrix(char const* ev) :
    matrix(),
    deallocated_groups(),
    next_group(1),
    reference_count(),
    deallocation_event_name(ev)
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
}

unsigned int CollisionMatrix::allocateIndex() {
    unsigned int new_group;

    if (deallocated_groups.empty()) { // If there's no available deallocated group index, create a new one.
        new_group = next_group;
        next_group++;

        reference_count.push_back(0u);
    } else { // Otherwise, take one from the set.
        new_group = deallocated_groups.top();
        deallocated_groups.pop();

        // No need to set the reference count to 0 since a deallocated group index always has a reference count of 0.
    }

    return new_group;
}

void CollisionMatrix::incrementReferenceCount(unsigned int group) {
    if (group != 0) { // We don't use reference counting for the default group index
        reference_count[group - 1]++;
    }
}

void CollisionMatrix::decrementReferenceCount(unsigned int group) {
    if (group != 0) { // We don't use reference counting for the default group index
        reference_count[group - 1]--;

        if (reference_count[group - 1] == 0) {
            deallocateGroup(group);
        }
    }
}


bool CollisionMatrix::getIndicesCollide(unsigned int i, unsigned int j) {
    unsigned int min, max;
    std::tie(min, max) = std::minmax(i, j);

    // If matrix[max] contains min, returns !default_behavior(min, max). Otherwise, return default_behavior(min, max).
    return (max < matrix.size() && matrix[max].count(min) == 1) != defaultBehavior(min, max);
}

void CollisionMatrix::setIndicesCollide(unsigned int i, unsigned int j, bool collide) {
    unsigned int min, max;
    std::tie(min, max) = std::minmax(i, j);

    if (max >= matrix.size()) { // If the matrix is too small to contain (min, max)
        if (collide != defaultBehavior(min, max)) { // If element (min, max) of the matrix is modified
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
        bool default_collide = defaultBehavior(min, max); // Get default behavior

        if (contains && (collide == default_collide)) { // If we have to remove collision group index min from set max
            // Remove it
            matrix[max].erase(current_collide_iter);

            // Update reference counts
            decrementReferenceCount(min);
            decrementReferenceCount(max);

            // Cleanup collision matrix
            cleanupMatrix();
        } else if (!contains && (collide != default_collide)) { // If we have to insert collision group index min in set max
            // Insert it
            matrix[max].insert(min);

            // Update reference counts
            incrementReferenceCount(min);
            incrementReferenceCount(max);
        }
    }
}

void CollisionMatrix::cleanupMatrix() {
    // Search last nonempty group index
    unsigned int last_nonempty_group = matrix.size() - 1;
    for (; last_nonempty_group < matrix.size() && matrix[last_nonempty_group].empty(); last_nonempty_group--) {}

    // Resize collision matrix
    matrix.resize(last_nonempty_group + 1);
}

void CollisionMatrix::deallocateGroup(unsigned int group) {
    // Call lunalua event
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> deallocation_event = std::make_shared<Event>(deallocation_event_name, false);
        deallocation_event->setDirectEventName(deallocation_event_name);
        deallocation_event->setLoopable(false);
        gLunaLua.callEvent(deallocation_event, group);
    }

    // Put group index in deallocated group indices
    deallocated_groups.push(group);
}

bool CollisionMatrix::defaultBehavior(unsigned int i, unsigned int j) const {
    return i == 0 || i != j;
}