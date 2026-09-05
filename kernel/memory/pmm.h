#ifndef PMM_H
#define PMM_H
#include <types.h>

// PMM => Physical Memory Manager
/*
#################################################
##################### TYPES #####################
#################################################
*/

typedef struct s_pmp {

  void *start;
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

void pmm_init(void *pmp_start, uint32 pmp_size);
void *pmm_get_pool_start();
uint32 pmm_get_pool_size();
void show_bitmap();

uint32 pmm_addre_to_frame(void *addre);
void *pmm_frame_to_addre(uint32 frame);
uint8 pmm_is_frame_free(uint32 frame);
uint8 pmm_is_addre_free(void *addre);

void pmm_use_frame(uint32 frame_number);
void pmm_free_frame(uint32 frame_number);

void *pmm_alloc();

void pmm_free(void *addre);

void pmm_info();

#endif
