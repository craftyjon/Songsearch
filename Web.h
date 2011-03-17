/* John Sotherland (John.Sotherland@tufts.edu)
 * COMP 15 Spring 2010
 *
 * Homework 6 (Project 2): Song Search
 * Web Output Module - Interface
 *
 * Web.h
 */

#ifndef WEB_DEFINED
#define WEB_DEFINED

#include <queue>
#include <vector>
#include <string>
using namespace std;

struct WSong {
     int    id;
     string artist;
     string title;
     string lyrics;
};

class Web {
public:
     ~Web();

     bool Add_song (int song_id,
		    string artist, string title, string lyrics);
     /* Requires specification of artist, title, and some lyrics for a
      *	given song to output. Requests the song id for error message.
      * Boolean indicates success or failure. At this time, the only
      * failure condition is when an empty string "" is specified for
      * any string parameter.
     */

     void Clear_list ();
     /* Removes all songs added up to this point from class storage. This
      * allows you to reuse the same class instance for multiple
      * searches.
      *
      * An alternative is to instantiate a new class for each search.
      * Using Clear_list conserves memory between searches, however.
      */

     void Output (string comment, const char* filename);
     /* Outputs all songs in the list at this point to the HTML file
      * specified. Comment appears as a descriptor at the top of the section;
      * use this to specify to what the given search results pertain.
      *
      * N.B.: Automatically checks whether you have output to this file
      * before _with this class instance_.  I.e., if you want to output
      * the results of multiple searches to the same HTML file, reuse
      * the same class instance and specify the same filename.
      *
      * If you do not reuse the same class instance but specify the same file
      * name, Output will overwrite the file.  Accordingly, it is recommended
      * that you reuse the same class instance for all outputs.
      */

private:
     queue <WSong> songs;
     vector <string> files;
};

#endif
