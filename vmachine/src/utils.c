#include "utils.h"
#include <fcntl.h>

/* Ensure that kpti is enabled and pcid is disabled to make this function work */ 
void flush_tlb() {
    open("0asdawdawdawd", O_RDWR); // invalid syscall tlb flush with kpti enabled and pcid disabled  
}
