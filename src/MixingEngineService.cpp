#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1),auto_sync(false), bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    delete decks[0];
    delete decks[1];
    decks[0]=nullptr;
    decks[1]=nullptr;
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    if(!decks[0] && !decks[1]){
        active_deck=0;
        PointerWrapper<AudioTrack> clone_track=track.clone();
        AudioTrack* trckPtr = clone_track.release();
        if(trckPtr==nullptr){
            std::cout<<"[ERROR] Track:  " << track.get_title() << " failed to clone "<<std::endl;
            return -1;
        }
        decks[0]=trckPtr;
        return 0;
    }
    PointerWrapper<AudioTrack> clone_track=track.clone();
        AudioTrack* trckPtr = clone_track.get();
        if(trckPtr==nullptr){
            std::cout<<"[ERROR] Track:  " << track.get_title() << " failed to clone "<<std::endl;
            return -1;
        }
    size_t target_deck = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;
    if(decks[target_deck]){
        delete decks[target_deck];
        decks[target_deck]=nullptr;
    }
    trckPtr->load();
    trckPtr->analyze_beatgrid();
    if(auto_sync){
        int bpm_difference = std::abs(decks[active_deck]->get_bpm() - trckPtr->get_bpm());
        if(bpm_difference > bpm_tolerance){
            sync_bpm(clone_track);
        }
    }
    clone_track.release();
    decks[target_deck]=trckPtr;
    std::cout << "[Load Complete] " << track.get_title() << " is now loaded on deck " << target_deck << std::endl;
    std::cout << "[Unload] Unloading previous deck: " << active_deck << " " << decks[active_deck]->get_title() <<std::endl;
    delete decks[active_deck];
    decks[active_deck]=nullptr;
    active_deck=target_deck;
    std::cout << "[Active Deck] switch to deck " << target_deck << std::endl;
    return target_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if(!decks[active_deck] || !track.get()){
        return false;
    }
    int bpm_diff = std::abs(decks[active_deck]->get_bpm() - track->get_bpm());
    return bpm_diff <= bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck] && track.get()){
        int active_bpm = decks[active_deck]->get_bpm();
        int track_bpm = track->get_bpm();
        int average_bpm = (active_bpm + track_bpm) / 2;
        std::cout << "  [Sync BPM] Syncing BPM from " << track_bpm << " to " << average_bpm << std::endl; //<original => track> to <new => average>
        track->set_bpm(average_bpm); // Setter function added as per forum discussion guidelines
    }

}
