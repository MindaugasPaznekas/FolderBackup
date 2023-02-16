#include <filesystem>
#include "LogUtility.h"
/**
 * @brief Helper for various file system operations
 */
class FSHelper
{
public: 
    FSHelper(LogUtility::LogWriter& logWriter);
    ~FSHelper();
    /**
     * @brief checks if file exists
     * @param dir: reference dir entry to check
     * @return true if exists
     */
    bool checkIfFileExists(const std::filesystem::directory_entry& dir) const;
    /**
     * @brief Check if directory exists
     * @param dir: reference to dir entry to check
     * @return true if exists
     */
    bool checkIfFolderExists(const std::filesystem::directory_entry& dir) const;
    /**
     * @brief Checks if directory exists, tries to create otherwise
     * @param dir: reference to dir entry to check
     * @return true: directory exists, or was created successfully
     * @return false: entry does not exist and cretion failed
     */
    bool checkIfFolderExistsOrCreate(const std::filesystem::directory_entry& dir) const;
    /**
     * @brief Bakcup a single file
     * Backs up a file if not already backed up or changed.
     * files with 'delete_' prefix are deleted.
     * @param fileTobackup: source file which needs backup
     */
    void backupSingleFile(const std::filesystem::directory_entry& fileTobackup) const;
    /**
     * @brief With compilation flag set prints out some info to help with debugging
     * @param logLine: message to print
     */
    void debugLog(std::string& logLine) const;
    /**
     * @brief Check prerequisites of environment
     * For example that hot folder exists and backup folder is possible to create
     * @return true: application is able to run in given environment
     * @return false: abort application due to environment issues
     */
    bool initEnvironment() const;
private:
    /**
     * @brief Waits till folder is created as it might take some time for folder to be created and visible for the program.
     * Up to 10s of wait time.
     * @param dir: directory for which were are waiting to be created   
     * @return true: folder found
     * @return false: not found
     */
    bool waitForDirectoryCreation(const std::filesystem::directory_entry& dir) const;
    bool copyFile(const std::filesystem::path& source, const std::filesystem::path& destination, LogUtility::Action& logAction) const;
    void setPermissions(const std::filesystem::directory_entry& dir) const;
    void updateTimeFromSourceToDestination(const std::filesystem::path& source, const std::filesystem::path& destination) const;
    void updateFileDate(const std::filesystem::path& pathToFile, const std::filesystem::file_time_type& time) const;
    bool fileDoesNotExistOrNeedsUpdate(const std::filesystem::path& fileTobackup, 
        const std::filesystem::directory_entry& destination, LogUtility::Action& logAction) const;
    std::filesystem::file_time_type getLastWriteTime(const std::filesystem::path& path) const;
    bool doesHotFileNeedToBeDeleted(const std::filesystem::directory_entry& fileToBackup) const;
    void removeFile(const std::filesystem::path& fileToRemove, bool logMessage = true) const;
    void deleteBackupFile(std::string sourceFile) const;
    void errorCodeHandler(std::string userText, std::error_code errorCode) const;
    uintmax_t getFileSize(const std::filesystem::directory_entry& file) const;
    uintmax_t getFileSize(const std::filesystem::path& file) const;
    LogUtility::LogWriter& m_logWriter;
};