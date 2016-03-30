#ifndef __STREAM_H__
#define __STREAM_H__

#define PAGE_MAXCOUNT   1000
#define PAGE_SIZE       4096
#define PAGE_BITMASK    4095

struct stream {	
	unsigned int size_byte;
	unsigned int allocated_page_count;
	char *pages[PAGE_MAXCOUNT];
};

void __cdecl stream_init(struct stream 	*);
void __cdecl stream_push4(struct stream *, int);
void __cdecl stream_clear(struct stream *);
void __cdecl stream_pushN(struct stream *, const void *, int);
// void *stream_allocN(struct stream*, int); // alloc N byte.. delete for problem of page allocation


#endif