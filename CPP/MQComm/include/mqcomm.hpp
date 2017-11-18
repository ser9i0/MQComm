#ifndef MQCOMM_HPP
#define MQCOMM_HPP

#if defined BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

class MQComm {
    public:
        MQComm( std::string ip, int port, std::string transport );
        int  Connect( void );
        void Disconnect( void );
        int  SendRequest( const char * _request );

    private:
        std::string ip;
        std::string transport;
        int port;
        void * context;
        void * socket;
};

std::string GetLogPath( void );

typedef const void * HMQCOMM;
extern "C"
{
    DLL_EXPORT HMQCOMM __stdcall CreateSocket( const char* _ip, const int _port, const char* _transport );
    DLL_EXPORT int __stdcall ConnectSocket( HMQCOMM h );
    DLL_EXPORT int __stdcall SendRequest( HMQCOMM h, const char * _request );
    DLL_EXPORT int __stdcall DisconnectSocket( HMQCOMM h );
}

enum ERROR_TYPE {
    ERROR_CONTEXT = 200,
    ERROR_SOCKET,
    ERROR_CONNECT,
    ERROR_MSG_INIT,
    ERROR_MSG_SEND,
    ERROR_MSG_RECV
};

#endif // MQCOMM_HPP
