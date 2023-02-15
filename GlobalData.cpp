#include "GlobalData.h"

using namespace std;
namespace fs = filesystem;

unique_ptr<GlobalData> GlobalData::s_instance = nullptr;
const filesystem::path GlobalData::BackupExtension{".bak"};
const string GlobalData::DeletePrefix{"delete_"};
const size_t GlobalData::DeletePrefixSize{DeletePrefix.size()};

GlobalData::GlobalData() :
    m_hotFolderDir{},
    m_backupFolderDir{}
{
}

GlobalData::~GlobalData()
{

}

GlobalData& GlobalData::getInstance()
{
    if (!s_instance)
    {
        s_instance = unique_ptr<GlobalData>(new GlobalData());
    }
    return *s_instance.get();
}

void GlobalData::removeInstance()
{
    s_instance.reset();
}

void GlobalData::setHotFolderPath(const string& Dir)
{
    m_hotFolderDir.assign(fs::u8path(Dir));
}

void GlobalData::setBackupFolderPath(const string& Dir)
{
    m_backupFolderDir.assign(fs::u8path(Dir));
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