#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../ltd_drivers/lt_ht107.h"

int server_socket_fd, client_socket_fd;
void handle_sigint(int sig) {
  printf("Closing Server Socket FD\n");
  close(server_socket_fd);
  printf("Closing Client Socket FD\n");
  close(client_socket_fd);
  exit(0);
}

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer);

uint8_t handle_client_packet(const char* packet) {
  DeviceMsg device_msg;
  decode_packet(packet, &device_msg);
  handle_device_msg(device_msg.config.msg_type, device_msg.config.cfg2, device_msg.msg_value_buffer);
}

uint16_t port = 6543;
uint8_t linux_socket_init() {
  signal(SIGINT, handle_sigint);
  // create TCP server
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  fcntl(server_socket_fd, F_SETFL, O_NONBLOCK);
  if (server_socket_fd < 0) {
    printf("Error Creating TCP Socket\n");
    return 1;
  }
  int sockopt = 1;
  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
  int socket_bind_rc = bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (socket_bind_rc < 0) {
    printf("Error Binding TCP Socket\n");
    return 1;
  }
  int socket_listen_rc = listen(server_socket_fd, 1);
  if (socket_listen_rc < 0) {
    printf("Error Listening on Address 127.0.0.1:%d\n", port);
    return 1;
  }
  printf("Listening on Address 127.0.0.1:%d\n", port);
  init_ltd_driver(LT_HT107_PROTOCOL_VERSION, lt_ht107_driver_config, LT_HT107_DRIVER_CONFIG_SIZE);
  return 0;
}

uint8_t is_connected = 0;
struct sockaddr_in client_addr;
uint32_t client_addr_size = sizeof(client_addr);
char packet_buffer[256];
uint8_t packet_size = 0;
void linux_socket_loop() {
  if (is_connected) {
    char _char = 0x00;
    if (recv(client_socket_fd, &_char, 1, 0) <= 0)
      return;
    packet_buffer[packet_size++] = _char;
    if (packet_buffer[packet_size - 2] == '\r' && packet_buffer[packet_size - 1] == '\n') {
      handle_client_packet(packet_buffer);
      packet_size = 0;
    }
    return;
  }
  client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  if (client_socket_fd < 0)
    return;
  is_connected = 1;
  printf("Received Connection from %s:%i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}

#ifdef EXEC
int main() {
  if (linux_socket_init() != 0)
    return 1;
  while (1) {
    linux_socket_loop();
  }
  return 0;
}
#endif