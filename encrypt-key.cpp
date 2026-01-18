#include <iostream>
#include <cstdlib>
#include <sodium.h>
#include <vector>

using namespace std;

vector<unsigned char>  key_encrypt(unsigned char * publickey){

       vector<unsigned char> encrypted_key(
               crypto_box_SEALBYTES + crypto_secretbox_KEYBYTES
       );

       unsigned char key[crypto_secretbox_KEYBYTES];
       randombytes_buf(key,sizeof(key));

       crypto_box_seal(
              encrypted_key.data(),
              key,
              sizeof(key),
              publickey
      );

      sodium_memzero(key,sizeof(key));

      return encrypted_key;

}
