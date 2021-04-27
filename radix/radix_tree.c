/*
 * radix tree实现代码，参考linux内核源码中radix实现
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "radix_tree.h"


struct radix_tree_path {
	struct radix_tree_node *node;
	unsigned int offset;
};

static inline struct radix_tree_node *
radix_tree_node_alloc(void)
{
	struct radix_tree_node *pNode;
	pNode = malloc(sizeof(struct radix_tree_node));
	if (pNode) {
		memset(pNode, 0, sizeof(struct radix_tree_node));
	}
	
	return pNode; 
}


/*
 * 将radix树缩小
 */
static inline void radix_tree_shrink(struct radix_tree_root *root)
{
	struct radix_tree_node *to_free;
	void *newptr;
	while (root->height) {
		to_free = root->rnode;
		
		/*
		 * 只存在最左节点将该节点删除
		 */
		if ((to_free->count != 1) || (NULL == to_free->slots[0]))
			break;
		newptr = to_free->slots[0];
		root->rnode = newptr;
		root->height--;
		free(to_free);
	}
}

/*
 * 高度为height的树可以容纳最大的index，从0开始
 */
static inline unsigned long radix_tree_maxindex(unsigned int height)
{
	return ((1 << (height*RADIX_TREE_MAP_SHIFT)) - 1);
}

/*
 * 扩展树的高度
 */
static inline int radix_tree_extend(struct radix_tree_root *root,
		unsigned long index)
{
	unsigned int height;
	struct radix_tree_node *node = NULL;

	height = root->height + 1;
	while (index > radix_tree_maxindex(height))
		height++;

	if (NULL == root->rnode){
		root->height = height;
		return 0;
	}

	/*
	 * 高度超长，需要扩展，扩展最左边的树
	 */
	do {
		if (NULL == (node = radix_tree_node_alloc())){
			return RADIX_ERROR;
		}
		node->slots[0] = root->rnode;
		node->height = ++(root->height);
		node->count = 1;
		root->rnode = node;
	}while(height > root->height);

	return 0;
}

int radix_tree_insert(struct radix_tree_root *root,
		unsigned long index, void *item)
{
	struct radix_tree_node *node = NULL, *slot;
	unsigned int height, shift;
	int offset;

	/*
	 * 新插入的需要的高度大于树的高度，则扩展树的高度。
	 * 保证扩展后为正常的树，空树不需要扩展
	 */
	if (index > radix_tree_maxindex(root->height)) {
		if (RADIX_ERROR == radix_tree_extend(root, index)){
			return RADIX_ERROR;
		}
	}
	slot = root->rnode;
	height = root->height;
	shift = (root->height - 1) * RADIX_TREE_MAP_SHIFT;

	offset = 0;
	while (height > 0){
		if (NULL == slot){
			if (!(slot = radix_tree_node_alloc()))
				return RADIX_ERROR;
			slot->height = height;
			
			/* 
			 * 扩展树的高度 
			 */
			if (node) {
				node->slots[offset] = slot;
				node->count++;
			} else {
				root->rnode = slot;
			}
		}

		offset = (index >> shift) & RADIX_TREE_MAP_MASK;
		node = slot;
		slot = node->slots[offset];
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

	if (NULL != slot){
		printf("index %lu exit \n", index);
		return 0;
	}

	if (node){
		node->slots[offset] = item;
		node->count++;
	}

	return 0;
}

void *radix_tree_delete(struct radix_tree_root *root,
		unsigned long index)
{
	struct radix_tree_path path[RADIX_TREE_MAX_PATH], *ppath = path;
	struct radix_tree_node *slot = NULL, *to_free;
	unsigned int height,shift,offset;
	
	/*
	 * 判读index是否在该树的高度范围内
	 */
	height = root->height;
	if (index > radix_tree_maxindex(height)) 
		goto out;
	
	/*
	 * path的第一个作为结束标示，查找index对应的value，
	 * 并将查找的路径记录到path中
	 */
	ppath->node = NULL;
	slot = root->rnode;
	shift = (height - 1)*RADIX_TREE_MAP_SHIFT;
	do {
		if (NULL == slot)
			goto out;
		ppath++;
		offset = (index >> shift) & RADIX_TREE_MAP_MASK;
		ppath->node = slot;
		ppath->offset = offset;
		slot = slot->slots[offset];
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	} while (height > 0);
	
	to_free = NULL;
	while (ppath->node) {
		ppath->node->slots[ppath->offset] = NULL;
		ppath->node->count--;
		
		if (to_free)
			free(to_free);
		
		/*
		 * 找到根据path释放找到的节点，若改节点上不存在节点，则释放改节点，
		 * 若node为头节点则尝试将树缩小
		 */
		if (ppath->node->count) {
			if (ppath->node == root->rnode)
				radix_tree_shrink(root);
			goto out;
		}
			
		to_free = ppath->node;
		ppath--;
	}
	
	/*
	 * 走到这里则整个树为空
	 */
	root->height = 0;
	root->rnode = NULL;
	if (to_free) 
		free(to_free);
	
out:
	return slot;
}


/*
 * 查找对应index的void *
 */
void * radix_tree_lookup(struct radix_tree_root *root,
		unsigned long index)
{
	struct radix_tree_node *node,*slot;
	unsigned int height,shift,offset;
	
	height = root->height;
	if (index > radix_tree_maxindex(height)) 
		return NULL;

	node = root->rnode;
	shift = (height - 1)*RADIX_TREE_MAP_SHIFT;
	do {
		offset = (index >> shift) & RADIX_TREE_MAP_MASK;
		slot = node->slots[offset];
		node = slot;
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}while((height > 0) && (node));
	
	return slot;
}
