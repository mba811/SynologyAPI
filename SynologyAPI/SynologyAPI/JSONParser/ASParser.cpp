//
//  Created on 26/09/14.
//  2014 palladion.it.
//

#include "ASParser.h"

namespace parser {
    
    using namespace std;
    
    ASParser::ASParser() : ASParser(string())
    {
        
    }
    
    ASParser::ASParser(string url) : parser::JSONParser(url)
    {
        
    }
    
    ASParser::~ASParser()
    {
        
    }
    
    //Private methods
    
    AudioInfo ASParser::ParseAudioInfo(json_t *node)
    {
        AudioInfo info;
        info.duration = IntInNodeForKey(node, "duration");
        info.bitrate = IntInNodeForKey(node, "bitrate");
        info.frequency = IntInNodeForKey(node, "frequency");
        info.channel = IntInNodeForKey(node, "channel");
        info.filesize = IntInNodeForKey(node, "filesize");
        return info;
    }
    
    Song ASParser::ParseSong(json_t *node)
    {
        json_t *tagNode = NodeForKey(node, "song_tag");
        
        if (tagNode == NULL) {
            tagNode = node;
        }
        
        Song song;
        song.id3Tag.albumNr = IntInNodeForKey(tagNode, "track");
        song.id3Tag.year = song.id3Tag.album.year = IntInNodeForKey(tagNode, "year");
        song.id3Tag.disc = IntInNodeForKey(tagNode, "disc");
        song.sId = StringInNodeForKey(node, "id");
        song.title = song.id3Tag.title = StringInNodeForKey(node, "title");
        song.id3Tag.artist = song.id3Tag.album.artist = StringInNodeForKey(tagNode, "artist");
        song.id3Tag.album.name = StringInNodeForKey(tagNode, "album");
        song.id3Tag.album.albumArtist = StringInNodeForKey(tagNode, "album_artist");
        song.id3Tag.genre = StringInNodeForKey(tagNode, "genre");
        song.aInfo = ParseAudioInfo(NodeForKey(node, "song_audio"));
        
        return song;
    }
    
    Album ASParser::ParseAlbum(json_t *node)
    {
        Album album;
        album.name = StringInNodeForKey(node, "name");
        album.artist = StringInNodeForKey(node, "artist");
        album.displayArtist = StringInNodeForKey(node, "display_artist");
        album.albumArtist = StringInNodeForKey(node, "album_artist");
        album.year = IntInNodeForKey(node, "year");
        
        return album;
    }
    
    //Protected methods
    
    //Public methods
    
    string ASParser::ParseInfo()
    {
        string info = string();
        info.append("Version: ");
        info.append("major - ");
        info.append(StringForKey("major"));
        info.append("; minor - ");
        info.append(StringForKey("minor"));
        info.append("\nBuild: ");
        info.append(StringForKey("build"));
        
        return info;
    }
    
    vector<Song> ASParser::GetSongs()//maybe build null check and merge with GetSongsFor();
    {
        vector<Song> songs = vector<Song>();
        json_t *songRoot = NodeForKey(rootNode, "songs");
        vector<json_t *> jSongs = NodesInArray(songRoot);
        
        for (auto &song : jSongs) {
            Song tmpSong = ParseSong(song);
            songs.push_back(tmpSong);
        }
        
        return songs;
    }
    
    vector<Song> ASParser::GetSongsFor(std::string album, std::string artist)
    {
        vector<Song> songs = vector<Song>();
        json_t *songRoot = NodeForKey(rootNode, "songs");
        vector<json_t *> jSongs = NodesInArray(songRoot);
        
        for (auto &song : jSongs) {
            Song tmpSong = ParseSong(song);
            
            if (tmpSong.id3Tag.artist.compare(artist) == 0 && tmpSong.id3Tag.album.name.compare(album) == 0) {
                songs.push_back(tmpSong);
            }
        }
        
        return songs;
    }
    
    vector<Album> ASParser::GetAlbumsFor(std::string artist)
    {
        vector<Album> albums = vector<Album>();
        json_t *albumRoot = NodeForKey(rootNode, "albums");
        vector<json_t *> jAlbums = NodesInArray(albumRoot);
        
        for (auto &album : jAlbums) {
            albums.push_back(ParseAlbum(album));
        }
        
        return albums;
    }
    
    Audio ASParser::GetSongFromData(std::string data, AudioType type)
    {
        Audio song;
        song.length = data.size();
        song.data = new char[song.length + 1]; //possible memory leak
        std::copy(data.begin(), data.end(), song.data);
        song.data[song.length] = '\0';
        
        return song;
    }
}