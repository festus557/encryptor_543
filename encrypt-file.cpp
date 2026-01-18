#include <iostream>
#include <fcntl.h>
#include <sodium.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

using namespace std;

bool is_encrypted(const string& infile , unsigned char key){

      unsigned char nonce[crypto_secretbox_NONCEBYTES];
      randombytes_buf(nonce, sizeof(nonce));

      int fd = open(infile.c_str(),O_RDONLY);

      if(fd < 0){
          return false;
      }

      string outfile = infile + ".enc";
      char buf[128];
      size_t bytes;
      vector<unsigned char> filecontent;

      while( (bytes = read(fd,buf,sizeof(buf))) > 0){
           filecontent.insert(filecontent.end(),buf,buf + bytes);
      }

      size_t filesize = filecontent.size();
      size_t CIPHERTEXT_SIZE = filesize + crypto_secretbox_MACBYTES;
      vector<unsigned char> ciphertext(CIPHERTEXT_SIZE);

      crypto_secretbox_easy(
              ciphertext.data(),
              filecontent.data(),
              filesize,
              nonce,
              &key
      );

      int out = open(outfile.c_str(),O_WRONLY);

      if(out < 0){
          return false;
      }

      write(out,(char *)nonce,sizeof(nonce));
      write(out,(char *)ciphertext.data(),ciphertext.size());

      close(fd);
      close(out);
      return true;

}
