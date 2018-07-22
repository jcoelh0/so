/**
 *  \author ...
 *  \tester ...
 */

#include "direntries.h"
#include "direntries.bin.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>

void soCheckEmptiness(int ih)
{
    soProbe(358, "soCheckEmptiness(%d)\n", ih);
    soCheckEmptinessBin(ih);
}
