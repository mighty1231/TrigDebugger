#include "stream.h"
#include <windows.h>

void stream_init(struct stream *str) {
	memset(str, 0, sizeof(str));
}
void stream_push4(struct stream *str, int n) {
	if (!(str->size_byte & PAGE_BITMASK)){
		if (str->allocated_page_count == PAGE_MAXCOUNT)
			return;
		str->pages[str->allocated_page_count++] = malloc(PAGE_SIZE);
	}
	*(int *)(str->pages[str->allocated_page_count-1] + (str->size_byte & PAGE_BITMASK)) = n;
	str->size_byte += 4;
}
void stream_clear(struct stream *str) {
	for (int i = 0; i < str->allocated_page_count; i++) {
		free(str->pages[i]);
	}
	memset(str->pages, 0, str->allocated_page_count << 2);
	str->size_byte = 0;
	str->allocated_page_count = 0;
}

void stream_pushN(struct stream *str, const void *buf, int size) {
	if (size < 0 || size & 3) {
		return;
	}
	int offset;
	int remaining_bytes;
	char *p = (char *) buf;
	while (size > 0) {
		offset = str->size_byte & PAGE_BITMASK;
		if (offset == 0) {
			if (str->allocated_page_count == PAGE_MAXCOUNT)
				return;
			str->pages[str->allocated_page_count++] = malloc(PAGE_SIZE);
		}

		remaining_bytes = (size > PAGE_SIZE-offset) ? (PAGE_SIZE-offset) : size;

		memcpy(str->pages[str->allocated_page_count-1] + offset, p, remaining_bytes);
		size -= remaining_bytes;
		p += remaining_bytes;
	}
}
