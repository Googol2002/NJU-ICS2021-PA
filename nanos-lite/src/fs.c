#include <fs.h>
#include <string.h>
#include <stdint.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};
//, FD_EVENT, FD_DISPINFO

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]    = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT]   = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR]   = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB]   = {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = config.width * config.height * sizeof(uint32_t);

  // for (int i = 5; i < sizeof(file_table) / sizeof(Finfo); ++i){
  //   // 我不确定会不会自动刷为0，不如再做一次
  //   file_table[i].open_offset = 0;
  //   file_table[i].write = NULL;
  //   file_table[i].read = NULL;
  // }
}

//flag, mode 被忽视
int fs_open(const char *pathname, int flags, int mode){
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); ++i){
    if (strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  Finfo *info = &file_table[fd];
  size_t real_len;
  
  //assert(info->open_offset + len <= info->size);
  if (info->read){
    real_len = info->read(buf, info->open_offset, len);
    info->open_offset += real_len;
  }else {
    real_len = info->open_offset + len <= info->size ?
    len : info->size - info->open_offset;
    ramdisk_read(buf, info->disk_offset + info->open_offset, real_len);
    info->open_offset += real_len;
  }

  return real_len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  //TODO: STDOUT添加支持
  Finfo *info = &file_table[fd];
  size_t real_len;
  
  if (info->write){
    real_len = info->write(buf, info->open_offset, len);
    info->open_offset += real_len;
  }else {
    assert(info->open_offset + len <= info->size);
    ramdisk_write(buf, info->disk_offset + info->open_offset, len);
    real_len = len;
    info->open_offset += len;
  }

  return real_len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  Finfo *info = &file_table[fd];

  switch(whence){
    case SEEK_CUR:
      assert(info->open_offset + offset <= info->size);
      info->open_offset += offset;
      break;

    case SEEK_SET:
      assert(offset <= info->size);
      info->open_offset = offset;
      break;

    case SEEK_END:
      assert(offset <= info->size);
      info->open_offset = info->size + offset;
      break;

    default:
      assert(0);
  }

  return info->open_offset;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}