#include <utils.h>

NEMUState nemu_state = { .state = NEMU_STOP };

int is_exit_status_bad() {
  int good = (nemu_state.state == NEMU_END && nemu_state.halt_ret == 0) ||
    (nemu_state.state == NEMU_QUIT);
  if (good){
    return 0;
  }else {
    return nemu_state.state;
  }
  
  //return !good;
}
