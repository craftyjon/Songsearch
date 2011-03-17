#include "Dict.h"

Dict::Dict()
{
    // cout << "1" << endl;
    if (!songDB.initialize())
    {
        cout << "Error: Song Database not available." << endl;
    }
    //cout << "2" << endl;
    dictfileName = "dict.dat";

    blacklistfileName = "blacklist.txt";
    blacklistfile.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    dictReady = false;
    dictFileExists = false;
    // cout << "3" << endl;
}

Dict::~Dict()
{
    blacklistfile.close();

}


/* Dict::initialize: returns true if the map is ready to be searched, false otherwise.
 */
bool Dict::initialize()
{

    ifstream dictfile(dictfileName.c_str(), ifstream::in);

    if (dictfile.good())
    {
        loadDict();
    }
    else
    {
        cout << "Dictionary file not present.  Creating new file.  This may take some time..." << endl;
        createDict();
    }
    if (USE_BLACKLIST)
    {
        try
        {
            blacklistfile.open(blacklistfileName.c_str(), ifstream::in);
            // cout << "Loading Blacklist..." << endl;
            if (blacklistfile.good())
                loadBlacklist();

        }
        catch (ifstream::failure e)
        {
            cout << "Warning: Blacklist file not present!" << endl;
        }
    }
    return true;
}

void Dict::loadBlacklist()
{
    string buf;
    while (!blacklistfile.eof())
    {
        try
        {
            blacklistfile >> buf;
        }
        catch (ifstream::failure e)
        {
            break;
        }
        // cout << buf << endl;
        blacklist.push_back(buf);
    }
    cout << "Blacklist Loaded." << endl;
}

bool Dict::isBlacklisted(string word)
{
    for (unsigned int i=0; i<blacklist.size(); i++)
    {
        if (word.compare(blacklist[i])==0)
            return true;
    }
    return false;
}

