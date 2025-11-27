#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}

DJLibraryService:: ~DJLibraryService(){
    for (AudioTrack* track : library) {
        delete track;
    }
}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for(const SessionConfig::TrackInfo& curr_track : library_tracks){
        if(curr_track.type == "MP3"){
            MP3Track* curr = new MP3Track(curr_track.title, curr_track.artists, curr_track.duration_seconds,
            curr_track.bpm, curr_track.extra_param1, curr_track.extra_param2);
            library.push_back(curr);
            std::cout << " MP3Track created:  " << curr->get_bitrate() << " kbps" << std::endl;
        }
        else if(curr_track.type == "WAV"){
            WAVTrack* curr = new WAVTrack(curr_track.title, curr_track.artists, curr_track.duration_seconds,
                 curr_track.bpm, curr_track.extra_param1, curr_track.extra_param2);
                library.push_back(curr);
                std::cout << " WAVTrack created: " << curr->get_sample_rate() << 
                "Hz/" << curr->get_bit_depth() << "bit" << std::endl;
        }

    }
    std::cout << "[INFO] Track library built:  " << library.size() << " tracks loaded" << std::endl;
}


/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);

}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << " [INFO] Loading playlist: " << playlist_name << std::endl;
    playlist.clean(playlist_name);
    int counter = 0; //Counting tracks added
    int size = library.size();
    for(int lib_index : track_indices){
        if(lib_index < 1 || lib_index > size){
            std::cout << "  [WARNING] Invalid track index: " << lib_index << std::endl;
            
        }
        else{
            PointerWrapper<AudioTrack> clone_track = library[lib_index-1]->clone();
            AudioTrack* clonePtr = clone_track.release();
            if(clonePtr==nullptr){
                std::cout<< "  [ERROR] Track: failed to clone " << std::endl;
            }
            else{
                clonePtr->load();
                clonePtr->analyze_beatgrid();
                playlist.add_track(clonePtr);
                counter++;
                std::cout << " Added '" << clonePtr->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
            }

        }
        std::cout << " [INFO] Playlist loaded: " << playlist_name << " (" << counter << " tracks)" << std::endl;
    }
    
    
    
                                                // Your implementation here
    // For now, add a placeholder to fix the linker error
    (void)playlist_name;  // Suppress unused parameter warning
    (void)track_indices;  // Suppress unused parameter warning
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> output_vec;
    std::vector<AudioTrack*> temp_list = playlist.getTracks();
    for(AudioTrack* trackPtr : temp_list){
        output_vec.push_back(trackPtr->get_title());
    }
    return output_vec;
}
