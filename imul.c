#include<stdio.h>
#include<stdarg.h> //для va_
#include<assert.h>
#include<stdlib.h>

#define pc reg[7]

#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD (1<<1)
#define HAS_NN (1<<3)
//#define HAS_XX (1<<5)
#define HAS_R (1<<4)
//#define HAS_R6 (1<<2)

#define fir_8b(x) (x & 0xff)
#define sec_8b(x) ((x>>8) & 0xff)

typedef unsigned char byte;
typedef int word;
typedef word adr;

byte mem[56*1024];
word reg[8];

int b;
word nn, rr;

void b_write(adr a, byte x)
{
    if(a < 8)
        reg[a] = (x & 0xff);//почему
    else
        mem[a] = (x & 0xFf);
}

byte b_read(adr a)
{
    return mem[a];
}

void w_write(adr a, word x)//???
{
    if(a < 8)
        reg[a] = x & 0xff;//почему
    else
    {
        assert(!(a % 2));
        mem[a] = (byte)fir_8b(x);
        mem[a + 1] = (byte)sec_8b(x);
    }
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

struct VAL_ADR
{
    word val;
    adr a;
} ss, dd;

void do_move_b()
{
    b_write(dd.a, ss.val);
}

void do_clr()
{
    w_write(dd.a, 0);
}

void do_sob()
{
    reg[rr]--;
    if(reg[rr] != 0)
    {
        //reg[rr]--;
        pc = pc - 2*nn;
    }
    printf("R%d ", rr);
}
void do_halt()//??
{
    printf("\nTHE END\n");
    reg_print();
    exit(0);
}

void do_add()
{
    w_write(dd.a, (dd.val + ss.val) & 0xffff);
}

void do_move()
{
    w_write(dd.a, (ss.val & 0xffff));
}

void do_unknown() {}



struct Command
{
    word opcode;
    word mask;
    const char * name;
    void (*do_func)();
    byte param;
}

cmds[] =
{
    {0010000, 0170000, "move",      do_move,    HAS_SS | HAS_DD},
    {0060000, 0170000, "add",       do_add,     HAS_SS | HAS_DD},
    {0000000, 0177777, "halt",      do_halt,    NO_PARAM},
    {0000000, 0170000, "unknown",   do_unknown, NO_PARAM},
    {0077000, 0177000, "sob",       do_sob,     HAS_R | HAS_NN},
	{0005000, 0177700, "clr",       do_clr,     HAS_DD},
	{0110000, 0170000, "movb", 	    do_move_b,    HAS_SS | HAS_DD},
};

int main(int argc, char * argv[])
{
    printf("\nthe third argum is %s\n", argv[2]);
    printf("there are %d argc\n", argc);
    load_file(argv[argc - 1]);
    mem_dump(0x200, 0xc);
    reg_print();
    run();
    reg_print();
    mem_dump(0x200, 0xc);
    return 0;
}

void mem_dump(adr adr_st, int n)
{
    printf("\nMemory dumping\n");
    int i;
    for(i = adr_st; i < adr_st + n; i+=2)
        printf("mem[%d] : %07o : %07o\n", i, i, w_read(i));
}

void reg_print()
{
    int i;
    printf("\nRegisters typing\n");
    for(i = 0; i < 8; i++)
        printf("reg[%d] = %07o\n", i, reg[i]);
}


struct VAL_ADR get_mode(word w)
{
    struct VAL_ADR res;
    word nn = w & 7; // fir_3b
    word mode = (w>>3) & 7;// sec_3b
    switch(mode)
    {
        case 0:      //регистр содержит искомое значение
            res.a = nn;
            res.val = reg[nn];
            printf("R%d ", nn);
            break;
        case 1:            //регистр содержит адрес ячейки памяти, где лежит значение
            res.a = reg[nn];
            if(b)
            {
                res.val = b_read(res.a);
                printf("(R%d) ", nn);
            }
            else
            {
                res.val = w_read(res.a);
                printf("(R%d) ", nn);
            }
            break;
        case 2:          //регистр содержит адрес ячейки памяти, где лежит значение, значение регистра увелич.
            res.a = reg[nn];
            if(b && (nn < 6))
            {
                res.val = b_read(res.a);
                reg[nn]++;
                //printf("(R%d)+ ", nn);
            }
            else
            {
                res.val = w_read(res.a);
                reg[nn]+=2;
            }
            if (nn != 7)
                printf("(R%d)+ ", nn);
            else
                printf("#%o ",res.val);
            break;
        case 3:              //регистр содержит адрес ячейки памяти, где лежит значение, значение регистра увелич.
            res.a = w_read(reg[nn]);
            if(b)
            {
                res.val = b_read(res.a);
            }
            else
            {
                res.val = w_read(res.a);
            }
            if (nn != 7)
                printf("@(R%d)+ ", nn);
            else
                printf("#%o ",res.val);
            reg[nn]+=2;
            break;
        case 4:           //уменьшаем значение регистра, интерпретируем его как адрес и находим значение
            if(b && nn < 6)
            {
                reg[nn]--;
                res.a = reg[nn];
                res.val = b_read(res.a);
            }
            else
            {
                reg[nn]-=2;
                res.a = reg[nn];
                res.val = w_read(res.a);
            }
            printf("-(R%d) ", nn);
            break;
        case 5:           //уменьшает значение регистра на 2, который содержит адрес ячейки памяти, где лежит значение, значение регистра увелич
            reg[nn]-=2;
            res.a = w_read(reg[nn]);
            if(b && nn < 6)
            {
                res.val = b_read(res.a);
            }
            else
            {
                res.val = w_read(res.a);
            }
            printf("@-(R%d) ", nn);
            break;
    }
    //printf("qwert");
    return res;
}

void run()
{
    printf("\nRunning\n");
    pc = 01000;//512//0x200
    while(1)
    {
        word w = w_read(pc);// & 0xffff;// исправитть
        b = (w>>15);
        printf("mem[%d] : %07o : %07o ",pc, pc, w);
        pc+=2;
        int i;
        for(i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++)
        {
            if((w & cmds[i].mask) == cmds[i].opcode)
            {
                printf("%s ", cmds[i].name);
                if(cmds[i].param & HAS_SS)
                    ss = get_mode(w>>6);
                if(cmds[i].param & HAS_DD)
                    dd = get_mode(w);
                if(cmds[i].param & HAS_NN)
					nn = w & 0x3f;
                if(cmds[i].param & HAS_R)
                    rr = (w>>6) & 7;
                cmds[i].do_func();
                //reg_print();
                break;
            }
        }
        printf("\n");
    }
    print_reg();
}


/*void trace(int debug_level, нетconst char * format. ...) {
if (debug_lvl ....)
return 	;
va_list ap;
va_start (format, ap);
vprintf(format, ap);
va_end(ap);
}
return 0;
*/
