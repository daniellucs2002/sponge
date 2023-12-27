#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <random>

const uint16_t portnum = ((std::random_device()()) % 50000) + 1025;

class TCPSocket {
private:
    int sockfd;

public:
    TCPSocket() : sockfd(-1) {
        // socket function is used to create a socket
        // AF_INET: IPv4 family, SOCK_STREAM: TCP socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            throw std::runtime_error("Error creating socket");
        }
    }

    // associate a socket with an internet address
    void bind(const char* ip, uint16_t port) {
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);  // host to network, unsigned short int
        if (inet_pton(AF_INET, ip, &(addr.sin_addr)) <= 0) {  // presentation format to network byte order
            throw std::runtime_error("Error converting IP address");
        }

        // '::' scope resolution operator: call the standard library's bind function
        // <const struct sockaddr*>: both IPv4 and IPv6 addresses
        if (::bind(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) == -1) {
            throw std::runtime_error("Error binding socket");
        }
    }

    // accept incoming connection requests from clients, backlog: size of the queue for pending connections
    void listen(int backlog) {
        if (::listen(sockfd, backlog) == -1) {
            throw std::runtime_error("Error listening on socket");
        }
    }

    // called on listening socket; once a connection request is received, it is accepted by the server
    // return value: used to communicate with the connected client, new socket
    TCPSocket accept() {
        int new_sockfd = ::accept(sockfd, nullptr, nullptr);
        if (new_sockfd == -1) {
            throw std::runtime_error("Error accepting connection");
        }
        return TCPSocket(new_sockfd);
    }

    // establish a connection to a remote server
    void connect(const char* ip, uint16_t port) {
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &(addr.sin_addr)) <= 0) {
            throw std::runtime_error("Error converting IP address");
        }

        if (::connect(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) == -1) {
            throw std::runtime_error("Error connecting to server");
        }
    }

    ssize_t write(const char* data) {
        return ::send(sockfd, data, std::strlen(data), 0);
    }

    ssize_t read(char* buffer, size_t length) {
        return ::recv(sockfd, buffer, length, 0);
    }

    void close() {
        if (sockfd != -1) {
            ::close(sockfd);  // terminates the local end of the network connection
            sockfd = -1;
        }
    }

    ~TCPSocket() {
        close();
    }

    TCPSocket(int socket) : sockfd(socket) {}  // don't need to create a socket
};

int main() {
    try {
        // create a TCP socket, bind it to a local address, and listen
        TCPSocket sock1{};
        sock1.bind("127.0.0.1", portnum);
        sock1.listen(1);

        // create another socket and connect to the first one
        TCPSocket sock2{};
        sock2.connect("127.0.0.1", portnum);

        // accept the connection
        TCPSocket sock3 = sock1.accept();
        sock3.write("hi there");

        char buffer[1024];
        ssize_t recvd = sock2.read(buffer, sizeof(buffer));
        buffer[recvd] = '\0';
        std::cout << "Received from sock2: " << buffer << std::endl;

        sock2.write("hi yourself");

        recvd = sock3.read(buffer, sizeof(buffer));
        buffer[recvd] = '\0';
        std::cout << "Received from sock3: " << buffer << std::endl;

        sock1.close();  // don't need to accept any more connections
        sock2.close();  // you can call close(2) on a socket
        sock3.close();  // close the accepted connection
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
