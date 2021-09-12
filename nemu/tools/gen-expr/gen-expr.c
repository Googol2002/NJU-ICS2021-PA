#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


int choose(int n){
  return rand() % n;
}

void gen(char c){
  char cha_buffer[2] = {c, '\0'};
  strcat(buf, cha_buffer);
}

void gen_rand_op(){
  switch (choose(4)){
    case 0:
      gen('+');
      break;
    case 1:
      gen('-');
      break;
    case 2:
      gen('*');
      break;
    case 3:
      gen('/');
      break;
  }
}

void gen_num(){
  char num_buffer[1024];
  num_buffer[0] = '\0';
  sprintf(num_buffer ,"%d", rand() % 10000);
  strcat(buf, num_buffer);
}

static void gen_rand_expr() {
  buf[0] = '\0';
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    printf("expr: %s \n", buf);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if (fscanf(fp, "%d", &result) != 1)
      assert(0);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
