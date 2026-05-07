#ifndef PMM_H
#define PMM_H
#include "type.h"

/*
#################################################
##################### TYPES #####################
#################################################
*/

typedef struct s_pmp {

  uint32 start;
  uint32 size;
} PMP;

/*
###################################################
##################### OBJECTS #####################
###################################################
*/

/*
#####################################################
##################### FUNCTIONS #####################
#####################################################
*/

void pmm_init(uint32 pmp_start, uint32 pmp_size);
uint32 pmm_get_pool_start();
uint32 pmm_get_pool_size();

uint32 pmm_addre_to_frame(uint32 addre);
uint32 pmm_frame_to_addre(uint32 frame);
uint8 pmm_is_frame_free(uint32 frame);

#endif
