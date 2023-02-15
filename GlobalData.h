#pragma once
#include <memory>
#include <filesystem>

class GlobalData
{
public:
    /**
     * @brief Creates if not already created and returns reference to GlobalData instance
     * @return reference to GlobalData instance
     */
    static GlobalData& getInstance();
    /**
     * @brief removes current instance
     */
    static void removeInstance();

    GlobalData(const GlobalData&) = delete;
    GlobalData& operator=(const GlobalData&) = delete;
    GlobalData& operator==(const GlobalData&) = delete;

    ~GlobalData();

    void setHotFolderPath(const std::string& dir);

    void setBackupFolderPath(const std::string& dir);

    const std::filesystem::path& getHotFolderPath() const;

    const std::filesystem::directory_entry& getHotFolderDir() const;

    const std::filesystem::path& getBackupFolderPath() const;

    const std::filesystem::directory_entry& getBackupFolderDir() const;

    void updatePaths();

    constexpr const std::filesystem::path& getBackupExtension()
    {
        return BackupExtension;
    }

    static constexpr const std::string& getDeletePrefix()
    {
        return DeletePrefix;
    }

    constexpr std::size_t getDeletePrefixSize()
    {
        return DeletePrefixSize;
    }
protected:

private:
    GlobalData();
    static std::unique_ptr<GlobalData> s_instance;

    std::filesystem::directory_entry m_hotFolderDir;
    std::filesystem::directory_entry m_backupFolderDir;

    static const std::filesystem::path BackupExtension;
    static const std::string DeletePrefix;
    static const std::size_t DeletePrefixSize;
};