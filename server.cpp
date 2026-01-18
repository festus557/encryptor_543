#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 50000

using namespace std;

/* Initialize OpenSSL */
SSL_CTX* init_ssl()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD* method = TLS_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);

    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    return ctx;
}

/* Create listening socket */
int create_server_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(1);
    }

    return sock;
}

int main()
{
    SSL_CTX* ctx = init_ssl();

    /* Load certificate and key */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    int serverfd = create_server_socket();
    cout << "Listening on: 0.0.0.0:" << PORT << endl;

    while (true) {
        int clientfd = accept(serverfd, nullptr, nullptr);
        if (clientfd < 0) {
            perror("accept");
            continue;
        }

        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientfd);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(clientfd);
            continue;
        }

        /* ---- READ HTTP REQUEST (STOP AT \r\n\r\n) ---- */
        vector<char> request;
        char buffer[1024];
        int bytes;

        while ((bytes = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
            request.insert(request.end(), buffer, buffer + bytes);

            if (request.size() >= 4) {
                if (string(request.end() - 4, request.end()) == "\r\n\r\n") {
                    break;
                }
            }
        }

        cout << "[+] Request received\n";
        cout<<"\n[+]Req:\n";
        cout.write(request.data(),request.size());
        cout<<endl;

        /* ---- CLEAN SHUTDOWN ---- */
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(clientfd);
    }

    close(serverfd);
    SSL_CTX_free(ctx);
    return 0;
}
