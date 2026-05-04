#ifndef RAIIHandle_hhh
#define RAIIHandle_hhh

#include <Windows.h>

class RAIIHandle {
    HANDLE h;

public:
    // Close the handle upon destruction
    ~RAIIHandle() {
        close();
    }

    // Initialize with invalid handle
    RAIIHandle() : h(INVALID_HANDLE_VALUE) {}

    // No copy constructor
    RAIIHandle(RAIIHandle const&) = delete;

    // Move constructor
    RAIIHandle(RAIIHandle&& that) : h(that.h) {
        // Set the handle of that to INVALID_HANDLE_VALUE to avoid it getting closed
        that.h = INVALID_HANDLE_VALUE;
    }

    // Construct from handle
    RAIIHandle(HANDLE handle) : h(handle) {}

    // No copy assignment
    RAIIHandle& operator=(RAIIHandle const&) = delete;

    // Move assignment
    RAIIHandle& operator=(RAIIHandle&& that) {
        *this = that.h;

        that.h = INVALID_HANDLE_VALUE;

        return *this;
    }

    // Assign handle
    RAIIHandle& operator=(HANDLE handle) {
        // Close current handle
        close();

        // Assign new handle
        h = handle;

        return *this;
    }

    // Get mutable reference to handle
    HANDLE& getHandleRef() {
        return h;
    }

    // Get const reference to handle
    HANDLE const& getHandleRef() const {
        return h;
    }

    // Get handle without taking ownership
    HANDLE borrow() const {
        return h;
    }

    // Get handle ownership
    HANDLE takeOwnership() {
        HANDLE handle = h;
        h = INVALID_HANDLE_VALUE;
        return handle;
    }

    // Check if the handle is valid
    bool isValid() const {
        return h != INVALID_HANDLE_VALUE;
    }
    
    // Close handle
    void close() {
        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
        }
        h = INVALID_HANDLE_VALUE;
    }
};

#endif