#ifndef CollisionMatrix_hhh
#define CollisionMatrix_hhh

#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CollisionMatrix {
    // Forward declaration
    class GroupOrIndex;

    // The collision matrix itself. matrix[j].count(i) == 1 if getGroupsCollide(i, j) != default_behavior(i, j).
    std::vector<std::unordered_set<unsigned int>> matrix; 

    // The type of `deallocated_groups`
    using queue_type = std::priority_queue<unsigned int, std::priority_queue<unsigned int>::container_type, std::greater<unsigned int>>;

    // Contains all currently deallocated groups
    queue_type deallocated_groups;

    // The next group to be allocated if deallocated_groups is empty
    unsigned int next_group;

    // How many references to the group exist. Index 0 corresponds to group 1
    std::vector<unsigned int> reference_count;

    // Maps collision groups to their respective collision group indices. Doesn't contain the default collision group
    std::unordered_map<std::string, unsigned int> string_to_index;

    // Maps collision group indices to their respective collision groups. Index 0 corresponds to group 1
    std::vector<std::string> index_to_string;

    // Removes all trailing empty sets from the collision matrix
    void cleanupMatrix();

    // Allocates a new collision group index
    unsigned int allocateGroupIndex(char const* collisionGroup);

    // Tries to get the collision group index of a collision group
    GroupOrIndex tryGetGroupIndex(char const* collisionGroup);

    // Auxiliary class for representing a possibly allocated collision group
    class GroupOrIndex {
        friend GroupOrIndex CollisionMatrix::tryGetGroupIndex(char const* collisionGroup);

        union {
            unsigned int index;
            char const* name;
        };
        bool is_allocated;

        explicit GroupOrIndex(unsigned int i);
        explicit GroupOrIndex(char const* n);

    public:
        bool isAllocated();
        unsigned int getIndex();
        char const* getGroup();
    };


    // Gets the collision group index of a collision group, allocates it if it's not
    unsigned int getOrAllocateGroupIndex(char const* collisionGroup);

    // Deallocates a collision group
    void deallocateGroupIndex(unsigned int groupIndex);

     // Increments the reference count of a group index
    void incrementReferenceCount(unsigned int groupIndex); 

    // The default behavior of the collision matrix
    bool defaultBehavior(GroupOrIndex i, GroupOrIndex j) const;
    bool defaultBehavior(unsigned int i, unsigned int j) const;
    bool defaultBehavior(char const* i, char const* j) const;

public:
    CollisionMatrix();

    // Resets this collision matrix
    void clear();

    // Returns the collision group of a collision group index
    std::string const& getGroupFromIndex(unsigned int groupIndex);

    // Handles reference counting when a collision group field is assigned a new value, returns the collision group index of `newGroup`
    unsigned int assignGroup(unsigned int previousGroupIndex, char const* newGroup);

    // Decrements the reference count of a group index
    void decrementReferenceCount(unsigned int groupIndex);

    // Reads the collision matrix
    bool getGroupsCollide(char const* i, char const* j);
    bool getGroupsCollide(unsigned int i, char const* j);
    bool getGroupsCollide(unsigned int i, unsigned int j);

    // Writes to the collision matrix
    void setGroupsCollide(char const* i, char const* j, bool collide);

};

// Global collision matrix
extern CollisionMatrix gCollisionMatrix;

#endif
