#ifndef DICT_H
#define DICT_H

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <time.h>
#include <string.h>
#include "lyrics.h"
#include "murmur.h"

#define USE_BLACKLIST 1
#define MAX_SONGS 20000000
#define MAX_SONG_POINTERS 1000      // Change this if you need more than 1000 results per keyword

using namespace std;

struct ToLower
{
    char operator() (char c) const
    {
        return std::tolower(c);
    }
};

struct MapValue
{
    MapValue(string k, vector<SongId> v)
    {
        key = k;
        songlist = v;
    }
    MapValue()
    {
        key = "";
        songlist = vector<SongId>();
    }
    string key;
    vector<SongId> songlist;
};

struct SongResult
{
    Song song;
    SongId id;
    string key;
    short keylength;
};

struct FileData
{
    unsigned int hash;
    string key;
    unsigned short nn;
    vector<SongId> songlist;
};

// Utility functions
bool compareSongs(SongResult a, SongResult b); // return a > b for sorting
string strip(string input);
string _(string);

class Dict
{
public:
    Dict();
    ~Dict();

    bool initialize();  // true = loaded from file, good to go.  false = file missing, map must be created
    list<SongResult> search(string key, unsigned int limit);
    void createDict();
    bool isBlacklisted(string word);

private:
    map <unsigned int, MapValue > theMap;
    SongDB songDB;
    SongDB::Iterator songDBiter;
    fstream blacklistfile;
    string dictfileName, blacklistfileName;
    bool dictReady;
    bool dictFileExists;
    vector<string> blacklist;

    unsigned int seed;

    bool loadDict();    // true = loaded and good to go.  false = file not found
    void loadBlacklist();


};

#endif // DICT_H
