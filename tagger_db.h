#ifndef NTAGGER_TAGGER_DB_H
#define NTAGGER_TAGGER_DB_H

int execQuery(string query, int (*callback)( void*, int, char**, char** ), void *_1stArg);

int loadExistFiles();

int addFile(path file);

int insertTag(path file, string tag);

vector<path> getFilesByOneTag(string tag);

vector<string> getTagList();

#endif //NTAGGER_TAGGER_DB_H
