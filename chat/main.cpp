#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

int main()
{
  std::string ip{};
  int port{};

  std::cout << "Enter Server IP: "
  std::getline(std::cin, ip);
  
  std::cout << "Server Port: "
  std::cin >> port;
  std::cin.ignore(); //leftover newline
  
  socketaddr_in server{};
  server.sin_family{ AF_INET };
  server.sin_port{ htons(port) };
  if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) != 1){
    std::cerr << "Invalid IP address" << '\n';
    return 1;
  }

  int sock{ socket(AF_INET, SOCK_DGRAM, 0) };  //UDP socket
  
  std::thread receiver([&]() {
    char buff[1024];
    while (true){
      ssize_t n{ recvfrom(sock, buff, sizeof(buff - 1), 0, nullptr, nullptr) };
      if (n < 0) {continue};
      buff[n]{ '\0' };
      std::cout << "[server]" << buff << '\n';
    }    
  });
  receiver.detach();


}
