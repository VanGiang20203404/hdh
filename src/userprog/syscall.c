#include "userprog/syscall.h"
#include "lib/stdio.h"
#include "lib/kernel/stdio.h"
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_user_vaddr(const void *vaddr)
{
  if (!is_user_vaddr(vaddr))
  {
    exit(-1);
  }
}

static void syscall_handler(struct intr_frame *f)
{
  // Lấy giá trị của system call từ địa chỉ ESP  //f->esp đại diện cho giá trị hiện tại của ESP (địa chỉ của đỉnh ngăn xếp) trong intr_frame (cấu trúc lưu trữ các thông tin của một khung ngắt)
  uint32_t syscall_number = *(uint32_t *)(f->esp);

  // Kiểm tra tính hợp lệ của địa chỉ người dùng
  check_user_vaddr(f->esp);

  // Biến cục bộ để lưu trữ kết quả trả về từ system call
  int32_t result = -1;

  // Thực hiện xử lý cho từng loại system call
  switch (syscall_number)
  {
  case SYS_HALT:
    halt();
    break;

  case SYS_EXIT:
    check_user_vaddr(f->esp + 4);
    exit(*(uint32_t *)(f->esp + 4));
    break;

  case SYS_EXEC:
    check_user_vaddr(f->esp + 4);
    result = exec((const char *)*(uint32_t *)(f->esp + 4));
    break;

  case SYS_WAIT:
    check_user_vaddr(f->esp + 4);
    result = wait((pid_t) * (uint32_t *)(f->esp + 4));
    break;

  case SYS_CREATE:
    check_user_vaddr(f->esp + 4);
    check_user_vaddr(f->esp + 8);
    result = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
    break;

  case SYS_REMOVE:
    check_user_vaddr(f->esp + 4);
    result = remove((const char *)*(uint32_t *)(f->esp + 4));
    break;

  case SYS_OPEN:
    check_user_vaddr(f->esp + 4);
    result = open((const char *)*(uint32_t *)(f->esp + 4));
    break;

  case SYS_FILESIZE:
    check_user_vaddr(f->esp + 4);
    result = filesize((int)*(uint32_t *)(f->esp + 4));
    break;

  case SYS_READ:
    check_user_vaddr(f->esp + 4);
    check_user_vaddr(f->esp + 8);
    check_user_vaddr(f->esp + 12);
    read((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
    break;

  case SYS_WRITE:
    check_user_vaddr(f->esp + 4);
    check_user_vaddr(f->esp + 8);
    check_user_vaddr(f->esp + 12);
    write((int)*(uint32_t *)(f->esp + 4), (void *)*(uint32_t *)(f->esp + 8), (unsigned)*((uint32_t *)(f->esp + 12)));
    break;

  case SYS_SEEK:
    check_user_vaddr(f->esp + 4);
    check_user_vaddr(f->esp + 8);
    seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
    break;

  case SYS_TELL:
    check_user_vaddr(f->esp + 4);
    result = tell((int)*(uint32_t *)(f->esp + 4));
    break;

  case SYS_CLOSE:
    check_user_vaddr(f->esp + 4);
    close((int)*(uint32_t *)(f->esp + 4));
    break;

  default:
    printf("Invalid system call number\n");
    thread_exit();
  }

  // Gán kết quả trả về vào thanh ghi EAX
  f->eax = result;
}

bool create(const char *file, unsigned initial_size)
{
  if (file == NULL)
  {
    exit(-1);
  }
  check_user_vaddr(file);
  return filesys_create(file, initial_size);
}

bool remove(const char *file)
{
  if (file == NULL)
  {
    exit(-1);
  }
  check_user_vaddr(file);
  return filesys_remove(file);
}

int open(const char *file)
{
  int i;
  struct file *fp;
  if (file == NULL)
  {
    exit(-1);
  }
  check_user_vaddr(file);
  fp = filesys_open(file);
  if (fp == NULL)
  {
    return -1;
  }
  else
  {
    for (i = 3; i < 128; i++)
    {
      if (thread_current()->fd[i] == NULL)
      {
        thread_current()->fd[i] = fp;
        return i;
      }
    }
  }
  return -1;
}

int filesize(int fd)
{
  if (thread_current()->fd[fd] == NULL)
  {
    exit(-1);
  }
  return file_length(thread_current()->fd[fd]);
}

void halt(void)
{
  shutdown_power_off();
}

// void exit (int status) {
//   printf("%s: exit(%d)\n", thread_name(), status);
//   thread_exit ();
// }

void exit(int status)
{
  int i;
  printf("%s: exit(%d)\n", thread_name(), status);
  thread_current()->exit_status = status;
  for (i = 3; i < 128; i++)
  {
    if (thread_current()->fd[i] != NULL)
    {
      close(i);
    }
  }
  thread_exit();
}

pid_t exec(const char *cmd_line)
{
  return process_execute(cmd_line);
}

int wait(pid_t pid)
{
  return process_wait(pid);
}

int read(int fd, void *buffer, unsigned size)
{
  int i;
  check_user_vaddr(buffer);
  if (fd == 0)
  {
    for (i = 0; i < size; i++)
    {
      if (((char *)buffer)[i] == '\0')
      {
        break;
      }
    }
  }
  else if (fd > 2)
  {
    if (thread_current()->fd[fd] == NULL)
    {
      exit(-1);
    }
    return file_read(thread_current()->fd[fd], buffer, size);
  }
  return i;
}

int write(int fd, const void *buffer, unsigned size)
{

  check_user_vaddr(buffer);
  if (fd == 1)
  {
    putbuf(buffer, size);
    return size;
  }
  else if (fd > 2)
  {
    if (thread_current()->fd[fd] == NULL)
    {
      exit(-1);
    }
    return file_write(thread_current()->fd[fd], buffer, size);
  }
  return -1;
}

void seek(int fd, unsigned position)
{
  if (thread_current()->fd[fd] == NULL)
  {
    exit(-1);
  }
  file_seek(thread_current()->fd[fd], position);
}

unsigned tell(int fd)
{
  if (thread_current()->fd[fd] == NULL)
  {
    exit(-1);
  }
  return file_tell(thread_current()->fd[fd]);
}

void close(int fd)
{
  struct file *fp;
  if (thread_current()->fd[fd] == NULL)
  {
    exit(-1);
  }
  fp = thread_current()->fd[fd];
  thread_current()->fd[fd] = NULL;
  return file_close(fp);
}