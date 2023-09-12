#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 512 // Process Address Space (PAS) size

int pas[ARRAY_SIZE] = {0}; // Process Address Space (PAS)
int lex_level_idx = 0;     // Current lexicographical level
int lex_levels[3] = {0};   // Lexicographical levels (max 3)
int stack_start_idx = 0;   // Index of the start of the stack in the PAS

FILE *in_file_ptr;
FILE *out_file_ptr;

int bp; // Base pointer
int sp; // Stack pointer
int pc; // Program counter

int op; // Op code
int l;  // Lexicographical level
int m;  // Modifier

void print_lex_levels() // Print the lexicographical levels (used for debugging)
{
    for (int i = 0; i < 3; i++)
        printf("%d ", lex_levels[i]);
    printf("\n");
}

void print_pas() // Print the entire process address space (used for debugging)
{
    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("%d ", pas[i]);
    printf("\n");
}

int base(int bp, int l)
{
    int arb = bp; // Activation record base
    while (l > 0) // Find base L levels down
    {
        arb = pas[arb];
        l--;
    }
    return arb;
}

// Store pneumonic for the current operation in the provided char array
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

// Print the state of the VM
void print_stack_trace()
{
    // Get the pneumonic for the current operation
    char operation_name[4];
    get_operation_name(operation_name);

    // Print the executed instruction and the state of the VM
    fprintf(out_file_ptr, "%3s %2d %2d %2d %2d %2d ", operation_name, l, m, pc, bp, sp);

    // Print the current stack state
    int i = 0;
    for (int j = stack_start_idx; j <= sp; j++)
    {
        // Print a vertical bar to indicate a new activation record
        if (i < lex_level_idx && j == lex_levels[i] && j != 0)
        {
            i++;
            fprintf(out_file_ptr, "| ");
        }
        fprintf(out_file_ptr, "%d ", pas[j]);
    }

    fprintf(out_file_ptr, "\n");
}

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    in_file_ptr = fopen(argv[1], "r");
    out_file_ptr = fopen("./src/output.txt", "w");

    if (in_file_ptr == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    // Store each instruction in the PAS (fetch cycle)
    int i = 0;
    while (1)
    {
        if (op == 9 && m == 3) // SYS 0, 3 (halt)
            break;

        // Read in the next instruction
        fscanf(in_file_ptr, "%d %d %d", &op, &l, &m);

        pas[i] = op;
        pas[i + 1] = l;
        pas[i + 2] = m;

        i += 3;
    }

    fclose(in_file_ptr);

    // Store the index of the start of the stack in the PAS
    // This will be used for printing the stack trace
    stack_start_idx = i;
    bp = i;
    sp = bp - 1;
    pc = 0;

    fprintf(out_file_ptr, "                     PC BP SP stack\n");
    fprintf(out_file_ptr, "Initial values       %2d %2d %2d\n", pc, bp, sp);

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
                sp--;
                break;
            case 2:
                // SUB
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp--;
                break;
            case 3: // MUL
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp--;
                break;
            case 4: // DIV
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp--;
                break;
            case 5: // EQL
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp--;
                break;
            case 6: // NEQ
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp--;
                break;
            case 7: // LSS
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp--;
                break;
            case 8: // LEQ
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp--;
                break;
            case 9: // GTR
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp--;
                break;
            case 10: // GEQ
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp--;
            }
            break;
        case 3: // LOD L, M
            sp++;
            pas[sp] = pas[base(bp, l) + m];
            break;
        case 4: // STO L, M
            pas[base(bp, l) + m] = pas[sp];
            sp--;
            break;
        case 5: // CAL L, M
            pas[sp + 1] = base(bp, l);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            pc = m;
            lex_levels[lex_level_idx++] = bp;
            break;
        case 6: // INC 0, M
            sp += m;
            break;
        case 7: // JMP 0, M
            pc = m;
            break;
        case 8: // JPC 0, M
            if (pas[sp] == 0)
                pc = m;
            sp--;
            break;
        case 9: // SYS
            switch (m)
            {
            case 1: // SYS 0, 1
                fprintf(out_file_ptr, "Output result is: %d\n", pas[sp]);
                sp--;
                break;
            case 2: // SYS 0, 2
                sp++;
                printf("Enter an integer: ");
                scanf("%d", &pas[sp]);
                break;
            case 3: // SYS 0, 3
                halt = 1;
            }
        }
        print_stack_trace();
    }

    fclose(out_file_ptr);
    return 0;
}