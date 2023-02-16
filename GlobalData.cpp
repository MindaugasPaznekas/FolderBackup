#include "GlobalData.h"

using namespace std;
namespace fs = filesystem;

unique_ptr<GlobalData> GlobalData::s_instance = nullptr;
const fs::path GlobalData::BackupExtension{".bak"};
const string GlobalData::DeletePrefix{"delete_"};
const size_t GlobalData::DeletePrefixSize{DeletePrefix.size()};

GlobalData::GlobalData(optional<const string> hotFolderPath,
    optional<const string> backupFolderPath) :
    m_hotFolderDir{},
    m_backupFolderDir{}
{
    if (hotFolderPath)
    {
        m_hotFolderDir.assign(fs::u8path(hotFolderPath.value()));
    }
    if (backupFolderPath)
    {
        m_backupFolderDir.assign(fs::u8path(backupFolderPath.value()));
    }
}

GlobalData::~GlobalData()
{

}

GlobalData& GlobalData::getInstance(optional<const string> hotFolderPath, optional<const string> backupFolderPath)
{
    if (!s_instance)
    {
        s_instance = unique_ptr<GlobalData>(new GlobalData(hotFolderPath, backupFolderPath));
    }
    return *s_instance.get();
}

void GlobalData::removeInstance()
{
    s_instance.reset();
}

const fs::path& GlobalData::getHotFolderPath() const
{
    return m_hotFolderDir.path();
}

const fs::directory_entry& GlobalData::getHotFolderDir() const
{
    return m_hotFolderDir;
}

const fs::path& GlobalData::getBackupFolderPath() const
{
    return m_backupFolderDir.path();
}

const fs::directory_entry& GlobalData::getBackupFolderDir() const
{
    return m_backupFolderDir;
}

void GlobalData::updatePaths()
{
    m_hotFolderDir.refresh();
    m_backupFolderDir.refresh();
}