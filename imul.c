#include<stdio.h>
#include<stdarg.h> //для va_
#include<assert.h>
#include<stdlib.h>

#define pc reg[7]

#define NO_PARAM 0
#define HAS_XX 1
#define HAS_SS (1<<1)
#define HAS_DD (1<<2)

#define fir_8b (x & 0xff)
#define sec_8b ((x>>8) & 0xff)



typedef unsigned char byte;
typedef int word;
typedef word adr;

byte mem[56*1024];
word reg[8];

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
    mem[a] = (byte)fir_8b;
    mem[a + 1] = (byte)sec_8b;
}

word w_read(adr a)
{
    assert (a % 2 == 0);
    return (((word)mem[a + 1])<<8) | (word)mem[a];//
}

void load_file(char * f)
{
    FILE *f_in = NULL;
    f_in = fopen(f, "r");
    if(f_in == NULL)
    {
        perror(f);
        exit(1);
    }
    adr ad;
    int n, i;
    byte * a;
    while(fscanf(f_in, "%x", &ad) > 0)
    {
        fscanf(f_in, "%x", &n);
        a = malloc(n * sizeof(*a));
        for(i = 0; i < n; i++)
        {
            fscanf(f_in, "%x", &a[i]);
            b_write(ad + i, a[i]);
        }
    }
    free(a);
    fclose(f_in);
}

struct Command
{
    word opercode;
    word mask;
    const char * name;
    //void (*do_func());
    byte param;
};

struct SSDD
{
    word val;
    adr a;
} ss, dd;

int main(int argc, char * argv[])
{
    printf("the 3 argum is %s\n", argv[2]);
    printf("there are %d argc\n", argc);
    load_file(argv[argc - 1]);
    mem_dump(0x200, 0xc);
    reg_print();
    return 0;
}

void mem_dump(adr adr_st, int n)
{
    printf("\nMemory dumping\n");
    int i;
    for(i = adr_st; i < adr_st + n; i+=2)
        printf("mem[%d] : %07o\n", i, w_read(i));
}

void reg_print()
{
    int i;
    printf("\nRegisters typing\n");
    for(i = 0; i < 8; i++)
        printf("reg[%d] = %07o\n", i, reg[i]);
}

/*void do_halt()//??
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
        write(dd.a) = ss.val;

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
*/
