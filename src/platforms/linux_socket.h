#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../ltd_drivers/lt_ch000.h"

int server_socket_fd, client_socket_fd;

void handle_sigint(int sig) {
  printf("Closing Server Socket FD\n");
  close(server_socket_fd);
  printf("Closing Client Socket FD\n");
  close(client_socket_fd);
  exit(0);
}

void handle_emu_spi_device_msg(uint8_t msg_type, uint8_t* msg_value_buffer);

#ifndef EMU_SPI_MSG_HANDLER
void handle_emu_spi_device_msg(uint8_t msg_type, uint8_t* msg_value_buffer) {
  printf("Msg Type: %d\n", msg_type);
  float decoded_value = 0;
  ((uint8_t*)&decoded_value)[0] = msg_value_buffer[0];
  ((uint8_t*)&decoded_value)[1] = msg_value_buffer[1];
  ((uint8_t*)&decoded_value)[2] = msg_value_buffer[2];
  ((uint8_t*)&decoded_value)[3] = msg_value_buffer[3];
  printf("Msg Value: %0.3f\n", decoded_value);
}
#endif

uint8_t handle_client_packet(const char* packet) {
  DeviceMsg device_msg;
  decode_packet(packet, &device_msg);
  handle_emu_spi_device_msg(device_msg.config.msg_type, device_msg.msg_value_buffer);
}

int server_thread() {
  signal(SIGINT, handle_sigint);

  // create TCP server
  uint16_t port = 6543;
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_fd < 0) {
    printf("Error Creating TCP Socket\n");
    return server_socket_fd;
  }
  int socket_bind_rc = bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (socket_bind_rc < 0) {
    printf("Error Binding TCP Socket\n");
    return socket_bind_rc;
  }

  init_ltd_driver(0x8787, lt_ch000_driver_config, LT_CH000_DRIVER_CONFIG_SIZE);

// listen for TCP connection
server_listen:
  int socket_listen_rc = listen(server_socket_fd, 1);
  if (socket_listen_rc < 0) {
    printf("Error Listening on Address 127.0.0.1:%d\n", port);
    return socket_listen_rc;
  }
  printf("Listening on Address 127.0.0.1:%d\n", port);
  struct sockaddr_in client_addr;
  uint32_t client_addr_size = sizeof(client_addr);
  client_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
  if (client_socket_fd < 0) {
    printf("Error Accepting Connection\n");
    return client_socket_fd;
  }
  printf("Received Connection from %s:%i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

  // client loop
  char packet[256];
  uint8_t packet_size = 0;
  while (1) {
    char _char = 0x00;
    if (recv(client_socket_fd, &_char, 1, 0) <= 0) {
      printf("Error Reading from Client %s:%i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      goto server_listen;
    }
    packet[packet_size++] = _char;
    if (packet[packet_size - 2] == '\r' && packet[packet_size - 1] == '\n') {
      handle_client_packet(packet);
      packet_size = 0;
    }
  }

  return 0;
}

#ifdef EXEC
int main() {
  return server_thread();
}
#endif