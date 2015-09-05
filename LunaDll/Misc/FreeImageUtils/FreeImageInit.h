#ifndef FreeImageInit_hhhhh
#define FreeImageInit_hhhhh
class FreeImageInit
{
private:
    bool m_isInit;
public:
    FreeImageInit();
    void init();
    inline bool isInit() { return m_isInit; }
    ~FreeImageInit();
};
#endif

