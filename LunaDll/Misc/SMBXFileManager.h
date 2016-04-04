#ifndef SMBXFileManager_hhhhh
#define SMBXFileManager_hhhhh

#include <string>

// This interface is the bridge which manages our data
struct SMBXDataInterface {

};



class SMBXLevelFileBase {
public:
    SMBXLevelFileBase();

    // Common Level File Function
    void ReadFile(const std::wstring& path);
    void ReadFile();


    // Status
    inline bool isValid() const { return m_isValid; }

protected:
    bool m_isValid;
};


#endif
