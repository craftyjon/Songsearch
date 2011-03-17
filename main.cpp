#include <iostream>
#include <math.h>
#include "Dict.h"
#include "Web.h"

using namespace std;

list<SongResult> songSort(list<SongResult> input);

int main(int argc, char* argv[])
{
    Dict *theDict = new Dict();
    Web *theWeb = new Web();
    list<SongResult> results;
    stringstream l;
    string t, lyrics;
    int li = 0;

    cout << "Loading..." << endl;

    theDict->initialize();
    // Input parsing is not strict.  Use with caution for now.

    if (argc < 2)
    {
        cout << "No input specified, exiting." << endl;
        return 0;
    }

    if (argc==2)
    {
        cout << "No limit specified on search string \"" << argv[1] << "\".  Defaulting to 10." << endl;
        results = theDict->search(argv[1], 10);
    }

    if (argc==3)
    {
        results = theDict->search(argv[1], atoi(argv[2]));
    }

    results = songSort(results);

    list<SongResult>::iterator iter = results.begin();

    for (unsigned int i=0; iter != results.end(); iter++)
    {
        i++;

        l.str("");
        l.clear();
        lyrics = (*iter).song.getLyrics();
        l.str(lyrics);

        li = 0;

        while (l >> t)
        {
            li += t.length()+1;
            t = strip(t);

            if (t.compare((*iter).key)==0)
                break;
        }

        if (li < 40)
        {
            //cout << "l1" << endl;
            lyrics = lyrics.substr(0,77)+"...";
        }
        else if (lyrics.length() - li < 40)
        {
            //cout << "l2" << endl;
            lyrics = "..."+lyrics.substr(lyrics.length()-77,77);
        }
        else
        {
            lyrics = "..."+lyrics.substr(li-t.length()-(37-(t.length()/2)), 74)+"...";
        }

        theWeb->Add_song((*iter).id, _((*iter).song.getArtist()), _((*iter).song.getTitle()), _(lyrics));

    }

    l.str("");
    l.clear();
    l << "Got " << results.size() <<" results.";

    theWeb->Output(l.str().c_str(), "results.htm");

    delete theDict;
    delete theWeb;

    return 0;
}

/* SongSort: returns a sorted list according to the order of this function.
 *           To Modify the sorting method, modify the compareSongs routine in Dict.cpp
 */
list<SongResult> songSort(list<SongResult> input)
{
    input.sort(compareSongs);
    return input;
}
