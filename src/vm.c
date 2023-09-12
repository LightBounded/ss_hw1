#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 512

int pas[ARRAY_SIZE] = {0};
int lex_level_idx = 0;
int lex_levels[3] = {0};
int starting_stack_idx;

FILE *input_file;
FILE *output_file;

int bp;
int sp;
int pc;

int op;
int l;
int m;

void print_lex_levels()
{
    for (int i = 0; i < 3; i++)
        printf("%d ", lex_levels[i]);
    printf("\n");
}

void print_pas()
{
    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("%d ", pas[i]);
    printf("\n");
}

int base(int bp, int l)
{
    int arb = bp; // arb = activation record base
    while (l > 0) // find base L levels down
    {
        arb = pas[arb];
        l--;
    }
    return arb;
}

void get_operation_name(char operation_name[4])
{
    switch (op)
    {
    case 1:
        strcpy(operation_name, "LIT");
        break;
    case 2:
        switch (m)
        {
        case 0:
            strcpy(operation_name, "RTN");
            break;
        case 1:
            strcpy(operation_name, "ADD");
            break;
        case 2:
            strcpy(operation_name, "SUB");
            break;
        case 3:
            strcpy(operation_name, "MUL");
            break;
        case 4:
            strcpy(operation_name, "DIV");
            break;
        case 5:
            strcpy(operation_name, "EQL");
            break;
        case 6:
            strcpy(operation_name, "NEQ");
            break;
        case 7:
            strcpy(operation_name, "LSS");
            break;
        case 8:
            strcpy(operation_name, "LEQ");
            break;
        case 9:
            strcpy(operation_name, "GTR");
            break;
        case 10:
            strcpy(operation_name, "GEQ");
            break;
        }
        break;
    case 3:
        strcpy(operation_name, "LOD");
        break;
    case 4:
        strcpy(operation_name, "STO");
        break;
    case 5:
        strcpy(operation_name, "CAL");
        break;
    case 6:
        strcpy(operation_name, "INC");
        break;
    case 7:
        strcpy(operation_name, "JMP");
        break;
    case 8:
        strcpy(operation_name, "JPC");
        break;
    case 9:
        strcpy(operation_name, "SYS");
        break;
    }
}

// Print stack seperating activation records with |
void print_stack()
{
    int i = 0;
    for (int j = starting_stack_idx; j <= sp; j++)
    {
        if (i < lex_level_idx && j == lex_levels[i] && j != 0)
        {
            i++;
            printf("| ");
        }
        printf("%d ", pas[j]);
    }
}

void print_stack_trace()
{
    char operation_name[4];
    get_operation_name(operation_name);

    printf("%3s %2d %2d %2d %2d %2d ", operation_name, l, m, pc, bp, sp);

    print_stack();

    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");
    output_file = fopen("./src/output.txt", "w");

    if (input_file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    // Fetch instructions
    int i = 0;
    while (1)
    {
        if (op == 9 && m == 3)
            break;

        fscanf(input_file, "%d %d %d", &op, &l, &m);

        pas[i] = op;
        pas[i + 1] = l;
        pas[i + 2] = m;

        i += 3;
    }

    fclose(input_file);

    starting_stack_idx = i;
    bp = i;
    sp = bp - 1;
    pc = 0;

    printf("                     PC BP SP stack\n");
    printf("Initial values       %2d %2d %2d\n", pc, bp, sp);

    int halt = 0;

    // Execute instructions
    while (!halt)
    {
        // Number of instructions has been met, halt
        if (pc == i)
            halt = 1;

        op = pas[pc];
        l = pas[pc + 1];
        m = pas[pc + 2];

        // Increment program counter by 3
        pc += 3;

        switch (op)
        {
        case 1: // LIT 0, M
            sp = sp + 1;
            pas[sp] = m;
            break;
        case 2: // OPR, 0, # (0 <= # <= 11)
            switch (m)
            {
            case 0: // RTN
                lex_level_idx = lex_level_idx - 1;
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                break;
            case 1: // ADD
                pas[sp - 1] = pas[sp - 1] + pas[sp];
                sp = sp - 1;
                break;
            case 2:
                // SUB
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp = sp - 1;
                break;
            case 3: // MUL
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp = sp - 1;
                break;
            case 4: // DIV
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp = sp - 1;
                break;
            case 5: // EQL
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp = sp - 1;
                break;
            case 6: // NEQ
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp = sp - 1;
                break;
            case 7: // LSS
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp = sp - 1;
                break;
            case 8: // LEQ
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp = sp - 1;
                break;
            case 9: // GTR
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp = sp - 1;
                break;
            case 10: // GEQ
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp = sp - 1;
            }
            break;
        case 3: // LOD L, M
            sp = sp + 1;
            pas[sp] = pas[base(bp, l) + m];
            break;
        case 4: // STO L, M
            pas[base(bp, l) + m] = pas[sp];
            sp = sp - 1;
            break;
        case 5: // CAL L, M
            pas[sp + 1] = base(bp, l);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            lex_levels[lex_level_idx++] = bp;
            pc = m;
            break;
        case 6: // INC 0, M
            sp = sp + m;
            break;
        case 7: // JMP 0, M
            pc = m;
            break;
        case 8: // JPC 0, M
            if (pas[sp] == 0)
                pc = m;
            sp = sp - 1;
            break;
        case 9: // SYS
            switch (m)
            {
            case 1: // SYS 0, 1
                printf("%d\n", pas[sp]);
                sp = sp - 1;
                break;
            case 2: // SYS 0, 2
                sp = sp + 1;
                scanf("%d", &pas[sp]);
                break;
            case 3: // SYS 0, 3
                halt = 1;
            }
        }
        print_stack_trace();
    }

    return 0;
}