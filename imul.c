#include<stdio.h>
#include<stdarg.h> //для va_

#define pc reg[7]
#define NO_PARAM 0
#define HAS_XX 1
#define HAS_SS (1<<1)
#define HAS_DD (1<<2)


typedef char byte;
typedef int word;
typedef word adr;

byte mem[56*1024];
word reg[8];
word nn;

struct SSDD
{
    word val;
    adr a;
} ss, dd;

void b_write(adr a, byte x)
{
    mem[a] = x;
}

byte b_read(adr a)
{
    return mem[a];
}

void w_write(adr a, word x)//???
{
    mem[2] = (byte)(x & 0xff);
    mem[3] = (byte)((x>>8) & 0xff);
}

void do_halt()//??
{
    printf("THE END\n");
    exir(0);
}

void do_add()
{
    write(dd.a) =ss.val + dd.val;
}

void do_move()
{
        write(dd.a) =ss.val;

}

void do_unknown() {}

struct Command
{
    word opcode;
    word mask;
    const char * name;
    void (*do_func());
    byte param;
};

Command[] =
{
    {0010000, 0170000, "move", do_move, HAS_SS | HAS_DD},
    {0060000, 0170000, "add", do_add, HAS_SS | HAS_DD},
    {0000000, 0177777, "halt", do_halt, NO_PARAM},
    {0000000, 0170000, "unknown", do_unknown, NO_PARAM},
}

void run()
{
    word pc = 0100;
    while(1)
    {
        word w = w_read(pc);
        fprintf(stdout, "%06o: %06o ", pc, w);
        pc+=2;
        for(int i = 0; i < ; i++
        {
            cmd = command[i];
            if (())
        })
    }
}

void trace(int debug_level, const char * format. ...) {
if (debug_lvl ....)
return 	;
va_list ap;
va_start (format, ap);
vprintf(format, ap);
va_end(ap);
}
return 0;
