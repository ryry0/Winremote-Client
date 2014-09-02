/*
 * Author: Ryan - David Reyes
 * This program sends keypresses and mouse movements to a windows host, which
 * emulates those movements on their desktop.
 */

#include <iostream>
#include <getch.h>
#include <tcpipnix.h>

struct settings_t {
  char * ip_address;
  unsigned short port;
}

enum states_t {RUNNING, EXITING};

void PrintHelp(char ** argv);
bool ParseCommands(const int argc, char ** argv, settings_t &settings);

int main(int argc, char ** argv) {
  settings_t program_settings;
  TCP tcp_connection;
  int server_socket;
  char key = "\0";
  states_t state = EXITING;

  ParseCommands(argc, argv, program_settings);
  server_socket = tcp_connection.connectToHost(program_settings.ip_address,
      program_settings.port);

  if (server_socket != false)
    state = RUNNING;

  if (state == RUNNING) {
    key = getch(); //start reading keyboard and mouse;
    tcp_connection.sendData(server_socket, (char *) &key, sizeof(key));
    //special key to exit.
  }
  else {
    std::cout << "Could not connect to host!\n";
  }

  return 0;
}

void PrintHelp(char ** argv) {
  std::cout << "Usage: " << argv[0] << "[OPTION]\n";
  std::cout << "This program connects to a server hosted on a ";
  std::cout << "windows computer. Specifying the IP address is required\n\n";

  std::cout << "-i, --ip-address\t Required. Specifies the IPv4 address.\n";
  std::cout << "-p, --port\t Required. Specifies the communication port.\n";
}

bool ParseCommands(const int argc, char ** argv, settings_t &settings) {
  if (argc < 2) {
    PrintHelp();
  }
  else {
    for (unsigned int i = 1; i < argc; i++) {

      //check for ip flag
      if ((strcmp(argv[i], "-i") == 0) ||
          (strcmp(argv[i], "--ip-address") == 0)) {
        if (i+1 < argc) //check existence of subsequent argument
          settings.ip_address = argv[i+1];
        else
          std::cout << "No IP specified\n";
      }

      //check for port flag
      else if ((strcmp(argv[i], "-p") == 0) ||
          (strcmp(argv[i], "--port") == 0)) {
        if (i+1 < argc) //check existence of subsequent argument
          settings.port = argv[i+1];
        else
          std::cout << "No port specified\n";
      }

    }
  }
}