/* Dict::createDict(): creates a new dictionary file using the SongDB.
*/
void Dict::createDict()
{
    ofstream dictfile;
    try
    {
        dictfile.open(dictfileName.c_str(), fstream::out | fstream::trunc);
    }
    catch (ifstream::failure e)
    {
        cout << "could not open dictionary file for output" << endl;
        return;
    }

    SongDB::Iterator dbi = songDB.begin();
    Song theSong;
    string theWord;
    stringstream theLyrics;
    int theID;
    unsigned int theHash;

    int collisions = 0, inserts=0;

    MapValue mv;
    string t;

    srand(time(NULL));
    seed = rand() % 40000000;
    bool flag = false;

    while (dbi != songDB.end())
    {
        theSong = dbi.value();
        theID = dbi.songId();
        theLyrics << "";
        theLyrics.clear();
        t = theSong.getLyrics();
        theLyrics.str(t);

        theWord = "";

        while (theLyrics >> theWord)
        {
            theWord = strip(theWord);

            if (theWord.length() < 2)
                continue;

            transform(theWord.begin(), theWord.end(), theWord.begin(), ToLower());

            if (USE_BLACKLIST && isBlacklisted(theWord))
                continue;

            theHash = MurmurHash2A(theWord.c_str(), theWord.length(), seed);

            if (theMap.find(theHash) == theMap.end())
            {
                mv = MapValue();
                //cout << " (new)";
                mv.songlist = vector<SongId>();
                mv.songlist.push_back(theID);
                mv.key = theWord;
                theMap.insert(pair<unsigned int, MapValue >(theHash,mv));
                inserts++;
            }
            else    // append
            {
                // check for collision
                mv = theMap[theHash];

                if (mv.key.compare(theWord)==0) // no collision, append
                {
                    // cout << "No collision, append" << endl;
                    // cout << "songlist size before = " << mv.songlist.size() << endl;
                    //  cout << "adding " << theID << " to the songID list for " << theWord << endl;

                    // check to see if the ID we're on is already on this list
                    flag = false;
                    for (unsigned int j=0; j<mv.songlist.size(); j++)
                    {
                        if (mv.songlist[j] == theID)
                            flag = true;
                    }

                    if (!flag)
                        mv.songlist.push_back(theID);

                    theMap[theHash] = mv;
                    inserts++;
                }
                else
                {
                    // collision, rehash with a different seed to try again
                    //cout << " ** Hash Collision! " << theHash << " current: " << mv.key << " mew: " << theWord << endl;
                    collisions++;

                    // linear forward search
                    do
                    {
                        mv = theMap[++theHash];
                    }
                    while ((mv.songlist.size() > 0) && (mv.key.compare(theWord)!=0));

                    //Now, we either have the first blank address (for a new collision) or the previously resolved collision address.
                    if (mv.songlist.size() > 0)
                    {
                        mv = MapValue();

                        mv.songlist = vector<SongId>();
                        mv.songlist.push_back(theID);
                        mv.key = theWord;

                        inserts++;
                        theMap[theHash] = mv;

                        //cout << "Previously resolved.  New address is " << theHash << endl;
                    }
                    else
                    {
                        // mv = theMap[theHash];
                        // cout << "Resolution: " << theHash << endl;
                        // cout << "Resolution: " << theHash << endl;
                        mv.key = theWord;
                        mv.songlist = vector<SongId>();
                        mv.songlist.push_back(theID);

                        theMap[theHash] = mv;
                        inserts++;
                    }
                    //goto restartRound;  // I know.
                }
            }

        }

        if (inserts > MAX_SONGS)
            break;
        //return;
        dbi = dbi.next();
    }
    cout << collisions << " collisions and " << inserts << " records ("  << (((1.0*collisions)/inserts)*100) << "%)" << endl;
    //Now, output the semi-ordered map to file
    // cout << "outputting file..." << endl;

    dictfile.put((char)0);

    dictfile.write((char *)&seed, sizeof(unsigned int));
    unsigned short tui = 0;
    char* tc;

//cout << "Seed = " << hex << seed << endl;
    dictfile.put('\n');
    //  dictfile << "Seed " << seed << endl;
    for (map<unsigned int, MapValue>::iterator iter=theMap.begin(); iter != theMap.end(); iter++)
    {

        mv = (*iter).second;
        // dictfile << (*iter).first << " " << mv.key << " " << mv.songlist.size() << " ";
        dictfile.write((char *)&(*iter).first, sizeof(unsigned int));
        //cout << hex << (*iter).first << " " << mv.key.length() << endl;

        tui = mv.key.length();
        dictfile.write((char *)&tui, sizeof(unsigned short));

        tc = new char[tui+1];
        strcpy(tc, mv.key.c_str());
        // cout << "tui = " << tui << endl;
        for (unsigned short j=0; j<=tui; j++)
        {
            //  cout << tc[j];
            dictfile.write(&tc[j], sizeof(char));
        }
        delete[] tc;

        // cout << mv.key << " " << mv.songlist.size() << endl;

        tui = mv.songlist.size();
        dictfile.write((char *)&tui, sizeof(unsigned short));

        for (unsigned short j=0; j<tui; j++)
        {
            dictfile.write((char*)&mv.songlist[j], sizeof(unsigned int));
        }
        //dictfile.put('\n');
    }

    dictfile.close();
}

bool Dict::loadDict()
{
    string tempString;
    stringstream ss;
    unsigned int i = 0;
    long tempHash = 0;
    unsigned short keylength = 0;
    string tempKey;
    unsigned short ns = 0;
    unsigned int tempSong = 0;
    vector<SongId> tempVector;
    MapValue mv, testmv;
    bool flag;

    ifstream dictfile;

    char buffer[128];

    FileData fd;

    dictfile.open(dictfileName.c_str(), ifstream::in | ifstream::binary);

    dictfile.seekg(0);

    strcpy(buffer, "");

    char tc;
    char* tcp;

    dictfile.get(tc);
    if (tc != 0)
    {
        cout << "Malformed dictionary file!" << endl;
        return false;
    }

    dictfile.read((char*)&seed, sizeof(unsigned int));
    // cout << "Seed: " << hex << seed << endl;

    dictfile.read((char*)&tc, sizeof(char));

    if (tc != '\n')
    {
        cout << "Malformed dictionary file!" << endl;
        return false;
    }

    flag = false;

    while (!flag)
    {
        // cout << i << endl;
        // return true;
        tempHash = 0;
        tempSong = 0;
        ns = 0;

        tempKey = "";
        strcpy(buffer, "");
        tempVector = vector<SongId>();
        i++;

        dictfile.read((char *)&tempHash, sizeof(unsigned int));

        // First, grab the size of the key
        dictfile.read((char *)&keylength, sizeof(unsigned short));

        //Next, the keyword
        tcp = new char[keylength+1];
        dictfile.read(tcp, keylength+1 * sizeof(char));
        tempKey = tcp;
        delete[] tcp;

        //Finally, the numsongs, and the songs
        dictfile.read((char *)&ns, sizeof(short));

        //cout << "keylength = " << keylength << " temphash = " << tempHash << " tempKey= " << tempKey << " ns = " << ns << endl;

        for (unsigned short j=0; j<ns; j++)
        {
            dictfile.read((char *)&tempSong, sizeof(unsigned int));
            tempVector.push_back(tempSong);
        }

        mv.key = tempKey;
        mv.songlist = tempVector;

        theMap[tempHash] = mv;

        dictfile.peek();
        if (dictfile.eof())
        {
            //cout << "eof" << endl;
            break;
        }
    }
    cout << "Dictionary Loaded." << endl;
    return true;
}

