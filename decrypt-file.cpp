#include <iostream>
#include <sodium.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

using namespace std;

typedef unsigned char uchar;

bool decrypt_file(const string & infile,const uchar key[crypto_secretbox_KEYBYTES]){

        vector<uchar> ciphertext;
        uchar nonce[crypto_secretbox_NONCEBYTES];
        string outfile = infile;
        string ext = ".enc";

        if((outfile.size() >= ext.size()) && (outfile.compare(outfile.size() - ext.size(),ext.size(),ext) == 0)){
                outfile.erase(outfile.size() - 4);
        }

        int readfd = open(infile.c_str(),O_RDONLY);
        if(readfd < 0){
                return false;
        }

        size_t bytes;
        char buf[128];

        if(read(readfd,nonce,sizeof(nonce)) != sizeof(nonce)){
             close(readfd);
             return false;
        }

        while((bytes = read(readfd,buf,sizeof(buf))) > 0){
                ciphertext.insert(ciphertext.end(),buf,buf + bytes);
        }

        if(ciphertext.size() < crypto_secretbox_MACBYTES){
             close(readfd);
             return false;
        }

        close(readfd);
        size_t CIPHERTEXT_LEN = ciphertext.size();
        size_t PLAINTEXT_LEN = CIPHERTEXT_LEN - crypto_secretbox_MACBYTES;

        vector<uchar> plaintext(PLAINTEXT_LEN);

        int done = crypto_secretbox_open_easy(
                plaintext.data(),
                ciphertext.data(),
                ciphertext.size(),
                nonce,
                key
        );

        if(done < 0){
           return false;
        }

        int writefd = open(outfile.c_str(),O_WRONLY | O_CREAT | O_TRUNC , 0600);

        if(writefd < 0){
           return false;
        }

        write(writefd,plaintext.data(),plaintext.size());

        close(writefd);

        return true;

}
