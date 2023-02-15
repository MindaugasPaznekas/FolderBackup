#include "FSHelper.h"
#include <iostream>
#include "GlobalData.h"
#include <chrono>
#include <thread>
#include <string>
#include "LogUtility.h"

using namespace std;
namespace fs = std::filesystem;

//#define DEBUG 1; //uncomment for extended output

void FSHelper::debugLog(string& logLine)
{
#ifdef DEBUG
    cout<<logLine<<"\n";
#endif
}

bool FSHelper::initEnvironment()
{
    const auto& gd = GlobalData::getInstance();

    if (!checkIfFolderExists(gd.getHotFolderDir()))
    {
        cerr << "Hot folder does not exist, or is not a directory: " << gd.getHotFolderPath() << endl;
        return false;
    }

    if (gd.getHotFolderDir() == gd.getBackupFolderDir())
    {
        cerr << "Hot folder is the same as backup folder. Please specify different folders.\n";
        cerr << "Provided 'hot' dir: " << gd.getHotFolderPath() << " 'backup': " << gd.getBackupFolderPath() << "\n";
        return false;
    }

    if (!checkIfFolderExistsOrCreate(gd.getBackupFolderDir()))
    {
        cerr << "Unable to create 'backup'dir.\n";
        cerr << "Provided 'backup' dir: " << gd.getBackupFolderPath() << "\n";
        return false;
    }

    return true;
}

bool FSHelper::checkIfFileExists(const fs::directory_entry& dir)
{
    GlobalData::getInstance().updatePaths();
    return dir.exists() && dir.is_regular_file();
}

bool FSHelper::checkIfFolderExists(const fs::directory_entry& dir)
{
    GlobalData::getInstance().updatePaths();
    return dir.exists() && dir.is_directory();
}

bool FSHelper::waitForDirectoryCreation(const fs::directory_entry& dir)
{
    const unsigned int sleepTimeMs = 10;
    const unsigned int sleepLoopsMax = 1000;
    for(unsigned int i = 0; i < sleepLoopsMax; i++)
    {
        if (checkIfFolderExists(dir))
        {
            return true;
        }
        this_thread::sleep_for(chrono::milliseconds(sleepTimeMs));
    }

    return checkIfFolderExists(dir);
}

bool FSHelper::checkIfFolderExistsOrCreate(const fs::directory_entry& dir)
{
    if (checkIfFolderExists(dir))
    {
        return true;
    }

    error_code ec;

    fs::create_directories(dir.path(), ec);

    if (ec) 
    {
        errorCodeHandler("Unable to create 'backup' dir" + dir.path().string(), ec);
        return false;
    }

    if (waitForDirectoryCreation(dir))
    {
        LogUtility::addMessageToLog(dir, LogUtility::Action::Created);
        setPermissions(dir);
    }

    return checkIfFolderExists(dir);
}

void FSHelper::setPermissions(const fs::directory_entry& dir)
{
    error_code ec;

    fs::permissions(dir, fs::perms::all, fs::perm_options::replace, ec);

    errorCodeHandler(dir.path().string() + " permission were not applied'", ec);
}

bool FSHelper::doesHotFileNeedToBeDeleted(const fs::directory_entry& fileToBackup)
{
    if (!fileToBackup.is_regular_file())
    {
        string log = fileToBackup.path().string() + " is not a file skipping";
        debugLog(log);
        return false;
    }

    if (!fileToBackup.path().has_filename())
    {
        return false;
    }

    if (!fileToBackup.path().has_stem())
    {
        return false;
    }

    const auto fileStem = fileToBackup.path().stem().string();

    auto& gd = GlobalData::getInstance();
    const auto deletePrefixSize = gd.getDeletePrefixSize();

    if (fileStem.size() < deletePrefixSize)
    {
        return false;
    }

    const auto& deletePrefix = gd.getDeletePrefix();

    auto cmp = deletePrefix.compare(0, deletePrefixSize, fileStem, 0, deletePrefixSize);

    return cmp == 0;
}

void FSHelper::deleteBackupFile(string sourceFilename)
{
    auto& gd = GlobalData::getInstance();
    gd.updatePaths();


    if (sourceFilename.size() <= gd.getDeletePrefixSize())
    {
        string log = sourceFilename + " source file name too short could not find backup";
        debugLog(log);
        return;
    }

    sourceFilename.erase(sourceFilename.begin(), sourceFilename.begin() + gd.getDeletePrefixSize());

    auto backUpToDelete = gd.getBackupFolderPath();
    backUpToDelete /= sourceFilename;
    backUpToDelete += gd.getBackupExtension();

    removeFile(backUpToDelete);
}

