// meow
#include <stdio.h>
#include <stdlib.h>

// syscalls 
#define SYS_write 1
#define SYS_exit 60

long syscall1(long number, long arg1)
{
	long result;
	__asm__ __volatile__(
		"syscall"
		: "=a" (result)
		: "a" (number), "D" (arg1)
		: "rcx", "r11", "memory"
	);

	return result;
}

long syscall2(long number, long arg1, long arg2)
{
	long result;
	__asm__ __volatile__(
		"syscall"
		: "=a" (result)
		: "a" (number), "D" (arg1), "S" (arg2)
		: "rcx", "r11", "memory"
	);

	return result;
}

long syscall3(long number, long arg1, long arg2, long arg3)
{
	long result;
	__asm__ __volatile__(
		"syscall"
		: "=a" (result)
		: "a" (number), "D" (arg1), "S" (arg2), "d" (arg3)
		: "rcx", "r11", "memory"
	);

	return result;
}

long unsigned strlen(char *string)
{
	char *cursor = string; 
	while (*cursor)
	{
		cursor++;
	}

	long unsigned result = cursor - string;
	return result;
}

void print(char *string)
{
	syscall3(SYS_write, 1, (long)string, strlen(string));
}

void putcharacter(char *character)
{
	syscall3(SYS_write, 1, (long)character, 1);
}

int main(int argc, char *argv[]) 
{
	if (argc != 2)
	{
		print("Usage: cat FILE\n Prints file to terminal\n");
		return 1;
	}

	char current_char;

	// open file 
	FILE *fptr = fopen(argv[1], "r");
	if (!fptr)
	{
		print("cannot open: ");
		print(argv[1]);
		print("\n");
	}

	while ( (current_char = fgetc(fptr)) != EOF )
	{
		putcharacter(&current_char);
	}
	
	fclose(fptr);
	return 0;

}

void exit(int code)
{
	syscall1(SYS_exit, code);

	for(;;) {}
}


__attribute__((naked)) void _start() 
{
	__asm__ __volatile__(
		"xor %ebp, %ebp\n" 
		"mov (%rsp), %rdi\n" 
		"lea 8(%rsp), %rsi\n" 
		"and $-16, %rsp\n"
		"call main\n" 
		"mov %rax, %rdi\n"
		"call exit\n"
	);
}
