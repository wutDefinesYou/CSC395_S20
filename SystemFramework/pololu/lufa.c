#include "lufa.h"

#define COMMAND_BUFF_LEN 16

char menu[50] = "MENU not defined";
char command[COMMAND_BUFF_LEN+1];
int index = 0;
char buffer_ready = FALSE;

void default_process(char* buffer) {
  char option[10];
  int value;
  sscanf(buffer,"%s %d",option, &value);
  printf("command %s. value %d\r\n",option, value);
  printf("process function not defined.\n\r");
}

void(*process_command)(char*) = default_process;

void initialize_lufa() {
    SetupHardware(); //This setups the USB hardware and stdio
    //process_command = default_process;
}

uint8_t user_command_ready() {

  if (buffer_ready) {
    printf("Command Buffer Ready. Not processed. Processing now.\n\r");
    USB_Mainloop_Handler();
    handle_command();
  }

  USB_Mainloop_Handler();
  char c = fgetc(stdin);
  while (c != EOF) {
    if (index >= COMMAND_BUFF_LEN) {
      return TRUE;
    }
    // if its backspace, reset index
    if (c == '\b') {
      index--;
      if (index < 0) index=0;
      printf("\b \b");
    }
    // if newline, go back 1 to write over
    /* how can it be newline??
    else if (c == '\n') {
      index--;
      if (index < 0) index=0;
    }
    */
    // if return, we have complete input
    else if (c == '\r') {
      command[index] = '\0';
      buffer_ready = TRUE;
      // echo back the return
      printf("\r\n");
      USB_Mainloop_Handler();
      return TRUE;
    }

    // otherwise, we got a char - echo it back
    printf("%c", c);
    USB_Mainloop_Handler();
    command[index] = c;
    index++;

    USB_Mainloop_Handler();
    c = fgetc(stdin);
  }
  return FALSE;
}

void handle_command( ) {
  process_command(command);
  USB_Mainloop_Handler();
  index = 0;
  buffer_ready = FALSE;
}
