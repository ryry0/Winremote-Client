/*
 * Author: Ryan - David Reyes
 * This program sends keypresses and mouse movements to a windows host, which
 * emulates those movements on their desktop.
 */

#include <iostream>
#include <iomanip>
#include <cstring>
#include <getch.h>
#include <cstdlib>
#include <tcpipnix.h>
#include <inttypes.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>

struct settings_t {
  bool use_x_grabkeys;
  char * ip_address;
  unsigned short port;
};

const char CTRL_C = 0x03;
enum states_t {RUNNING, EXITING};
states_t state = EXITING;

void PrintHelp(char ** argv);
//parses the arguments given to the program at runtime
bool ParseCommands(const int argc, char ** argv, settings_t &settings);
void PrintModifiers (uint32_t mask);

int main(int argc, char ** argv) {
  settings_t program_settings;
  TCP tcp_connection;
  int server_socket = false;
  char key_pressed = '\0';

  //x variables
  xcb_connection_t      *x_connection;
  xcb_screen_t          *screen;
  xcb_window_t          win;
  xcb_generic_event_t   *event;
  xcb_generic_event_t   *repeat_event;
  xcb_key_press_event_t *key_press;
  xcb_key_press_event_t *repeat_key_press;
  xcb_keysym_t          *key_symbol;
  xcb_key_symbols_t     *symbols;
  uint32_t mask;
  uint32_t values[2];

  //initialize variables
  program_settings.use_x_grabkeys = false;
  x_connection = xcb_connect(NULL, NULL);

  //if parsing the commands is successful
  if (ParseCommands(argc, argv, program_settings)) {

    //if user wants to use x_grabkeys
    if (program_settings.use_x_grabkeys) {
      //get the screen, ask for window id and create window, map it to screen
      screen = xcb_setup_roots_iterator (xcb_get_setup(x_connection)).data;

      //initialize the mask values;
      mask      = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
      values[0] = screen->white_pixel;
      values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
        XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE;

      /* ask for the a window ID */
      win = xcb_generate_id(x_connection);

      xcb_create_window(  x_connection,                 // connection
          XCB_COPY_FROM_PARENT,         // depth same as root
          win,                          // window id
          screen->root,                 // parent window
          0, 0,                         // x and y
          150, 150,                     // width and height
          10,                           // border_width
          XCB_WINDOW_CLASS_INPUT_OUTPUT, // class
          screen->root_visual,          // visual
          mask, values);
      // map the window on the screen
      xcb_map_window (x_connection, win);
      symbols = xcb_key_symbols_alloc(x_connection);
      xcb_flush(x_connection); //ensure commmands are sent
    }
    state = RUNNING;
    /*
    //connect via tcpip
    server_socket = tcp_connection.connectToHost(program_settings.ip_address,
        program_settings.port);
    if (server_socket != false) {
      state = RUNNING;
      std::cout << "Connected to host at " << program_settings.ip_address <<
        "\n";
    }
    else {
      std::cout << "Could not connect to host!\n";
    }
  */
  }
  else { //if parsing is not successful
    std::cout << "Argument parse error.\n";
  }

  //if we are using x grabkeys
  if (program_settings.use_x_grabkeys) {
    while (state == RUNNING) {
      //grab the keys off stack by in while loop
      while ( (event = xcb_wait_for_event(x_connection)) ) {
        switch (event->response_type & ~0x80) {
          case XCB_KEY_PRESS:
            //cast the event into a keypress type
            key_press = (xcb_key_press_event_t *) event;
            printf ("key code: %x\n", key_press->detail);

            printf ("key sym: %c\n",
                xcb_key_press_lookup_keysym(symbols, key_press,
                  key_press->state));

            PrintModifiers(key_press->state);
            printf ("Key pressed in window %" PRIu32 "\n\n", key_press->event);

            break;

          case XCB_KEY_RELEASE:
            key_press = (xcb_key_press_event_t *) event;
            printf ("key code: %x\n", key_press->detail);

            printf ("key sym: %c\n",
                xcb_key_press_lookup_keysym(symbols, key_press,
                  key_press->state));

            PrintModifiers(key_press->state);
            printf ("Key released in window %" PRIu32 "\n\n", key_press->event);

            //check if key repeat
            repeat_event = xcb_poll_for_event(x_connection);
            if (repeat_event != NULL) {
              if ((repeat_event->response_type & ~0x80) == XCB_KEY_PRESS) {
                repeat_key_press = (xcb_key_press_event_t *) repeat_event;
                if (repeat_key_press->time == key_press->time)
                  printf("RUPEET KEY HAUHF\n\n");
              }
            }
            break;
        }
      }
      //check if you can actually see the keys pressed and modifiers.
      //also need to check if the program handles the switch correctly.
      //send them to host
      //check if it is ctrl d
      //exit
      //if (/*ctrl d pressed*/)
      // state = EXITING;
    }
  }
  else { //if we are not using x_grabkeys
    while (state == RUNNING) {
      key_pressed = term::getch(); //start reading keyboard and mouse;
      tcp_connection.sendData(server_socket, (char *) &key_pressed,
          sizeof(key_pressed));
      if (key_pressed == 0x04)
        state = EXITING;
      //std::cout << std::hex << (int) key_pressed << std::endl;
    }
  }


  //clean connection
  if (program_settings.use_x_grabkeys) {
    xcb_disconnect(x_connection);
    xcb_key_symbols_free(symbols);
  }

  return 0;
} //end main

