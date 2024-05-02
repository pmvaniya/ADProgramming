#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

FILE *readSource(char *filename)
{
    FILE *program;
    program = fopen(filename, "r");

    if (NULL == program)
    {
        printf("Source file could not be opened.\n");
        exit(0);
    }

    return program;
}

void printSource(FILE *program)
{
    char ch;
    long initialPosition = ftell(program);

    printf("\n=> Source Code\n");
    while ((ch = fgetc(program)) != EOF)
    {
        putchar(ch);
    }
    printf("\n");

    fseek(program, initialPosition, SEEK_SET);
}

void writeStage1(int op, int num, int line_number)
{
    FILE *stage1;
    stage1 = fopen("stage1", "a");

    if (stage1 == NULL)
    {
        printf("Error opening the stage1 file.\n");
        exit(0);
    }

    fprintf(stage1, "%d,%d,%d\n", op, num, line_number);

    fclose(stage1);
}

void generateTokens(FILE *program)
{
    printf("\n=> Starting token generation\n");

    char ch;
    int line_number = 1;
    long initialPosition = ftell(program);

    ch = fgetc(program);
    while (ch != EOF)
    {
        if (ch == ' ' || ch == '\t')
        {
            ch = fgetc(program);
            continue;
        }

        if (ch == '\n')
        {
            line_number += 1;
            ch = fgetc(program);
            continue;
        }

        if (isdigit(ch))
        {
            char intToken[8]; /* Maximum 8 digit integer allowed */
            intToken[0] = ch;
            int i = 1;
            ch = fgetc(program);
            while (isdigit(ch))
            {
                intToken[i++] = ch;
                ch = fgetc(program);
            }
            intToken[i] = '\0';
            printf("'INT', '%s', %d\n", intToken, line_number);
            writeStage1(0, atoi(intToken), line_number);
            continue;
        }

        if (ch == '+')
        {
            printf("'ADD', '+', %d\n", line_number);
            writeStage1(1, 0, line_number);
            ch = fgetc(program);
            continue;
        }

        putchar(ch);
        ch = fgetc(program);
    }

    printf("\n");

    fseek(program, initialPosition, SEEK_SET);
}

int **readTokens(char *filename, int *rows, int *cols)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Unable to open the data file.\n");
        exit(1);
    }

    char line[32]; /* Length of longest possible record in the stage1 file + 1 to include '\n' */
    char *token;
    int row_count = 0;
    int col_count = 0;

    // Counting rows and columns
    while (fgets(line, sizeof(line), file))
    {
        token = strtok(line, ",");
        col_count = 0;

        while (token != NULL)
        {
            col_count++;
            token = strtok(NULL, ",");
        }

        row_count++;
    }

    fseek(file, 0, SEEK_SET);

    // Allocating memory for data
    int **data = (int **)malloc(row_count * sizeof(int *));
    if (data == NULL)
    {
        perror("Memory allocation error.\n");
        exit(1);
    }

    for (int i = 0; i < row_count; i++)
    {
        data[i] = (int *)malloc(col_count * sizeof(int));
        if (data[i] == NULL)
        {
            perror("Memory allocation error.\n");
            exit(1);
        }
    }

    int row = 0;
    // Reading data from file and storing in the array
    while (fgets(line, sizeof(line), file))
    {
        token = strtok(line, ",");
        int col = 0;

        while (token != NULL)
        {
            data[row][col] = atof(token);
            col++;
            token = strtok(NULL, ",");
        }
        row++;
    }

    fclose(file);

    *rows = row_count;
    *cols = col_count;

    return data;
}

void printTokens(int **data, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%4d ", data[i][j]);
        }
        printf("\n");
    }
}

void writeAssembly(char *a, char *b, char *c)
{
    FILE *assembly;
    assembly = fopen("assembly.asm", "a");

    if (assembly == NULL)
    {
        printf("Error opening the assembly file.\n");
        exit(0);
    }

    fprintf(assembly, "%-10s %-10s %-10s\n", a, b, c);

    fclose(assembly);
}

void generateAssembly(int **tokens, int rows, int cols)
{
    char str_num[8], str_line_num[8];

    writeAssembly("", "global", "_start");
    writeAssembly("", "section", ".text");
    writeAssembly("_start:", "", "");

    for (int i = 0; i < rows; i++)
    {
        if (tokens[i][0] == 0)
        {
            sprintf(str_num, "rax, %d", tokens[i][1]);
            sprintf(str_line_num, "; line %d: push int", tokens[i][2]);

            writeAssembly("", str_line_num, "");
            writeAssembly("", "mov", str_num);
            writeAssembly("", "push", "rax");
        }
        else if (tokens[i][0] == 1)
        {
            sprintf(str_line_num, "; line %d: add", tokens[i][2]);
            writeAssembly("", str_line_num, "");
            writeAssembly("", "pop", "rax");
            writeAssembly("", "pop", "rbx");
            writeAssembly("", "add", "rbx, rax");
            writeAssembly("", "push", "rbx");
        }
    }

    writeAssembly("exit:", "", "");
    writeAssembly("", "mov", "rax, 60");
    writeAssembly("", "pop", "rdi");
    writeAssembly("", "syscall", "");
}

int main()
{
    printf("\nThis is a compiler for AD Programming Language.\n");

    /**
     * Read the contents of the Source File.
     */
    FILE *program = readSource("source");
    long initialPosition = ftell(program);

    /**
     * Print the contents of the source file
     */
    printSource(program);

    /**
     * Generate tokens
     */
    generateTokens(program);

    /**
     * Read tokens
     */
    int rows, cols;
    int **tokens = readTokens("stage1", &rows, &cols);
    printTokens(tokens, rows, cols);

    /**
     * Generate Assembly
     */
    generateAssembly(tokens, rows, cols);

    /**
     * Gracefull close
     */
    fclose(program);

    for (int i = 0; i < rows; i++)
    {
        free(tokens[i]);
    }
    free(tokens);

    printf("\nEnd of Compiler.\n");

    return 0;
}