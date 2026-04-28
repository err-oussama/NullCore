#include "isr.h"
#include "kprint.h"

void dived_error_handler() {
  kprint_err("\nDivision Error");
  while (0)
    ;
}
