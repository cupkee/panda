/*
 *
 */

#ifndef __ESBL_TREE_INC__
#define __ESBL_TREE_INC__

#include "config.h"

intptr_t tree_create();
int tree_destroy();

int tree_insl();
int tree_insr();
int tree_reml();
int tree_remr();

int tree_merge(t1, t2, root);

int tree_node_is_leaf();
int tree_node_is_eof();
int tree_node_data();
int tree_node_left();
int tree_node_right();

#endif /* __ESBL_TREE_INC__ */

