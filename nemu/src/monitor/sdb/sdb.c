#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <utils.h>

#include "sdb.h"
#include "../ftrace/ftracer.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_si(char *args){
  int n = 1;
  if (args != NULL){
    sscanf(args, "%d", &n);
  }
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args){
  if (args == NULL){
    printf("info指令 缺少参数\n");
  }else if (strcmp(args, "r") == 0){
    isa_reg_display();
  }else if (strcmp(args, "w") == 0){
    watchpoint_display();
  }else {
    printf("未知的参数 [%s] \n", args);
  }

  return 0;
}

static int cmd_x(char *args){
  char *arg = strtok(NULL, " ");
  int n = -1;
  bool success = true;
  paddr_t base = 0x80000000; 
  sscanf(arg, "%d", &n); //对于n不支持表达式，只支持常量。
  arg = args + strlen(arg) + 1;
  //sscanf(arg, "%i", &base);
  base = expr(arg, &success);
  if (!success) {
    return 0;
  }
  

  for (int i = 0; i < n; ++i){
    if (i % 4 == 0){
      printf ("\n\e[1;36m%#x: \e[0m\t", base + i * 4);
    }
    for (int j = 0; j < 4; ++j){
      uint8_t* pos = guest_to_host(base + i * 4 + j);
      printf("%.2x ", *pos);
    }
    printf("\t");
  }
  printf("\n");
  return 0;
}

static int cmd_px(char *args){
  bool success;
  uint32_t v = expr(args, &success);
  if (success)
    printf("%s = \e[1;36m%#.8x\e[0m\n", args, v);
  return 0;
}

static int cmd_p(char *args){
  bool success;
  uint32_t v = expr(args, &success);
  if (success)
    printf("%s = \e[1;36m%u\e[0m\n", args, v);
  return 0;
}

static int cmd_w(char *args){
  bool success = true;
  WP *point = new_wp(args, &success);
  if (!success){
    printf("Some thing wrong happend.\n");
  }else {
    printf("Created a \e[1;36mWatchPoint(NO.%d)\e[0m: %s \n", point->NO, point->condation);
  }
  return 0;
}

static int cmd_d(char *args){
  int NO;
  sscanf(args, "%d", &NO);
  free_wp(NO);
  return 0;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_s(char *args) {
  print_stack_trace();
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "si [N] 让程序单步执行N条指令后暂停执行,当N没有给出时, 缺省为1", cmd_si},
  { "info", "info r 打印寄存器状态, info w 打印监视点信息", cmd_info},
  { "x", "x N EXPR 求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节", cmd_x},
  { "p", "p EXPR 求出表达式EXPR的值", cmd_p},
  { "w", "w EXPR 当表达式EXPR的值发生变化时, 暂停程序执行", cmd_w},
  { "d", "d N 删除序号为N的监视点", cmd_d},
  { "s", "s 打印当前函数调用栈", cmd_s},
  { "px", "功能同p，但是以十六进制输出结果", cmd_px}
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
