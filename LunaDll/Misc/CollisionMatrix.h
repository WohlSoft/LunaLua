#ifndef CollisionMatrix_hhh
#define CollisionMatrix_hhh

#include <functional>
#include <queue>
#include <unordered_set>

class CollisionMatrix {
    using fun_type = bool (*)(unsigned int i, unsigned int j);
    using queue_type = std::priority_queue<unsigned int, std::priority_queue<unsigned int>::container_type, std::greater<unsigned int>>;

    std::vector<std::unordered_set<unsigned int>> matrix; // The collision matrix itself. matrix[j].count(i) == 1 if getGroupsCollide(i, j) != default_behavior(i, j).

    queue_type deallocated_groups; // Contains all currently deallocated groups
    unsigned int next_group; // The next group to be allocated if deallocated_groups is empty

    std::vector<unsigned int> reference_count; // How many references to the group exist. Index 0 corresponds to group 1

    fun_type default_behavior; // The default behavior of the collision matrix

    char const* deallocation_event_name; // The lunalua event which is called whenever a group is deallocated
    
    void cleanupMatrix(); // Removes all trailing empty sets from the collision matrix
    void deallocateGroup(unsigned int group); // Deallocates a collision group

public:
    CollisionMatrix() = delete;
    CollisionMatrix(fun_type default_behavior, char const* deallocation_event_name);

    void clear(); // Resets this collision matrix
    unsigned int allocateIndex(); // Allocates a new collision group index
    void incrementReferenceCount(unsigned int group); // Increments the reference count of a group index
    void decrementReferenceCount(unsigned int group); // Decrements the reference count of a group index
    bool getIndicesCollide(unsigned int i, unsigned int j); // Reads the collision matrix
    void setIndicesCollide(unsigned int i, unsigned int j, bool collide); // Writes to the collision matrix

};

extern CollisionMatrix gCollisionMatrix;

#endif
