//+------------------------------------------------------------------+
//|                                                     MQCommEA.mq4 |
//|                                                               RW |
//|                                                  randomwalks.xyz |
//+------------------------------------------------------------------+
#property copyright "RW"
#property link      "randomwalks.xyz"
#property version   "1.00"
#property strict

//+------------------------------------------------------------------+
//| DLL imports                                                      |
//+------------------------------------------------------------------+
#import "mqcomm.dll"
int CreateSocket( const uchar &ip[], const int port, const uchar &transport[] );
int ConnectSocket( const int h );
int SendRequest( const int h, const uchar &request[] );
int DisconnectSocket( const int h );
#import

int handler;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//---
   Print("OnInit - Request");
   string ip_str = "10.0.2.2";
   uchar ip[];
   StringToCharArray(ip_str, ip);
   int port = 5555;
   string transport_str = "tcp";
   uchar transport[];
   StringToCharArray(transport_str, transport);
   handler = CreateSocket(ip,5555,transport);
   int rc = ConnectSocket(handler);
   string request_str = "Connection established with EA";
   uchar request[];
   StringToCharArray(request_str, request);
   //int reply = SendRequest(handler,request);
   Print("OnInit - Respuesta: ",IntegerToString(rc));
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//---
   Print("OnDeinit - Request");
   int rc = DisconnectSocket(handler);
   Print("OnDeinit - Reply: ",IntegerToString(rc));
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   Print("OnTick - Request");
   string symbol = Symbol();
   int resolution = Period();
   datetime dt = iTime(symbol,resolution,1);
   double open = iOpen(symbol,resolution,1);
   double high = iHigh(symbol,resolution,1);
   double low = iLow(symbol,resolution,1);
   double close = iClose(symbol,resolution,1);
   long volume = iVolume(symbol,resolution,1);

   string request_str = StringConcatenate("{\"s\":\"",symbol,
                                          "\",\"t\":\"CURRENCY\"",
                                          ",\"r\":",resolution,
                                          ",\"d\":\"",TimeToStrFormatted(dt),
                                          "\",\"o\":",open,
                                          ",\"h\":",high,
                                          ",\"l\":",low,
                                          ",\"c\":",close,
                                          ",\"v\":",volume,"}");
   uchar request[];
   StringToCharArray(request_str, request);
   int reply = SendRequest(handler,request);
   Print("OnTick - Reply: ",IntegerToString(reply));
  }
  
string TimeToStrFormatted(datetime dt)
  {
   string dt_string = TimeToStr(dt); // yyyy.mm.dd hh:mm:ss
   string dt_string_formatted =
      StringSubstr(dt_string, 0, 4) + //yyyy
      StringSubstr(dt_string, 5, 2) + //mm
      StringSubstr(dt_string, 8, 5) + //dd hh
      StringSubstr(dt_string, 14, 2) + //mm
      StringSubstr(dt_string, 17, 2); //ss
   
   return(dt_string_formatted);
  }
//+------------------------------------------------------------------+
