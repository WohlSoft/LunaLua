#ifndef HardcodedGraphicsManager_hhh
#define HardcodedGraphicsManager_hhh

#include <string>
#include <map>
class INIReader;

class HardcodedGraphicsManager
{
public:
    HardcodedGraphicsManager();
    ~HardcodedGraphicsManager();

    
    //************************************
    // Method:    patchGraphics
    // FullName:  HardcodedGraphicsManager::patchGraphics
    // Access:    public 
    // Returns:   int -1 if Successful, -2 If file not found or the size of the file if too big to be patched.
    // Qualifier:
    // Parameter: void * offset The memory offset from the beginning of the EXE file.
    // Parameter: char * filepath The image file to patch
    // Parameter: int sizeOfFile The size of the original image memory segment
    //************************************
    static int patchGraphics(unsigned int offset_i, const char* filepath);

    static void loadGraphics();
    static void buildMap();

private:
        static std::map<unsigned int, int> hardcoded_data_map;

        static std::string root;
        static void loadIniImage(unsigned int hex, unsigned int hex_m, INIReader &ini, std::string sct, std::string value);
};

#endif
