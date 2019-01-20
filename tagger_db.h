#ifndef NTAGGER_TAGGER_DB_H
#define NTAGGER_TAGGER_DB_H

int execQuery(string query);

int loadExistFiles();

int addFile(path file);

int insertTag(path file, string tag);

#endif //NTAGGER_TAGGER_DB_H
