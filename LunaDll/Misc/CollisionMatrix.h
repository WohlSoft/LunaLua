#ifndef CollisionMatrix_hhh
#define CollisionMatrix_hhh

#include <unordered_map>

#define COLLISION_GROUP_STRING_LENGTH 32
constexpr std::size_t collisionGroupStringLength = COLLISION_GROUP_STRING_LENGTH;

class GroupPair {
    char firstGroup[collisionGroupStringLength];
    char secondGroup[collisionGroupStringLength];

public:
    GroupPair(char const first[collisionGroupStringLength], char const second[collisionGroupStringLength]);

    constexpr char const* first() const {
        return firstGroup;
    }

    constexpr char const* second() const {
        return secondGroup;
    }

    bool operator==(GroupPair const& that) const;
    
    inline bool operator!=(GroupPair const& that) const {
        return !(*this == that);
    }
};

namespace std {
    template<>
    struct hash<GroupPair> {
        std::size_t operator()(GroupPair const& value) const;
    };
}

class CollisionMatrix {
    std::unordered_map<GroupPair, bool> matrix;

public:
    CollisionMatrix() = default;

    inline void clear() {
        matrix.clear();
    }

    bool getGroupsCollide(char const firstGroup[collisionGroupStringLength], char const secondGroup[collisionGroupStringLength]);
    void setGroupsCollide(char const firstGroup[collisionGroupStringLength], char const secondGroup[collisionGroupStringLength], bool collide);
};

extern CollisionMatrix gCollisionMatrix;

#endif