#pragma once

#include <memory>
#include <filesystem>
#include <optional>

class GlobalData
{
public:
    /**
     * @brief Creates if not already created and returns reference to GlobalData instance
     * @note First call initializes it and should provide the required paths
     * @param hotFolderPath: path to directory which should be updated
     * @param backupFolderPath: backup directory
     * @return GlobalData& instance
     */
    static GlobalData& getInstance(std::optional<const std::string> hotFolderPath = std::nullopt, 
        std::optional<const std::string> backupFolderPath = std::nullopt);
    /**
     * @brief removes current instance
     */
    static void removeInstance();

    GlobalData(const GlobalData&) = delete;
    GlobalData& operator=(const GlobalData&) = delete;
    GlobalData& operator==(const GlobalData&) = delete;

    ~GlobalData();

    const std::filesystem::path& getHotFolderPath() const;

    const std::filesystem::directory_entry& getHotFolderDir() const;

    const std::filesystem::path& getBackupFolderPath() const;

    const std::filesystem::directory_entry& getBackupFolderDir() const;

    void updatePaths();

    constexpr const std::filesystem::path& getBackupExtension()
    {
        return BackupExtension;
    }

    constexpr const std::string& getDeletePrefix()
    {
        return DeletePrefix;
    }

    constexpr std::size_t getDeletePrefixSize()
    {
        return DeletePrefixSize;
    }
protected:
    GlobalData(std::optional<const std::string> hotFolderPath = std::nullopt,
        std::optional<const std::string> backupFolderPath = std::nullopt);
private:
    static std::unique_ptr<GlobalData> s_instance;

    std::filesystem::directory_entry m_hotFolderDir;
    std::filesystem::directory_entry m_backupFolderDir;

    static const std::filesystem::path BackupExtension;
    static const std::string DeletePrefix;
    static const std::size_t DeletePrefixSize;
};