void FSHelper::backupSingleFile(const fs::directory_entry& fileToBackup)
{
    if (doesHotFileNeedToBeDeleted(fileToBackup))
    {
        removeFile(fileToBackup);
        deleteBackupFile(fileToBackup.path().filename().string());

        return;
    }

    auto& gd = GlobalData::getInstance();

    if (fileToBackup.is_regular_file())
    {
        gd.updatePaths();
        auto destination = gd.getBackupFolderPath();
        destination /= fileToBackup.path().filename();
        destination += gd.getBackupExtension();

        fs::directory_entry dest_dir{destination};
        LogUtility::Action logAction = LogUtility::Action::Backup;
        if (!fileDoesNotExistOrNeedsUpdate(fileToBackup, dest_dir, logAction))
        {
            return;
        }

        copyFile(fileToBackup, destination, logAction);

        updateTimeFromSourceToDestination(fileToBackup, destination);
    }
    else
    {
        string log = fileToBackup.path().string() + " is not a file skipping";
        debugLog(log);
    }
}

bool FSHelper::fileDoesNotExistOrNeedsUpdate(const fs::path& fileToBackup, const fs::directory_entry& destination,
    LogUtility::Action& logAction)
{
    if (!destination.exists())
    {
        return true;
    }

    //copy_options::update_existing does not work with MSYS on windows
    //@see https://github.com/msys2/MSYS2-packages/issues/1937#issuecomment-1002694786
    //So we will check modification date and size then delete file manually
    if (getLastWriteTime(fileToBackup) != getLastWriteTime(destination.path()))
    {
        removeFile(destination, false);
        logAction = LogUtility::Action::Update;
        return true;
    }
    else if (getFileSize(fileToBackup) != getFileSize(destination))
    {
        logAction = LogUtility::Action::Update;
        removeFile(destination, false);
        return true;
    }

    return false;
}

uintmax_t FSHelper::getFileSize(const fs::directory_entry& file)
{
    error_code ec;
    auto size = file.file_size(ec);

    errorCodeHandler(file.path().string() + " failed to read it's size.", ec);

    return size;
}

uintmax_t FSHelper::getFileSize(const fs::path& file)
{
    fs::directory_entry fileDir{file};

    return getFileSize(fileDir);
}

void FSHelper::removeFile(const fs::path& fileToRemove, bool logMessage)
{
    const fs::directory_entry entry{fileToRemove};
    if (!entry.exists())
    {
        string log = fileToRemove.string() + " - does not exist, will not be removed";
        debugLog(log);

        return;
    }

    error_code ec;

    fs::remove(fileToRemove, ec);

    errorCodeHandler("Failed to delete file: " + fileToRemove.string(), ec);

    if (!ec)
    {
        if (logMessage)
        {
            LogUtility::addMessageToLog(fileToRemove, LogUtility::Action::Delete);
        }

        string log = fileToRemove.string() + " - was deleted";
        debugLog(log);
    }
}


void FSHelper::updateTimeFromSourceToDestination(const fs::path& source, const fs::path& destination)
{
    const auto originalTime = getLastWriteTime(source);

    updateFileDate(destination, originalTime);
}

fs::file_time_type FSHelper::getLastWriteTime(const fs::path& path)
{
    error_code ec;

    auto lastWriteTime = fs::last_write_time(path, ec);

    errorCodeHandler("Failed to read last modification time for: " + path.string(), ec);

    return lastWriteTime;
}

void FSHelper::updateFileDate(const fs::path& pathToFile, const fs::file_time_type& time)
{
    error_code ec;

    fs::last_write_time(pathToFile, time, ec);

    errorCodeHandler(pathToFile.string() + " updating last write time failed'", ec);
}


bool FSHelper::copyFile(const filesystem::path& source, 
    const filesystem::path& destination, LogUtility::Action& logAction)
{
    error_code errorCode;
    fs::copy_options copyOption = fs::copy_options::update_existing;

    fs::copy_file(source, destination, copyOption, errorCode);

    if (errorCode) 
    {
        if (errorCode == errc::file_exists)
        {
            string log = destination.string() + " skipped because it already exists";
            debugLog(log);
        }
        else
        {
            errorCodeHandler(source.string() + " was not copied to 'backup''", errorCode);
            return false;
        }
    }

    LogUtility::addMessageToLog(source, destination, logAction);

    string log = destination.string() + " was copied";
    debugLog(log);

    return true;
}

void FSHelper::errorCodeHandler(std::string userText, std::error_code errorCode)
{
    if (errorCode)
    {
        cerr<<userText<<" - Due to Error: "<< errorCode.message()<<"\n";
    }
}