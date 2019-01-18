#ifndef __CC_INTERNALLIST_H
#define __CC_INTERNALLIST_H

#include "cs_parser_common.h"

#define SCODE_META    (-2)   // meta tag follows
#define SCODE_INVALID (-1)   // this should never get added, so it's a fault
#define SMETA_LINENUM (1)
#define SMETA_END (2)


struct ccInternalList {
    int length;    // size of array, in ints
    int allocated; // memory allocated for array, in bytes
    AGS::SymbolScript script;
    int pos;
    int lineAtEnd;
    int cancelCurrentLine;  // whether to set currentline=-10 if end reached

    void startread();
    AGS::Symbol peeknext();
    AGS::Symbol getnext();  // and update global current_line
    void ccInternalList::write(AGS::Symbol value);
    void ccInternalList::write_meta(AGS::Symbol type, int param);
    void shutdown();
    void init();
    ~ccInternalList();
    ccInternalList();

private:
    bool isPosValid(int pos);
};

#endif // __CC_INTERNALLIST_H
