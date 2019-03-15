//
//  ExamplePeer.hpp
//  Multi_Level_Secure_Blockchain
//
//  Created by Kendric Hood on 3/15/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#ifndef ExamplePeer_hpp
#define ExamplePeer_hpp

#include <stdio.h>
#include "Peer.hpp"
//
// Example Peer used for network testing
//
class ExamplePeer : public Peer<ExampleMessage>{
protected:
    int counter;
public:
    ExamplePeer(std::string);
    ExamplePeer(const ExamplePeer &rhs);
    ~ExamplePeer();
    

    void                 receive();
    void                 preformComputation();
    void                 transmit();
};
#endif /* ExamplePeer_hpp */