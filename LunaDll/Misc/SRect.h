#ifndef SRECT_H_
#define SRECT_H_

template <typename T>
class SRect {
public:
    T x1;
    T y1;
    T x2;
    T y2;
public:
    // Constructs a rect based on x1, y1, x2, and y2.
    // Note: If x1 >= x2 or y1 >= y2 is true, this rect will be considered to
    //       empty.
    SRect(T _x1, T _y1, T _x2, T _y2) :
        x1(_x1), y1(_y1), x2(_x2), y2(_y2)
    {}

    // Returns the corrosponding rect based on x, y, width, and height.
    static inline SRect<T> fromXYWH(T x, T y, T w, T h)
    {
        return SRect<T>(x, y, x + w, y + h);
    }

    // Checks if this rect is empty
    inline bool isEmpty() const {
        return (x1 >= x2) || (y1 >= y2);
    }

    // Note, this may return rectangles with x1 > x2 or y1 > y2, which
    // represent no intersection. To detect this use isEmpty()
    inline SRect<T> intersection(const SRect<T>& other) const
    {
        return SRect<T>(
            (x1 >= other.x1) ? x1 : other.x1,
            (y1 >= other.y1) ? y1 : other.y1,
            (x2 <= other.x2) ? x2 : other.x2,
            (y2 <= other.y2) ? y2 : other.y2
            );
    }

    // Given 'this' is a projection of o1, and o2 is a subset of o1, return the
    // subset of 'this' corrosponding to o2
    // Note: Only really 'safe' for floating point types
    inline SRect<T> shrinkProportionately(const SRect<T>& o1, const SRect<T>& o2) const
    {
        // If any input is empty, return empty
        if (isEmpty() || o1.isEmpty() || o2.isEmpty()) return SRect<T>(0, 0, 0, 0);

        T thisW  = x2 - x1;
        T thisH  = y2 - y1;
        T o1W    = o1.x2 - o1.x1;
        T o1H    = o1.y2 - o1.y1;
        T left   = o2.x1 - o1.x1;
        T right  = o1.x2 - o2.x2;
        T top    = o2.y1 - o1.y1;
        T bottom = o1.y2 - o2.y2;

        return SRect<T>(
            x1 + left   * thisW / o1W,
            y1 + top    * thisH / o1H,
            x2 - right  * thisW / o1W,
            y2 - bottom * thisH / o1H
            );
    }
};

#endif // SRECT_H_