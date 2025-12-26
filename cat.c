// meow
#include <stdio.h>
#include <stdlib.h>

// syscalls
#define SYS_read 0
#define SYS_write 1
#define SYS_open 2
#define SYS_close 6
#define SYS_exit 60

// file operations
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02

long syscall1(long number, long arg1)
{
  long result;
  __asm__ __volatile__("syscall" : "=a"(result) : "a"(number), "D"(arg1) : "rcx", "r11", "memory");

  return result;
}

long syscall2(long number, long arg1, long arg2)
{
  long result;
  __asm__ __volatile__("syscall"
                       : "=a"(result)
                       : "a"(number), "D"(arg1), "S"(arg2)
                       : "rcx", "r11", "memory");

  return result;
}

long syscall3(long number, long arg1, long arg2, long arg3)
{
  long result;
  __asm__ __volatile__("syscall"
                       : "=a"(result)
                       : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3)
                       : "rcx", "r11", "memory");

  return result;
}

// func prototypes
long unsigned strlen(char *string);
long fileopen(char *path, char *mode);
void fileclose(long fd);
long getcharacter(long fd, char *ch);
void print(char *string);
void putcharacter(char *character);
void exit(int code);

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

// opens a file given the path and the mode
// returns the file descriptor to be used later
long fileopen(char *path, char *mode)
{
  long fd = syscall2(SYS_open, (long)path, (long)mode);

  return fd;
}

void fileclose(long fd)
{
  syscall1(SYS_close, fd);
}

// get a single character from the file you opened
// returns it back in the ch buffer
long getcharacter(long fd, char *ch)
{
  long bytes_read;
  bytes_read = syscall3(SYS_read, fd, (long)ch, 1);
  return bytes_read;
}

// simple printing to the screen
// doesn't take extra arguments
void print(char *string)
{
  syscall3(SYS_write, 1, (long)string, strlen(string));
}

// prints one character to the screen
void putcharacter(char *character)
{
  syscall3(SYS_write, 1, (long)character, 1);
}

// main work
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    print("Usage: cat FILE\n Prints file to terminal\n");
    return 1;
  }

  char current_char = 1;

  // open file
  long fd = fileopen(argv[1], O_RDONLY);

  if (fd < 0)
  {
    print("Error opening file\n");
    print(argv[1]);
    print("\n");
    fileclose(fd);
    exit(2);
  }

  long bytes_read;
  do
  {
    bytes_read = getcharacter(fd, &current_char);
    if (bytes_read > 0)
    {
      putcharacter(&current_char);
    }
  } while (bytes_read > 0);

  fileclose(fd);
  return 0;
}

void exit(int code)
{
  syscall1(SYS_exit, code);

  for (;;)
  {
  }
}

__attribute__((naked)) void _start()
{
  __asm__ __volatile__("xor %ebp, %ebp\n"
                       "mov (%rsp), %rdi\n"
                       "lea 8(%rsp), %rsi\n"
                       "and $-16, %rsp\n"
                       "call main\n"
                       "mov %rax, %rdi\n"
                       "call exit\n");
}
