#ifndef _VB6Logic_H_
#define _VB6Logic_H_

// This is a wrapper for the && operator which ensures both sides of the operation get evaluated,
// so side effects may happen. This is consistent with the "And" operator in VB6
static inline bool VBAnd(bool a, bool b)
{
    return a && b;
}

// This is a wrapper for the || operator which ensures both sides of the operation get evaluated,
// so side effects may happen. This is consistent with the "And" operator in VB6
static inline bool VBOr(bool a, bool b)
{
    return a || b;
}

#endif
