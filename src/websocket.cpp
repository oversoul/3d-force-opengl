#include "websocket.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> socketServer;

class WebSocketServer::Impl {

public:
  socketServer m_server;
  SocketHandler m_handler;
};

WebSocketServer::WebSocketServer() : pImpl(new Impl) {
  pImpl->m_server.set_access_channels(websocketpp::log::alevel::all);
  pImpl->m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

  pImpl->m_server.init_asio();

  pImpl->m_server.set_message_handler([&](websocketpp::connection_hdl, socketServer::message_ptr msg) {
    if (pImpl->m_handler == nullptr) {
      return;
    }

    pImpl->m_handler(msg->get_payload());
    // Echo the message back to the client
    // m_server.send(hdl, msg->get_payload(), msg->get_opcode());
  });
}

void WebSocketServer::set_message_handler(SocketHandler handler) { pImpl->m_handler = handler; }

void WebSocketServer::run(uint16_t port) {
  pImpl->m_server.listen(port);
  pImpl->m_server.start_accept();

  try {
    pImpl->m_server.run();
  } catch (const std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}

void WebSocketServer::stop() { pImpl->m_server.stop(); }
