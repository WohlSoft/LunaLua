#include "CollisionMatrix.h"
#include <windows.h>
#include <cstring>
#include "GlobalFuncs.h"
#include <algorithm>

CollisionMatrix gCollisionMatrix;

GroupPair::GroupPair(char const first[collisionGroupStringLength], char const second[collisionGroupStringLength]) {

    if (std::strncmp(first, second, collisionGroupStringLength) < 0) {
        std::strncpy(this->firstGroup, first, collisionGroupStringLength);
        std::strncpy(this->secondGroup, second, collisionGroupStringLength);
    } else {
        std::strncpy(this->firstGroup, second, collisionGroupStringLength);
        std::strncpy(this->secondGroup, first, collisionGroupStringLength);
    }
}

std::size_t std::hash<GroupPair>::operator()(GroupPair const& value) const {
    return combineHash(hashString(value.first()), hashString(value.second()));
}

bool GroupPair::operator==(GroupPair const& that) const {
    return std::strncmp(this->firstGroup, that.firstGroup, collisionGroupStringLength) == 0
        && std::strncmp(this->secondGroup, that.secondGroup, collisionGroupStringLength) == 0;
}

bool CollisionMatrix::getGroupsCollide(char const firstGroup[collisionGroupStringLength], char const secondGroup[collisionGroupStringLength]) {
    GroupPair pair(firstGroup, secondGroup);
    auto search = matrix.find(pair);

    // If there's already a value corresponding to the pair in our hashmap, return it.
    if (search != matrix.end()) {
        return search->second;
    }

    // Otherwise, fallback to the default behavior and cache the result.
    bool collide = firstGroup[0] == '\0' || strncmp(firstGroup, secondGroup, collisionGroupStringLength) != 0;
    matrix[pair] = collide;

    return collide;
}

void CollisionMatrix::setGroupsCollide(char const firstGroup[collisionGroupStringLength], char const secondGroup[collisionGroupStringLength], bool collide) {
    GroupPair pair(firstGroup, secondGroup);

    matrix[pair] = collide;
}