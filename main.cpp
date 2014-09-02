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
#include <csignal>
#include <tcpipnix.h>

struct settings_t {
  char * ip_address;
  unsigned short port;
};

const char CTRL_C = 0x03;
enum states_t {RUNNING, EXITING};
states_t state = EXITING;

void PrintHelp(char ** argv);
bool ParseCommands(const int argc, char ** argv, settings_t &settings);
void signal_handler(int signal);

int main(int argc, char ** argv) {
  settings_t program_settings;
  TCP tcp_connection;
  int server_socket = false;
  char key_pressed = '\0';
  signal (SIGINT, signal_handler);

  if (ParseCommands(argc, argv, program_settings)) {
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
  }
  else
    std::cout << "Argument parse error.\n";

  while (state == RUNNING ) {
    key_pressed = term::getch(); //start reading keyboard and mouse;
    tcp_connection.sendData(server_socket, (char *) &key_pressed,
        sizeof(key_pressed));
    //std::cout << std::hex << (int) key_pressed << std::endl;
  }

  if (server_socket != false) {
    //send special key to exit.
    key_pressed = 0x03;
    tcp_connection.sendData(server_socket, (char *) &key_pressed,
        sizeof(key_pressed));
  }

  return 0;
} //end main

void PrintHelp(char ** argv) {
  std::cout << "Usage: " << argv[0] << "[OPTION]\n";
  std::cout << "This program connects to a server hosted on a ";
  std::cout << "windows computer. Specifying the IP address is required\n\n";

  std::cout << "-i, --ip-address\t Required. Specifies the IPv4 address.\n";
  std::cout << "-p, --port\t Required. Specifies the communication port.\n";
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
    } //end for
  } //end else
  return return_value;
} //end ParseCommands

void signal_handler(int signal) {
  state = EXITING;
}
