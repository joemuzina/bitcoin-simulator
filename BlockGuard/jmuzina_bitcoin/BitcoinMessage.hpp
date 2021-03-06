#ifndef JMUZINA_BCOINMSG
#define JMUZINA_BCOINMSG

#include "../Common/Block.hpp"

struct BitcoinMessage {
    Block block;
    std::string peerId;
    int length;

    BitcoinMessage() {
        block = Block();
        peerId = -1;
        length = 0;
    }

    BitcoinMessage(const std::string id) {
        block = Block();
        peerId = id;
        length = 1;
    }

    BitcoinMessage(const Block& newBlock, const std::string id, const int len) {
        block = newBlock;
        peerId = id;
        length = len;
    }

    // Copy constructor
    BitcoinMessage(const BitcoinMessage& copy) { 
        block = copy.block; 
        peerId = copy.peerId;
        length = copy.length;
    }

    // Equal operator overload
    BitcoinMessage& operator=(const BitcoinMessage& rhs) {
        block = rhs.block;
        peerId = rhs.peerId;
        length = rhs.length;
        return *this;
    }
};

class splitHash {
public:
    splitHash() {
        hash = "";
        nonce = -1;
    }

    splitHash(std::string);

    splitHash(long newNonce, std::string newHash) {
        nonce = newNonce;
        hash = newHash;
    }

    splitHash(const splitHash& copy) {
        nonce = copy.nonce;
        hash = copy.hash;
    }

    splitHash& operator=(const splitHash& rhs) {
        nonce = rhs.nonce;
        hash = rhs.hash;
        return *this;
    }

    long long   getNonce    ()                      const { return nonce; };
    std::string getHash     ()                      const { return hash; };
    void        setNonce    (long long newNonce)          { nonce = newNonce; };
    void        setHash     (std::string newHash)         { hash = newHash; };
private:
    long long nonce;
    std::string hash;
};

#endif 