void PrintHelp(char ** argv) {
  std::cout << "Usage: " << argv[0] << "[OPTION]\n";
  std::cout << "This program connects to a server hosted on a ";
  std::cout << "windows computer. Specifying the IP address is required\n";
  std::cout << "C-d to exit.\n\n";

  std::cout << "-i, --ip-address\t Required. Specifies the IPv4 address.\n";
  std::cout << "-p, --port\t Required. Specifies the communication port.\n";
  std::cout << "-x, --x_grabkeys\t Not required. Specifies whether program ";
  std::cout << "should use xgrabkey function to grab all needed keys. ";
  std::cout << "This allows natural keybindings.\n";
} //end PrintHelp

bool ParseCommands(const int argc, char ** argv, settings_t &settings) {
  bool return_value = true;
  if (argc < 2) {
    PrintHelp(argv);
    return_value = false;
  }
  else {
    for (int i = 1; i < argc; i++) {

      //check for ip flag
      if ((strcmp(argv[i], "-i") == 0) ||
          (strcmp(argv[i], "--ip-address") == 0)) {
        if (i+1 < argc) {//check existence of subsequent argument
          settings.ip_address = argv[i+1];
        }
        else {
          std::cout << "No IP specified\n";
          return_value = false;
        }
      }

      //check for port flag
      else if ((strcmp(argv[i], "-p") == 0) ||
          (strcmp(argv[i], "--port") == 0)) {
        if (i+1 < argc) {//check existence of subsequent argument
          settings.port = atoi(argv[i+1]);
        }
        else {
          std::cout << "No port specified\n";
          return_value = false;
        }
      } //end else if

      //check for port flag
      else if ((strcmp(argv[i], "-x") == 0) ||
          (strcmp(argv[i], "--x_grabkeys") == 0)) {
        settings.use_x_grabkeys = true;
      } //end else if
    } //end for
  } //end else
  return return_value;
} //end ParseCommands

void PrintModifiers (uint32_t mask) {
  const char *MODIFIERS[] = {
    "Shift", "Lock", "Ctrl", "Alt",
    "Mod2", "Mod3", "Mod4", "Mod5",
    "Button1", "Button2", "Button3", "Button4", "Button5"
  };

  printf ("Modifier mask: ");
  for (const char **modifier = MODIFIERS ; mask; mask >>= 1, ++modifier) {
    if (mask & 1) {
      printf (*modifier);
    }
  }
  printf ("\n");
} //end print_modifiers
