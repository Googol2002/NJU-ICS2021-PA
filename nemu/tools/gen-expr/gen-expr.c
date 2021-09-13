#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char output_buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = (%s); "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


uint32_t perform(uint32_t n1, uint32_t n2, char op){
  switch (op){
    case '+':
      return n1 + n2;
    case '-':
      return n1 - n2;
    case '*':
      return n1 * n2;
    case '/':
      return n1 / n2;
    default:
      return 0;
  }
}

uint32_t choose(uint32_t  n){
  return rand() % n;
}

void gen(char c){
  char cha_buffer[2] = {c, '\0'};
  strcat(buf, cha_buffer);
}

char gen_rand_op(){
  switch (choose(4)){
    case 0:
      gen('+');
      return '+';
    case 1:
      gen('-');
      return '-';
    case 2:
      gen('*');
      return '*';
    case 3:
      gen('/');
      return '/';
  }
  return ' ';
}

uint32_t gen_num(){
  char num_buffer[1024];
  num_buffer[0] = '\0';
  uint32_t number = rand() % 100 + 1;
  sprintf(num_buffer ,"%du", number);
  strcat(buf, num_buffer);
  return number;
}

void generate_output(){
  int j = 0;
  for (int i = 0; buf[i] != '\0'; ++i){
    if (buf[i] != 'u'){
      output_buf[j++] = buf[i];
    }
  }
  output_buf[j] = '\0';
}

static uint32_t gen_rand_expr() {
  if (strlen(buf) > 65536 - 1000){
    gen('(');
    uint32_t number = gen_num();
    gen(')');
    return number;
  }

  switch (choose(3)) {
    case 0: 
      return gen_num();
    case 1: 
      gen('(');
      uint32_t number = gen_rand_expr();
      gen(')'); 
      return number;
    default: {
      uint32_t n1 = gen_rand_expr();
      char op = gen_rand_op();
      int index = strlen(buf);
      uint32_t n2 = gen_rand_expr();
      if (op == '/'){
        while (n2 == 0u){
          buf[index] = '\0';
          n2 = gen_rand_expr();
        }
      }
    
      return perform(n1, n2, op);
    }
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
    buf[0] = '\0';
    gen_rand_expr();
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
    int b = fscanf(fp, "%d", &result);
    pclose(fp);
    if (b != 1)
      continue;

    generate_output();
    printf("%u %s\n", result, output_buf);
  }
  return 0;
}
