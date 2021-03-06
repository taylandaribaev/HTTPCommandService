//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTPS_SERVER_HPP
#define HTTPS_SERVER_HPP

#ifdef WITH_SSL

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace openrc {
namespace http {
namespace server {

/// The top-level class of the HTTP server.
class ssl_server
{
public:
    ssl_server(const ssl_server&) = delete;
    ssl_server& operator=(const ssl_server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit ssl_server(const std::string& address, const std::string& port,
      const std::string& doc_root);

    /// Run the server's io_service loop.
    void run();

private:
    /// Perform an asynchronous accept operation.
    void do_accept();

    /// Wait for a request to stop the server.
    void do_await_stop();

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The connection manager which owns all live connections.
    connection_manager connection_manager_;

    /// The next socket to be accepted.
    boost::asio::ip::tcp::socket socket_;

    /// The handler for all incoming requests.
    request_handler request_handler_;

    boost::asio::ssl::context context_;
};

} // namespace server
} // namespace http
} // namespace openrc.

#endif
#endif // HTTP_SERVER_HPP
