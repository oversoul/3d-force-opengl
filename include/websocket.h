#include <functional>
#include <iostream>
#include <thread>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> socketServer;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef std::function<void(websocketpp::connection_hdl, socketServer::message_ptr)> SocketHandler;

class WebSocketServer {
public:
  WebSocketServer() {
    m_server.set_access_channels(websocketpp::log::alevel::all);
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

    m_server.init_asio();

    m_server.set_message_handler(bind(&WebSocketServer::on_message, this, ::_1, ::_2));
  }

  void set_message_handler(SocketHandler handler) { m_handler = handler; }

  void on_message(websocketpp::connection_hdl hdl, socketServer::message_ptr msg) {
    if (m_handler == nullptr) {
      return;
    }

    m_handler(hdl, msg);
    // Echo the message back to the client
    // m_server.send(hdl, msg->get_payload(), msg->get_opcode());
  }

  void run(uint16_t port) {
    m_server.listen(port);
    m_server.start_accept();

    try {
      m_server.run();
    } catch (const std::exception &e) {
      std::cout << "Exception: " << e.what() << std::endl;
    }
  }

  void stop() { m_server.stop(); }

private:
  socketServer m_server;
  SocketHandler m_handler;
};
