#ifndef NTAGGER_TAGGER_DB_H
#define NTAGGER_TAGGER_DB_H

// execute given SQL query
int execQuery( string query, int (*callback)( void *, int, char **, char ** ), void *_1stArg );

// fills %existFiles with data from DB
int loadExistFiles();

// adds new file record into DB
int addFile( string file );

// adds tag to %file
int insertTag( string file, string tag );

// returns vector of files tagged as %tag from DB
vector<string> getFilesByOneTag( string tag );

// returns vector of tags that exist in DB
vector<string> getTagList();

// remove %tag from DB and from tags of every file record
int removeTag( string tag );

// renames tag
int renameTag( string oldTag, string newTag );

// remove file record from DB
int removeFile( string file );

#endif //NTAGGER_TAGGER_DB_H
