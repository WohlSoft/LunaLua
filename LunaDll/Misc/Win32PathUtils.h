#if !defined(_MISC_WIN32PATHUTILS_H_)
#define _MISC_WIN32PATHUTILS_H_

#include <cstddef>
#include <string>

template <typename T>
T normalizeToBackslashAndResolvePath(const T& path)
{
    size_t len = path.size();
    T ret;
    ret.resize(len);
    size_t outIdx = 0;
    size_t idx = 0;
    while (true)
    {
        // Detect if we're now ending a .. segment
        if ((outIdx >= 3) && ((idx >= len) || ((idx < len) && ((path[idx] == '\\') || (path[idx] == '/')))) &&
            (ret[outIdx - 3] == '\\') &&
            (ret[outIdx - 2] == '.') &&
            (ret[outIdx - 1] == '.')
            )
        {
            // Remove the prior segment from output
            outIdx -= 3;
            for (; outIdx > 0; outIdx--)
            {
                if (ret[outIdx - 1] == '\\') break;
            }
            idx++;
        }

        // End of string
        if (idx >= len) break;

        T::value_type c = path[idx];
        if (c == '/')
        {
            c = '\\';
        }
        if ((c == '\\') &&
            (outIdx > 0) && (ret[outIdx - 1] == '\\')
            )
        {
            idx++;
            continue;
        }
        ret[outIdx++] = c;
        idx++;
    }
    ret.resize(outIdx);
    return ret;
}

template <typename T>
class NormalizedPath
{
private:
    T mData;
public:
    // Default constructor
    NormalizedPath() :
        mData()
    {
    }

    // Copy consturctor
    NormalizedPath(const NormalizedPath<T>& input) :
        mData(input.mData)
    {
    }

    // Construct from string
    NormalizedPath(const T& input) :
        mData(normalizeToBackslashAndResolvePath(input))
    {
    }

    // Construct from C string
    NormalizedPath(const typename T::value_type* input) :
        mData(normalizeToBackslashAndResolvePath(T(input)))
    {
    }

    // Assign from string
    NormalizedPath<T>& operator= (const T& input)
    {
        mData = normalizeToBackslashAndResolvePath(input);
        return *this;
    }

    // Assign from C string
    NormalizedPath<T>& operator= (const typename T::value_type* input)
    {
        mData = normalizeToBackslashAndResolvePath(T(input));
        return *this;
    }

    // Append from string
    NormalizedPath<T>& operator+= (const T& input)
    {
        mData = normalizeToBackslashAndResolvePath(mData + input);
        return *this;
    }

    // Append from C string
    NormalizedPath<T>& operator+= (const typename T::value_type* input)
    {
        mData = normalizeToBackslashAndResolvePath(mData + input);
        return *this;
    }

    // Destructor
    ~NormalizedPath()
    {
    }

    // Type cast operator
    operator T() const { return mData; }

    // Other string-like operators
    const typename T::value_type* c_str() const { return mData.c_str(); }
    const T& str() const { return mData; }
    const T& str_lower() const { return mDataLower; }
};

bool isAbsolutePath(const std::wstring& path);
bool isAbsolutePath(const std::string& path);

extern void removeFilePathW(std::wstring &path);
extern void removeFilePathW(wchar_t*path, int length);
void removeFilePathA(std::string &path);
void removeFilePathA(char*path, int length);

#endif // !defined(_MISC_WIN32PATHUTILS_H_)