/* Dict::search: returns a vector of Song objects matching the given search terms (up to limit results) */
list<SongResult> Dict::search(string key, unsigned int limit)
{
    list<SongResult> retval;
    SongResult tsr;

    key = strip(key);

    if (USE_BLACKLIST && isBlacklisted(key))
    {
        cout << key << " is on the blacklist. Exiting." << endl;
        return retval;
    }

    unsigned int theHash = MurmurHash2A(key.c_str(), key.length(), seed);
    MapValue mv = theMap[theHash];



    //cout << key << " hashes to " << theHash << " and the key at that location is " << mv.key << endl;

    if (mv.key.length() > 0 && mv.key.compare(key)==0)  // match
    {
        //cout << "match found. " << mv.songlist.size() << " songs are attached." << endl;
        for (unsigned int i=0; i<mv.songlist.size(); i++)
        {
            if ((i+1) > limit)
                break;
            //cout << mv.songlist[i] << " ";
            tsr = SongResult();
            tsr.key = key;
            tsr.keylength = key.length();
            tsr.id = mv.songlist[i];
            tsr.song = songDB.getSongById(mv.songlist[i]);
            retval.push_back(tsr);
        }
        //cout << endl;
    }
    else if (mv.key.length() > 0)   // collision
    {
        // linear forward search
        do
        {
            mv = theMap[++theHash];
        }
        while ((mv.key.length() > 0) && (mv.key.compare(key)!=0));

        //Now, we ought to
        if (mv.key.length() > 0)
        {
            for (unsigned int i=0; i<mv.songlist.size(); i++)
            {
                if ((i+1) > limit)
                    break;
                //cout << mv.songlist[i] << " ";
                tsr = SongResult();
                tsr.key = key;
                tsr.keylength = key.length();
                tsr.song = songDB.getSongById(mv.songlist[i]);
                retval.push_back(tsr);
            }
        }
        else
        {
            cout << "Unresolved hash collision on input. Fatal error." << endl;
        }
    }
    else
    {
        cout << "not found." << endl;
        // no results.
    }

    return retval;
}


bool compareSongs(SongResult a, SongResult b)
{
    // We'd better hope that a.key == b.key
    int ac=0, bc=0;

    // Search the lyrics of each song, count the occurrances of key
    stringstream ss;
    string s;
    ss.str(a.song.getLyrics());

    while (ss >> s)
    {
        s = strip(s);
        reverse(s.begin(), s.end());
        if (s.compare(a.key)==0)
            ac++;
    }

    ss.clear();
    ss.str(b.song.getLyrics());

    while (ss >> s)
    {
        s = strip(s);
        reverse(s.begin(), s.end());
        if (s.compare(b.key)==0)
            bc++;
    }

    //  cout << _(a.song.getTitle()) << " has " << ac << " occurrances and " << _(b.song.getTitle()) << " has " << bc << endl;

    return (ac > bc);
}

string _(string n)
{

    replace(n.begin(), n.end(), '\n', '/');
    replace(n.begin(), n.end(), '\r', '/');
    remove(n.begin(), n.end(), '\t');

    replace(n.end()-1, n.end(), '/',' ');

    return n;
}


// Formats a string for keyword searching.  We are assuming that the stream operator can split at word boundaries appropriately (\n, space)
// Returns the string, in lowercase, with no punctuation.
string strip(string input)
{
    transform(input.begin(), input.end(), input.begin(), ToLower());

    //TODO: This doesn't properly remove punctuation from the last character of a string.

    string searchString( "abcdefghijklmnopqrstuvwxyz1234567890" );

    string::size_type pos = 0;
    while ( (pos = input.find_first_not_of(searchString, pos)) != string::npos )
    {
        input.replace( pos, 1, "" );
        pos++;
    }

    return input;
}
