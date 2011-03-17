/* John Sotherland (John.Sotherland@tufts.edu)
 * COMP 15 Spring 2010
 *
 * Homework 6 (Project 2): Song Search
 * Web Output Module - Implementation
 *
 * Web.cpp
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "Web.h"
using namespace std;

Web::~Web() {
     while (!songs.empty()) songs.pop();
     files.clear();
}

void Web::Clear_list() {
     while (!songs.empty()) songs.pop();
}

bool Web::Add_song (int song_id, string artist, string title, string lyrics) {
     bool success = true;
     WSong s;

     ostringstream buf;
     buf << "Web output error (song ID "
	 << song_id
	 << "): ";
     string prefix = buf.str();


     // Validate all parameters nonempty
     if (artist != "")
	  s.artist = artist;
     else {
	  success = false;
	  cout << prefix << "You must specify an artist." << endl;
     }
     if (title != "")
	  s.title = title;
     else {
	  success = false;
	  cout << prefix << "You must specify a title." << endl;
     }
     if (lyrics != "")
	  s.lyrics = lyrics;
     else {
	  success = false;
	  cout << prefix << "You must specify matching lyrics." << endl;
     }


     if (success)
	  songs.push(s);
     else
	  cout << "This song will not be output to the Web." << endl;


     return success;
}

void Web::Output(string comment, const char* filename) {
     // Build output string

     if (sizeof(filename)==0) {
	  cout << "You must specify a filename to output!" << endl;
	  return;
     }

     string output;

     if (comment != "")
	  output += "<h2>" + comment + "</h2>\n";

     if (songs.empty())
	  output = "<h3>No songs to display.</h3>";
     else {
	  unsigned int l = songs.size();

	  for (unsigned int i = 0; i < l; i++) {
	       WSong s = songs.front();
	       output = output
		    + "<h4>From the song \"" + s.title
		    + "\" by <em>" + s.artist
		    + "</em>:</h4>\n"

		    + "<p>\""
		    + s.lyrics
		    + "\"</p>\n";
	       songs.pop();
	       songs.push(s);
	  }
     }


     // Output to file. Append if previously used by this class instance.

     bool used = false;
     for (unsigned int i = 0; i < files.size(); i++)
	  if (files[i] == filename)
	       used = true;

     ofstream file;
     if (!used) {
	  files.push_back(filename);
	  file.open(filename, ios::out);
	  file << "<html>\n"
	       << "<head>\n"
	       << "<style type=\"text/css\"><!--\n"
	       << "    body {\n"
	       << "        font-face: Arial, Helvetica, sans-serif;\n"
	       << "    }\n"
	       << "    h1 {\n"
	       << "        \n"
	       << "    }\n"
	       << "    h2 {\n"
	       << "        margin: 40px 0 15px 0;\n"
	       << "    }\n"
	       << "    h3 {\n"
	       << "        \n"
	       << "    }\n"
	       << "    h4 {\n"
	       << "        margin: 10px 0 0 0;\n"
	       << "    }\n"
	       << "    p {\n"
	       << "        margin: 0;\n"
	       << "    }\n"
	       << "--></style>\n"
	       << "</head>\n"
	       << "<body>\n"
	       << "<h1>Search Results</h1>\n";

     }
     else file.open(filename, ios::app);

     file << output << endl;
     file.close();
}
