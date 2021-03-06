#include "common.h"
#include "tagger_fs.h"

void scanForNewFiles()
{
	std::set<string> allFiles;
	for ( const auto &entry : fs::directory_iterator( currentDir ) )
	{
		if ( entry.path().filename() != dbFile && entry.is_regular_file() )
			allFiles.insert( entry.path().filename().string() );
	}

	for ( const auto &path : allFiles )
		if ( existFiles.find( path ) == existFiles.end() )
			newFiles.insert( path );

	if ( newFiles.empty() )
		cout << "No new files found. ";
	else
		cout << "New files: " << endl;
	for ( const auto &path : newFiles )
		cout << "\t" << path << endl;

	for ( const auto &path : existFiles )
		if ( allFiles.find( path ) == allFiles.end() )
			missingFiles.insert( path );

	if ( !missingFiles.empty() )
		cout << "Missing files: " << endl;
	for ( const auto &path : missingFiles )
		cout << "\t" << path << endl;
}

void makeResult( const vector<string> &files )
{
	fs::remove_all( currentDir / resultFolder );
	fs::create_directory( currentDir / resultFolder );
	for ( const auto &file : files )
		try
		{
			fs::create_symlink( currentDir / file, currentDir / resultFolder / file );
		}
		catch ( fs::filesystem_error &e )
		{
			try
			{
				fs::copy( currentDir / file, currentDir / resultFolder / file );
			}
			catch ( fs::filesystem_error &e )
			{
				cout << "Couldn't create either symlinks or copy the result files." << endl;
				break;
			}
		}
}