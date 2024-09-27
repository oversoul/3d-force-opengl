#pragma once

#include <cstdint>
#include <functional>
#include <string>

typedef std::function<void(std::string)> SocketHandler;

class WebSocketServer {
public:
  WebSocketServer();

  void set_message_handler(SocketHandler handler);

  void run(uint16_t port);
  void stop();

private:
  class Impl;
  Impl *pImpl;
};
