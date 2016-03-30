#include "log.h"
#include "hash.h"
#include <windows.h>

static struct datalog *dlFront = NULL;
static struct datalog *dlEnd = NULL;
static size_t dlCount = 0;

void datalog_clear(void) {
	if (dlCount == 0)
		return;

	struct datalog *d = dlFront;
	struct datalog *next;
	while (d != NULL) {
		next = d->next;
		free(d);
		d = next;
	}
	dlFront = NULL;
	dlEnd = NULL;
	dlCount = 0;
}

// @TODO implement this as balanced binary tree.
// hard thing is considering the case for same address but different data.
size_t datalog_insert(const void *new_trig, unsigned int address) {
	// searching
	unsigned int hashval = hash(new_trig, 2408);
	struct datalog *curdl = dlFront; // at this moment, dlFront is same as the root of binary tree
	struct datalog **lastsite = NULL;
	struct datalog *lowerdl = NULL; // to implement duplicates in the bst.

	/* make new datalog when
	 *  1. new address
	 *  2. same address, but different data : for this case, new datalog is inserted to left part.
	 */
	while (curdl != NULL) {
		if (curdl->address > address) {
			lastsite = &curdl->left;
			curdl = curdl->left;
		}
		else if (curdl->address < address) {
			lastsite = &curdl->right;
			curdl = curdl->right;
		}
		else {
			if (hashval != curdl->hashval) {
				/* three cases
				 *  1. nothing in the left
				 *  2. same address thing in the left
				 *  3. lower address thing in the left
				 */
				lastsite = &curdl->left;
				if (curdl->left == NULL)
					break;
				else if (curdl->left->address == address) {
					curdl = curdl->left;
				} else {
					lowerdl = curdl->left;
					break;
				}
			}
			return curdl->index;
		}
	}

	// if not searched
	struct datalog *dl;
	if (lastsite == NULL) { // The first dl
		dl = dlFront = dlEnd = (struct datalog *) malloc(sizeof(struct datalog));
		dl->address = address;

		dl->left = dl->right = dl->prev = dl->next = NULL;

		dl->index = dlCount++;
		dl->hashval = hashval;
		memcpy(dl->data, new_trig, 2408);
	}
	else if (lowerdl == NULL){ // general case insertion
		// lower thing ?
		dl = (struct datalog *) malloc(sizeof(struct datalog));
		dl->address = address;

		dl->left = dl->right = NULL;
		dl->prev = dlEnd; dl->next = NULL;
		dlEnd->next = dl; dlEnd = dl;

		*lastsite = dl;

		dl->index = dlCount++;
		dl->hashval = hashval;
		memcpy(dl->data, new_trig, 2408);
	} else { // wierd case. (insert with already-exist-address value)
		dl = (struct datalog *) malloc(sizeof(struct datalog));
		dl->address = address;

		dl->left = lowerdl; dl->right = NULL;
		dl->prev = dlEnd; dl->next = NULL;
		dlEnd->next = dl; dlEnd = dl;

		*lastsite = dl;

		dl->index = dlCount++;
		dl->hashval = hashval;
		memcpy(dl->data, new_trig, 2408);
	}
	return dl->index;
}

size_t datalog_comp_size_byte(void){
	return dlCount * sizeof(datalog_comp);
}
void datalog_comp_save(void *buf){
	struct datalog *cur = dlFront;
	struct datalog_comp *curcomp = (struct datalog_comp *) buf;

	while(cur != NULL) {
		curcomp->address = cur->address;
		curcomp->left = (cur->left)?cur->left->index:-1;
		curcomp->right = (cur->right)?cur->right->index:-1;
		memcpy(curcomp->data, cur->data, 2408);
		curcomp++;
		
		cur = cur->next;
	}
}
