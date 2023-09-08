#include <stdio.h>

#define ARRAY_SIZE 512

int pas[ARRAY_SIZE] = {0};

void print_pas(int pas[])
{
    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("%d ", pas[i]);
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

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fptr;

    fptr = fopen(argv[1], "r");

    if (fptr == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    char line[4];
    int i = 0;

    while (fgets(line, sizeof(line), fptr) != NULL)
    {
        fscanf(fptr, "%d", &pas[i++]);
        fscanf(fptr, "%d", &pas[i++]);
        fscanf(fptr, "%d", &pas[i++]);
    }

    fclose(fptr);

    int pc = 0;
    int sp = i - 1;
    int bp = sp + 1;
    int halt = 1;
    while (halt)
    {
        int op = pas[pc];
        int l = pas[pc + 1];
        int m = pas[pc + 2];

        printf("%d %d %d\n", op, l, m);

        switch (op)
        {
        case 1:
            sp = sp + 1;
            pas[sp] = m;
            break;
        case 2:
            switch (m)
            {
            case 1:
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                break;
            case 2:
                pas[sp - 1] = pas[sp - 1] + pas[sp];
                sp = sp - 1;
                break;
            case 3:
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp = sp - 1;
                break;
            case 4:
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp = sp - 1;
                break;
            case 5:
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp = sp - 1;
                break;
            case 6:
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp = sp - 1;
                break;
            case 7:
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp = sp - 1;
                break;
            case 8:
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp = sp - 1;
                break;
            case 9:
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp = sp - 1;
                break;
            case 10:
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp = sp - 1;
                break;
            case 11:
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp = sp - 1;
            }
            break;
        case 3:
            sp = sp + 1;
            pas[sp] = pas[base(bp, l) + m];
            break;
        case 4:
            pas[base(bp, l) + m] = pas[sp];
            sp = sp - 1;
            break;
        case 5:
            pas[sp + 1] = base(bp, l);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            pc = m;
            break;
        case 6:
            sp = sp + m;
            break;
        case 7:
            pc = m;
            break;
        case 8:
            if (pas[sp] == 0)
                pc = m;
            sp = sp - 1;
            break;
        case 9:
            switch (m)
            {
            case 1:
                printf("%d\n", pas[sp]);
                sp = sp - 1;
                break;
            case 2:
                scanf("%d", &pas[sp]);
                break;
            case 3:
                halt = 0;
            }
        }

        pc = pc + 3;
    }

    return 0;
}