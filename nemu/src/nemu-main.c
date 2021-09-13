#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

void test_cmd_p();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  IFDEF(CONFIG_DEBUG, test_cmd_p());
  IFDEF(CONFIG_DEBUG, Log("通过200个测试样例"));

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}

word_t expr(char *e, bool *success);

#define TEST_CMD_P_PATH "../testset/p.txt"
void test_cmd_p(){
  char buffer[65535];
  char *expression;
  FILE *fp = fopen(TEST_CMD_P_PATH, "r");
  assert(fp != NULL);

  char* input = fgets(buffer, ARRLEN(buffer), fp);
  while (input != NULL){
    input[strlen(input) - 1] = '\0'; // 删去末尾'\n'
    uint32_t ans = 0;
    bool success = false;
    char* ans_text = strtok(input, " ");
    sscanf(ans_text, "%u", &ans);
    expression = input + strlen(ans_text) + 1;
    printf("This is %u %s \n", ans, expression);
    uint32_t result = expr(expression, &success);
    assert(result == ans);
    input = fgets(buffer, ARRLEN(buffer), fp);
  }
}
