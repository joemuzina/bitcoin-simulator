//
// Created by snwbr on 11/6/2019.
//
#include "quickshards.h"

quickShards::quickShards(int peers, int intersections, double tolerance) {
    srand(time(nullptr));
    _intersections = intersections;
    int quorums = 0.5 * sqrt(8 * peers + _intersections) / sqrt(_intersections) + 1;

    //Check quorum size, decrease size if needed
    while (quorums * (quorums - 1) / 2 * _intersections > peers)
        --quorums;
    _reserve = peers - (quorums * (quorums - 1) / 2 * _intersections);

    // Create shards
    _peersPerShard = (quorums - 1) * _intersections;
    int f = (_peersPerShard-1) * tolerance;
    for (int i = 0; i < quorums; ++i) {
        _shards[i] = quickPBFT(i, _peersPerShard, quorums-1, f);
    }

    // Create intersections
    for (int workQuorum = 0; workQuorum < quorums; ++workQuorum){
        for (int offset = 0; offset < _intersections; ++offset ){
            for (int workPeer=workQuorum;workPeer< (_peersPerShard/_intersections); ++workPeer){
                //std::cout << workQuorum << ", " << workPeer+peersPerQuorum*offset << std::endl;
                quickPeer* tmpA = _shards[workQuorum].getPeer(workPeer+_peersPerShard/_intersections*offset);
                quickPeer* tmpB = _shards[workPeer+1].getPeer(workQuorum+offset*_peersPerShard/_intersections);
                tmpA->setCoPeer(tmpB);
                tmpB->setCoPeer(tmpA);
            }
        }
    }

}

void quickPBFT::printPeers(std::ostream& out) {
    for (auto e: _peers) {
        assert(e.second.getCoPeer() != nullptr);
        out << e.second.getID().first << "." << e.second.getID().second
            << " copeer " ;
        out << e.second.getCoPeer()->getID().first << "." << e.second.getCoPeer()->getID().second << std::endl;
    }
}

int quickShards::getConsensusCount() {
    int count = 0;
    for (auto e: _shards)
        if (e.second.validConsensus() && e.second.validIntersections())
            ++count;
    return count;
}

void quickShards::setRandomByzantineTrue(){

    if (getByzantineNumber() == _peersPerShard*_shards.size()/2){
        return;
    }
    if (_reserve > 0)
        _reserve--;
    else{
        auto tmp = _shards[rand()%_shards.size()].getPeer(rand()% _peersPerShard);
        while (tmp->isByzantine()){
            tmp = _shards[rand()%_shards.size()].getPeer(rand()% _peersPerShard);
        }
        tmp->setByzantineStatus(true);
    }
}
void quickShards::setRandomByzantineFalse() {
    // Get index of byzantine peers;
    std::vector<std::pair<int,int>> byzantineList;
    for(int i = 0; i < _shards.size() ; ++i){
        for(int j = 0; j < _shards[i].getPeerList().size(); ++j)
            if (_shards[i].getPeer(j)->isByzantine()){
                byzantineList.push_back(std::make_pair(i,j));
            }
    }
    // if no byzantine, add to reserve
    if(byzantineList.empty())
        ++_reserve;
    // Randomly choose peer index and make correct
    else{
        auto tmp = byzantineList[rand()%byzantineList.size()];
        _shards[tmp.first].getPeer(tmp.second)->setByzantineStatus(false);
    }
}

void quickShards::getSummary(std::ostream& out){
    out << "quorums=" << _shards.size() << std::endl;
    out << "peersPerQuorums=" << _peersPerShard << std::endl;
    out << "f= " << _shards.begin()->second.getF() << std::endl;
    out << "reserve= " << _reserve << std::endl;
    out << "valid peers= " << getValidPeers()<<std::endl;
    out << std::endl;
}

int quickShards::getValidPeers() {
    int count = 0;
    for(auto e: _shards){
        for(int peer = 0; peer < _peersPerShard; ++peer){
            if (!e.second.getPeer(peer)->isByzantine())
                ++count;
        }
    }
    return count/2;
}

std::deque<int> scheduleEvents(int numberOfEvents, int numberOfRounds){
    assert(numberOfEvents <= numberOfRounds);
    std::deque<int> schedule = std::deque<int>();
    std::uniform_int_distribution<int> randomDistribution(1,numberOfRounds);
    int seed = time(nullptr);
    std::default_random_engine randomGenerator (seed);

    for(int i = 0; i < numberOfEvents; i++) {
        int randomRound = 0;
        do {
            randomRound = randomDistribution(randomGenerator);
        } while (std::find(schedule.begin(), schedule.end(), randomRound) != schedule.end());
        schedule.push_back(randomRound);
    }

    std::sort(schedule.begin(), schedule.end());

    return schedule;
}

int quickShards::getByzantineNumber() {
    int count = 0;
    for(auto e: _shards){
        for(int peer = 0; peer < _peersPerShard; ++peer){
            if (e.second.getPeer(peer)->isByzantine())
                ++count;
        }
    }
    return count/2;
}

void quickShards::setAllCorrect() {
    for(auto e: _shards){
        for(int peer = 0 ; peer < _peersPerShard; ++peer)
            e.second.getPeer(peer)->setByzantineStatus(false);
    }
}