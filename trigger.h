#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include <windows.h>

struct condition {
	/*  0 = 00h */  DWORD dwLocation;
	/*  4 = 04h */  DWORD dwGroup;
	/*  8 = 08h */  DWORD dwCount;
	/* 12 = 0Ch */  WORD  wUnitType;
	/* 14 = 0Eh */  BYTE  bComparisonType;
	/* 15 = 0Fh */  BYTE  bConditionType;
	/* 16 = 10h */  BYTE  bExtraType;
	/* 17 = 11h */  BYTE  bFlags;
	/* 18 = 12h */  BYTE  __bAlign[2];
};

struct action {
	/*  0 = 00h */  DWORD dwSourceLocation;
	/*  4 = 04h */  DWORD dwTextString;
	/*  8 = 08h */  DWORD dwWavString;
	/* 12 = 0Ch */  DWORD dwTime;
	/* 16 = 10h */  DWORD dwSourceGroup;
	/* 20 = 14h */  DWORD dwExtra;
	/* 24 = 18h */  WORD  wType;
	/* 26 = 1Ah */  BYTE  bActionType;
	/* 27 = 1Bh */  BYTE  bModifier;
	/* 28 = 1Ch */  BYTE  bFlags;
	/* 29 = 1Dh */  BYTE  __bAlign[3];
};

struct trigger {
	/*    0 = 000h */  struct trigger   *prev;
	/*    4 = 004h */  struct trigger   *next;
	/*    8 = 008h */  struct condition conditions[16];   // 20byte
	/*  328 = 148h */  struct action    actions[64];      // 32byte
	/* 2376 = 948h */  DWORD            dwExecutionFlags;
	/* 2380 = 94Ch */  BYTE             bExecuteFor[27];
	/* 2407 = 967h */  BYTE             bCurrentActionIndex;
};

#endif
