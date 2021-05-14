#ifndef _RADIX_TREE_H_
#define _RADIX_TREE_H_

#define RADIX_TREE_MAP_SHIFT  3U
#define RADIX_TREE_MAP_SIZE (1UL << RADIX_TREE_MAP_SHIFT)
#define RADIX_TREE_MAP_MASK (RADIX_TREE_MAP_SIZE - 1)

#define DIV_ROUND_UP(X,Y) (((X) +((Y) - 1 )) / (Y))

#define RADIX_TREE_INDEX_BITS (8 * sizeof(unsigned long))
#define RADIX_TREE_MAX_PATH (DIV_ROUND_UP(RADIX_TREE_INDEX_BITS , \
						RADIX_TREE_MAP_SHIFT) )

#define RADIX_ERROR -1

#define RADIX_TREE_INIT   { \
        .height = 0,        \
        .rnode = NULL,      \
}

#define RADIX_TREE(name) \
         struct radix_tree_root name = RADIX_TREE_INIT


#define INIT_RADIX_TREE(root, mask)  \
do {                                 \
        (root)->height = 0;          \
        (root)->rnode = NULL;        \
} while (0)
	
struct radix_tree_node {
	unsigned int height;
	unsigned int count;
	void	*slots[RADIX_TREE_MAP_SIZE];
};

struct radix_tree_root{
	unsigned int height;
	struct radix_tree_node *rnode;
};

extern int radix_tree_insert(struct radix_tree_root *root,
		unsigned long index, void *item);

extern void *radix_tree_delete(struct radix_tree_root *root,
		unsigned long index);

extern void * radix_tree_lookup(struct radix_tree_root *root,
		unsigned long index);

#endif
