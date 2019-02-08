#include "common.h"
#include "tagger_db.h"
#include "tagger_fs.h"

#if defined(_WIN32) || defined(WIN32)
    #include <windows.h>
#endif

sqlite3 *db = nullptr;
const char *dbFile = "tags.db";

path currentDir;
const char *resultFolder = "_result";

set<path> existFiles;
set<path> newFiles;
set<path> removedFiles;

const char* CREATE_TABLES = "create table if not exists tags(id integer primary key, name text not null, unique(name));"
                            "create table if not exists files(id integer primary key, file text not null, unique(file));"
                            "create table if not exists rels(fid integer not null, tid integer not null, unique(fid, tid));";

int getChoice(const string &msg)
{
    cout << msg << endl;
    int c;
    auto getStr = []()
    {
        string in;
        cin >> in;
        return std::stoi(in);
    };
    while(true)
    {
        try
        {
            c = getStr();
        }
        catch (std::exception &e)
        {
            cout << "Couldn't interpret the input. Try again." << endl;
            continue;
        }
        break;
    }
    return c;
}

//removes spaces in start and end, and replaces multiple space with one
string clearTag( string dirty )
{
    string ret;
    bool ws = true;
    for ( auto &c : dirty )
    {
        if( std::isspace(c) )
        {
            if( ws )
                continue;
            else
            {
                ws = true;
                ret.push_back(c);
            }
        }
        else
        {
            ws = false;
            ret.push_back(c);
        }
    }
    if( std::isspace( ret.back() ) )
        ret.pop_back();
    return ret;
}

void tagNewFiles()
{
    cin.clear(); cin.ignore();
    //for ( const auto &file : newFiles )
    while ( !newFiles.empty() )
    {
        auto &file = *newFiles.begin();
        string in; set<string> tags;
        cout << "Set tags for file: " << file.filename() << " separated by commas. Enter a empty line to stop tagging." << endl;
        std::getline(cin, in);
        if ( in.empty() )
        {
            cout << "Stop." << endl;
            break;
        }
        std::stringstream ss(in);
        while ( ss.good() )
        {
            string tag;
            getline( ss, tag, ',' );
            tag = clearTag(tag);
            if ( !tag.empty() )
                tags.insert(tag);
        }
        if ( addFile(file.filename().string()) )
            break;
        for ( auto &tag: tags )
            insertTag(file.filename().string(), tag);
        existFiles.insert(file);
        newFiles.erase(file);
    }
}

void searchByTags()
{
    cout << "Type one tag or enter empty line to cancel." << endl;
    string in;
    cin.clear(); cin.ignore();
    std::getline(cin, in);
    if ( in.empty() )
        return;
    auto result = getFilesByOneTag(clearTag(in));
    makeResult( result );
    for ( auto &file : result )
        cout << file << endl;

}

inline void pressEnter()
{
    cout << "press Enter..." << endl;
    string trash;
    cin.clear(); cin.ignore();
    std::getline(cin, trash);
}

void showTagList()
{
    vector<string> tags = getTagList();
    if ( tags.size() <= 20 )
    {
        cout << "Tag list:" << endl;
        for ( int i = 0; i < tags.size(); i++ ) {
            cout << "\t" << i + 1 << ") " << tags[i] << endl;
        }
        pressEnter();
    }
    else
    {
        int curPage;
        int maxPage = static_cast<int>(tags.size() / 20 + 1);
        int c = 1;
        do
        {
            if ( c > 0 && c <= maxPage )
            {
                curPage = c;
                cout << "Tag list (page " << curPage << "/" << maxPage << "):" << endl;
                for ( int i = 20 * ( curPage - 1 ); i < tags.size() && i < 20 * curPage; i++ ) {
                    cout << "\t" << i + 1 << ") " << tags[i] << endl;
                }
            }
            else
                cout << "Wrong page number." << endl;
        } while ((c = getChoice("Actions:\n"
                                "\t0) Back.\n"
                                "\tN) Show page N.")));
    }
}

void manageTags()
{
    while ( int c = getChoice("Actions:\n"
                              "\t0) Back.\n"
                              "\t1) Tag list.\n"
                              "\t2) Remove tag.\n"
                              "\t3) Rename tag.\n"
                              "\t4) (NI)Merge tags.") )
    {
        switch(c)
        {
            case 1:
                showTagList();
                break;
            case 2: {
                string tagToRemove;
                cout << "Type tag to remove." << endl;
                cin >> tagToRemove; //TODO: add multiple tag removing
                tagToRemove = clearTag(tagToRemove); //TODO: implement cancelling
                removeTag( tagToRemove );
                pressEnter();
                break;
            }
            case 3: {
                string oldTag, newTag;

                cout << "Type tag to rename." << endl;//TODO: implement cancelling
                cin >> oldTag;
                oldTag = clearTag( oldTag );

                cout << "Type new tag." << endl;//TODO: implement cancelling
                cin >> newTag;
                newTag = clearTag(newTag);

                renameTag(oldTag, newTag);
                pressEnter();
                break;
            }
            default:
                cout << "Wrong number" << endl;
        }
    }
}

int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(WIN32)
    SetConsoleOutputCP(CP_UTF8); // Set console code page to UTF-8 so console known how to interpret string data
    setvbuf(stdout, nullptr, _IOFBF, 1000); // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
#endif
    currentDir = fs::current_path();//TODO: add try-catch or pass error_code?

    if ( argc > 1 ) //TODO: add proper arguments handling
    {
        if ( argc > 2 )
        {
            if ( string(argv[1]) ==  "-s" )
            {
                cout << "INFO: multi-tag searching doesn't not implemented yet" << endl;
                cout << "Searching: \"" << clearTag( argv[2] ) << "\", results in folder " << resultFolder << endl;
                makeResult( getFilesByOneTag( clearTag( argv[2] ) ) );
                return 0;
            }
        }
    }

    char *err = nullptr;
    if ( sqlite3_open(dbFile, &db) )
    {
        cout << "Couldn't open/create database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    if ( sqlite3_exec(db, CREATE_TABLES, nullptr, nullptr, &err) )
    {
        cout << "SQL Error: " << err << endl;
        sqlite3_free(err);
        return 1;
    }
    loadExistFiles();

    scanForNewFiles();
    while ( int c = getChoice("Actions:\n"
                              "\t0) Exit. Not tagged files won't be added to database\n"
                              "\t1) Tag new files\n"
                              "\t2) (PI)Manage tags.\n"
                              "\t3) (NI)Manage files.\n"
                              "\t4) Search.") )
    {
        switch(c)
        {
            case 1:
                tagNewFiles();
                break;
            case 2:
                manageTags();
                break;
            case 3:
                //manageFiles();
                break;
            case 4:
                searchByTags();
                break;
            default:
                cout << "Wrong number." <<endl;
        }
    }

    sqlite3_close(db);
    return 0;
}