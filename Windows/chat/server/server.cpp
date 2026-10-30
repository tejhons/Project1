#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

/* Find IP address used on local network */
std::string getIP(){
  
  std::string ip{ "127.0.0.1" };
  SOCKET s{ socket(AF_INET, SOCK_DGRAM, 0) };
  sockaddr_in dns{};

  dns.sin_family = AF_INET;
  dns.sin_port = htons(80);
  inet_pton(AF_INET, "8.8.8.8", &dns.sin_addr);

  if (connect(s, (sockaddr *)&dns, sizeof(dns)) == 0) {
    sockaddr_in me{};
    int len{ sizeof(me) };
    getsockname(s, (sockaddr *)&me, &len);
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &me.sin_addr, buf, sizeof(buf));
    ip = buf;
  }
  closesocket(s);
  return ip;
}

/*--------------------------MAIN-------------------------*/
int main()
{
  WSADATA wsa{};
  if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
    std::cerr << "WSAStartup Failed" << '\n';
    return 1;
  }

  Socket sock{ socket(AF_INET, SOCK_DGRAM, 0) };  //UDP Socket
  
  sockaddr_in addr{};
}
