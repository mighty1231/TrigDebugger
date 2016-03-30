#ifndef __LOG_H__
#define __LOG_H__

#define TRIGSTATUS_DISABLED         0
#define TRIGSTATUS_EXECUTE          1
#define TRIGSTATUS_EXECUTE_ALWAYS   2

#define COND_DISABLED   0x01
#define COND_NOCOND     0x02

struct execlog {
	// int address; // included in datalog
	int datalog_index;

	int trigstatus;

	unsigned char cond_execflag[16];
	/*
	* 0  1  2  3  ... 30 31
	* 
	* for (int edi = 0; edi < 16; edi++, esi+=20) {
	* 	if (!(* ((char *)esi + 2)) & 2) {
	*       // push 1
	* 		if (*(char *)esi != 0) { // if conditionType != 0
	*           // push 1
	* 			eax = *(char *) esi;
	* 			ecx = esi - 15; // same with &(trig->conditions[edi])
	* 			if (TRGCND_Array[eax]() == 0) {
	* 				// 48923E
	* 				return 0;
	* 			}
	* 		}
	* 		else
	* 			break;
	* 	}
	* }
	* // 489236
	* return 1;
	* 
	* 
	* 
	* 
	*/
	// reserved for only TRIG_EXECUTE
	// unsigned int cond_infos; // add this later
};

/* ------------------------------ */
struct datalog {
	// binary search tree
	// same key might be happened, and it is searched or inserted on LEFT child.
	unsigned int address; // key for bst
	struct datalog *left, *right;

	// only for storing
	struct datalog *prev, *next; 
	size_t index;

	unsigned int hashval;   // simple hash
	char data[2408];
};
struct datalog_comp {
	unsigned int address;
	size_t left, right;
	char data[2408];
};
void datalog_clear(void);
size_t datalog_insert(const void *new_trig, unsigned int addr);
size_t datalog_comp_size_byte(void);
void datalog_comp_save(void *buf);

/* ------------------------------ */

#define LOG_TICK       0
#define LOG_TRIGVECTOR 1
#define LOG_EXECUTION  2
#define LOG_CONDINFO   3

struct log {
	unsigned int type;
	unsigned int size;
	union {
		unsigned int tick;
		struct execlog execution;
		unsigned int trigvector;
		unsigned int condinfo[];
	};
};

#define LOG_TRIGGER_SIZE    (sizeof(unsigned int)+sizeof(unsigned int)+sizeof(struct trigLog))
#define LOG_TICK_SIZE       (sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int))
#define LOG_TRIGVECTOR_SIZE (sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int))

#endif
