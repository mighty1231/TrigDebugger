#include "hook.h"
#include "trigger.h"

// referenced
// https://github.com/bwapi/bwapi/blob/master/bwapi/BWAPI/Source/Assembly.cpp

extern struct stream *mainstream;

unsigned int st_eax, st_ebx, st_ecx, st_edx, st_esi, st_edi, st_esp, st_ebp;
int CONDITIONFUNCS[24] = {
0x0042C680, // Always()
0x0042C720, // CountdownTimer(Comparison, Time)
0x0042CC20, // Command(Player, Comparison, Number, Unit)
0x0042CD70, // Bring(Player, Comparison, Number, Unit, Location)
0x0042CBE0, // Accumulate(Player, Comparison, Number, ResourceType)
0x0042CBA0, // Kills(Player, Comparison, Number, Unit)
0x0042CA70, // CommandMost(Unit)
0x0042CCF0, // CommandMostAt(Unit, Location)
0x0042C990, // MostKills(Unit)
0x0042C8B0, // HighestScore(ScoreType)
0x0042C7D0, // MostResources(ResourceType)
0x0042C610, // Switch(Switch, State)
0x0042C6E0, // ElapsedTime(Comparison, Time):
0x0042C670, // Briefing()
0x0042CB20, // Opponents(Player, Comparison, Number)
0x0042CB60, // Deaths(Player, Comparison, Number, Unit)
0x0042CA00, // CommandLeast(Unit)
0x0042CC70, // CommandLeastAt(Unit, Location)
0x0042C920, // LeastKills(Unit)
0x0042C840, // LowestScore(ScoreType)
0x0042C760, // LeastResources(ResourceType)
0x0042CAE0, // Score(Player, ScoreType, Comparison, Number)
0x0042C680, // Always()
0x0042C670  // Never()
};

#define SAVE_REGISTERS() __asm mov st_eax, eax \
                         __asm mov st_ebx, ebx \
                         __asm mov st_ecx, ecx \
                         __asm mov st_edx, edx \
                         __asm mov st_edi, edi \
                         __asm mov st_esi, esi \
                         __asm mov st_ebp, ebp \
                         __asm mov st_esp, esp

#define LOAD_REGISTERS() __asm mov eax, st_eax \
                         __asm mov ebx, st_ebx \
                         __asm mov ecx, st_ecx \
                         __asm mov edx, st_edx \
                         __asm mov edi, st_edi \
                         __asm mov esi, st_esi \
                         __asm mov ebp, st_ebp \
                         __asm mov esp, st_esp

// cover from 489462 to 489493
__declspec(naked) void triggerHook(void) {
    // SAVE_REGISTERS();


    // *(int *)0x58A364 += 1;
    /*
    eax = trig->next; // Caution: triggerVector has different structure with Trigger.
    if (trig->next >= 0){
        Trigger *trigger = esi;

        while ((unsigned int) trigger >= 0){
            if (!(trigger->flag & 8)) {
                (Trigger *)0x6509AC = trigger; // currentTriggerNode
                if (!(trigger->flag & 1)) {
                    if (ExecuteTriggerConditions(trigger)){
                        trigger->bCurrentActionIndex = 0;
                        ExecuteTriggerActions();
                    }
                } else
                    ExecuteTriggerActions();
            }
            trigger = trigger->next;
        }
    }
    */
    __asm {
    loc_489462:
        mov eax, [esi+948h]
        test al, 8
        jnz loc_48948D
        test al, 1
        mov dword ptr ds:[0x6509AC], esi ; different with mov dword ptr [0x6509AC], esi and masm prevents it
        jnz loc_489488
        mov eax, esi
        call conditionHook
        test eax, eax
        jz loc_48948D
        mov byte ptr [esi+967h], 0
    loc_489488:
        mov edx, 0x489136
        call edx
    loc_48948D:
        mov esi, [esi+4]
        test esi, esi
        jg loc_489462
    loc_489494:
        mov edx, 0x489494
        jmp edx
    }
    // stream_push4(mainstream, (int) curtrig);
    // stream_push4(mainstream, curtrig->dwExecutionFlags);
    // if(!(curtrig->dwExecutionFlags & 8)) {
    //     *(struct trigger *)0x6509AC = trigger;
    //     if(!(curtrig->dwExecutionFlags & 1)) {
    //         // check conditions
    //     }
    // }

    // LOAD_REGISTERS();
    // __asm {
    //     mov eax, [esi+948h]
    //     test al, 8
    //     jnz 0x48948D
    //     mov edx, 0x48946C
    //     jmp edx
    // }
}

// instead of routine in 489200
// patch on 
__declspec(naked) void conditionHook(void) {
    __asm {
        push esi
        push edi

        mov esi, eax
        xor edi, edi
        add esi, 0x17

    mainloop:
        test BYTE PTR [esi+0x2], 0x2
        jne conti
        mov al, BYTE PTR [esi]
        test al, al
        je returnTrue

        movzx eax, al
        lea ecx, [esi-0xF]
        call DWORD PTR [eax*4+CONDITIONFUNCS]
        test eax, eax
        je returnFalse

        conti:
        inc edi
        add esi, 0x14
        cmp edi, 0x10
        jl mainloop

    returnTrue:
        pop edi
        mov eax, 0x1
        pop esi
        ret

    returnFalse:
        pop edi
        xor eax, eax
        pop esi
        ret
    }
}
