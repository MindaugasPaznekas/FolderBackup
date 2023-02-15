#include <filesystem>
#include "LogUtility.h"
/**
 * @brief Helper for various file system operations
 */
class FSHelper
{
public: 
    FSHelper() = delete;
    ~FSHelper() = delete;
    /**
     * @brief checks if file exists
     * @param dir: reference dir entry to check
     * @return true if exists
     */
    static bool checkIfFileExists(const std::filesystem::directory_entry& dir);
    /**
     * @brief Check if directory exists
     * @param dir: reference to dir entry to check
     * @return true if exists
     */
    static bool checkIfFolderExists(const std::filesystem::directory_entry& dir);
    /**
     * @brief Checks if directory exists, tries to create otherwise
     * @param dir: reference to dir entry to check
     * @return true: directory exists, or was created successfully
     * @return false: entry does not exist and cretion failed
     */
    static bool checkIfFolderExistsOrCreate(const std::filesystem::directory_entry& dir);
    /**
     * @brief Bakcup a single file
     * Backs up a file if not already backed up or changed.
     * files with 'delete_' prefix are deleted.
     * @param fileTobackup: source file which needs backup
     */
    static void backupSingleFile(const std::filesystem::directory_entry& fileTobackup);
    /**
     * @brief With compilation flag set prints out some info to help with debugging
     * @param logLine: message to print
     */
    static void debugLog(std::string& logLine);
    /**
     * @brief Check prerequisites of environment
     * For example that hot folder exists and backup folder is possible to create
     * @return true: application is able to run in given environment
     * @return false: abort application due to environment issues
     */
    static bool initEnvironment();

private:
    /**
     * @brief Waits till folder is created as it might take some time for folder to be created and visible for the program.
     * Up to 10s of wait time.
     * @param dir: directory for which were are waiting to be created   
     * @return true: folder found
     * @return false: not found
     */
    static bool waitForDirectoryCreation(const std::filesystem::directory_entry& dir);
    static bool copyFile(const std::filesystem::path& source, const std::filesystem::path& destination, LogUtility::Action& logAction);
    static void setPermissions(const std::filesystem::directory_entry& dir);
    static void updateTimeFromSourceToDestination(const std::filesystem::path& source, const std::filesystem::path& destination);
    static void updateFileDate(const std::filesystem::path& pathToFile, const std::filesystem::file_time_type& time);
    static bool fileDoesNotExistOrNeedsUpdate(const std::filesystem::path& fileTobackup, 
        const std::filesystem::directory_entry& destination, LogUtility::Action& logAction);
    static std::filesystem::file_time_type getLastWriteTime(const std::filesystem::path& path);
    static bool doesHotFileNeedToBeDeleted(const std::filesystem::directory_entry& fileToBackup);
    static void removeFile(const std::filesystem::path& fileToRemove, bool logMessage = true);
    static void deleteBackupFile(std::string sourceFile);
    static void errorCodeHandler(std::string userText, std::error_code errorCode);
    static uintmax_t getFileSize(const std::filesystem::directory_entry& file);
    static uintmax_t getFileSize(const std::filesystem::path& file);
};