#include <assert.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <zmq.h>

#include "mqcomm.hpp"

extern "C"
{
    DLL_EXPORT HMQCOMM __stdcall CreateSocket( const char * _ip, const int _port, const char * _transport )
    {
        std::string ip(_ip);
        std::string transport(_transport);
        MQComm * szmq = new MQComm(ip,_port,transport);
        return (HMQCOMM)szmq;
    }

    DLL_EXPORT int __stdcall ConnectSocket( HMQCOMM h )
    {
        assert(h != NULL);
        MQComm * szmq = (MQComm *)h;
        int result = szmq->Connect();
        return result;
    }

    DLL_EXPORT int __stdcall DisconnectSocket( HMQCOMM h )
    {
        assert(h != NULL);
        MQComm * szmq = (MQComm *)h;
        szmq->Disconnect();
        delete szmq;
        return 1;
    }

    DLL_EXPORT int __stdcall SendRequest( HMQCOMM h, const char * _request )
    {
        assert(h != NULL);
        MQComm * szmq = (MQComm *)h;
        int result = szmq->SendRequest(_request);
        return result;
    }
}

MQComm::MQComm( std::string ip, int port, std::string transport ) :
    ip(ip), port(port), transport(transport)
{
    /* Save log files */
    std::string log_path = GetLogPath();
    freopen( log_path.c_str(), "a", stderr );

    std::clog << "Received initialization: " << transport << "://" << ip << ":" << port << std::endl;
}

//Connect to socket
int MQComm::Connect( void )
{
    /* Create context */
    context = zmq_ctx_new ();
    if (context == NULL) {
        std::cerr << "Error " << ERROR_CONTEXT << " - Could not create context." << std::endl;
        return ERROR_CONTEXT;
    }
    /* Create socket */
    socket = zmq_socket (context, ZMQ_REQ);
    if (socket == NULL) {
        std::cerr << "Error " << ERROR_SOCKET << " - Could not create socket." << std::endl;
        return ERROR_SOCKET;
    }
    /* Connect to socket */
    std::string connection_string = transport+"://"+ip+":"+std::to_string(port);
    int rc = zmq_connect (socket, connection_string.c_str());
    if (rc != 0) {
        std::cerr << "Error " << ERROR_CONNECT << " - Could not connect the socket to the endpoint: "
            << zmq_strerror (zmq_errno ()) << " - " << connection_string << std::endl;
        return ERROR_CONNECT;
    }

    std::clog << "Successfully connected to " << connection_string << std::endl;
    return 1;
}

// Send a request with the tick information
int MQComm::SendRequest( const char * _request )
{
    /* Create a new message */
    std::string request(_request);
    int request_size = request.size();
    zmq_msg_t zmq_request;
    int rc = zmq_msg_init_size (&zmq_request, request_size);
    if (rc != 0) {
        std::cerr << "Error " << ERROR_MSG_INIT << " - Could not init request message." << std::endl;
        return ERROR_MSG_INIT;
    }

    /* Create an empty 0MQ message for the reply */
    zmq_msg_t reply;
    rc = zmq_msg_init (&reply);
    if (rc != 0) {
        std::cerr << "Error " << ERROR_MSG_INIT << " - Could not init reply message." << std::endl;
        return ERROR_MSG_INIT;
    }

    /* Fill in message content */
    memcpy (zmq_msg_data (&zmq_request), request.c_str(), request_size);
    /* Send the message to the socket */
    rc = zmq_msg_send (&zmq_request, socket, 0);
    if (rc != request_size) {
        std::cerr << "Error " << ERROR_MSG_SEND << " - Could not send request message: " << request << " - size: " << request_size << std::endl;
        return ERROR_MSG_SEND;
    }
    std::clog << "Sent request: " << request << std::endl;

    /* Block until a message is available to be received from socket */
    rc = zmq_msg_recv (&reply, socket, 0);
    if (rc == -1) {
        std::cerr << "Error " << ERROR_MSG_RECV << " - Could not receive reply message." << std::endl;
        return ERROR_MSG_RECV;
    }
    std::string str_reply = std::string(static_cast<char*>(zmq_msg_data (&reply)), zmq_msg_size (&reply));
    std::clog << "Received reply: " << str_reply << std::endl;
    /* Release message */
    zmq_msg_close (&reply);

    return 1;
}

void MQComm::Disconnect( void )
{
    /* Deinitializations */
    zmq_close (socket);
    zmq_ctx_destroy (context);
    std::clog << "Socket disconnected" << std::endl;
    fclose(stderr);
}

std::string GetLogPath( void )
{
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            __FILE__,
            &hm))
    {
        int ret = GetLastError();
        std::cerr << "Error - GetModuleHandle returned " << ret;
    }
    GetModuleFileNameA(hm, path, sizeof(path));

    std::string return_path = std::string(path);
    return_path.replace(return_path.length() - 4, 4, "_log.txt");

    return return_path;
}
