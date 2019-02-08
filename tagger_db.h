#ifndef NTAGGER_TAGGER_DB_H
#define NTAGGER_TAGGER_DB_H

int execQuery(string query, int (*callback)( void*, int, char**, char** ), void *_1stArg);

int loadExistFiles();

int addFile(string file);

int insertTag(string file, string tag);

vector<string> getFilesByOneTag(string tag);

vector<string> getTagList();

int removeTag(string tag);

int renameTag(string oldTag, string newTag);

#endif //NTAGGER_TAGGER_DB_H
