#ifndef TAGGER_TAGGER_FS_H
#define TAGGER_TAGGER_FS_H

// fills %newFiles and %missingFiles
void scanForNewFiles();

// tries to create folder with results
void makeResult( const vector<string> &files );

#endif //TAGGER_TAGGER_FS_H
