#ifndef LYRICS_H
#define LYRICS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

using namespace std;

#define ENDF char(127)
#define SOURCEPATH "/home/jevans06/Dropbox/School/comp15/hw6/includes/lyricsdb.txt"

typedef int SongId;

class Song{
 public:
  Song ();
  Song (const string&);
  Song (const Song &);

  string getArtist();
  string getTitle();
  string getLyrics();

 private:
  string data [3];
};

class SongDB {

 public:
  SongDB();
  SongDB(const SongDB &);

  bool initialize(int amount = 13232);

  class Iterator {
  public:
    Iterator();
    Iterator(int);
    Iterator(int,ifstream*);
    Iterator(const SongDB::Iterator & rhs);
    Iterator & operator++ ();
    Iterator & operator++ (int ct);
    Iterator & next();
    Iterator & operator= (const SongDB::Iterator & rhs);
    bool operator!= (const SongDB::Iterator &);
    bool operator== (const SongDB::Iterator &);
    Song operator* ();
    Song value();
    SongId songId();
  protected:
    int positionInFile;
    ifstream * source;
  };
  Iterator begin();
  Iterator end();
  Iterator find (SongId);

  Song getSongById(SongId);

 private:
  int startDir;
  int startFile;
  int endDir;
  ifstream source;
  int currentPosition;
  int limit;
  int currentArtist;

  Iterator endItr;
};

#endif
