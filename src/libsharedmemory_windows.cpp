#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <system_error>

#include "libsharedmemory.hpp"

namespace lsm
{
namespace lsm_windows_detail
{

    bool AssignPermissionsToFilesystemPath(const std::string path, int permissionsBitMask)
    {
        char buffer[MAX_FS_PATH] = { 0 };
        PSID usersSid = nullptr;

        if (path.empty() || path.size() >= MAX_FS_PATH)
            return false;

        if (!(permissionsBitMask & (UTIL_PERM_READ | UTIL_PERM_WRITE | UTIL_PERM_EXECUTE)))
        {
            return false;
        }

        strcpy_s(buffer, sizeof(buffer), path.c_str());

        if (!ConvertStringSidToSidA("S-1-5-32-545", &usersSid)) //built-in Users
            return false;

        EXPLICIT_ACCESSA accessPermissions = {};

        accessPermissions.grfAccessPermissions = 0;

        if (permissionsBitMask & UTIL_PERM_READ)
        {
            accessPermissions.grfAccessPermissions |= FILE_GENERIC_READ;
        }

        if (permissionsBitMask & UTIL_PERM_WRITE)
        {
            accessPermissions.grfAccessPermissions |= FILE_GENERIC_WRITE;
            accessPermissions.grfAccessPermissions |= DELETE;
        }

        if (permissionsBitMask & UTIL_PERM_EXECUTE)
        {
            accessPermissions.grfAccessPermissions |= FILE_GENERIC_EXECUTE;
        }

        accessPermissions.grfAccessMode = GRANT_ACCESS;
        accessPermissions.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
        accessPermissions.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        accessPermissions.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
        accessPermissions.Trustee.ptstrName = (LPCH)usersSid;

        // Retrieve existing DACL to merge with new ACE
        PACL oldDacl = nullptr;
        PSECURITY_DESCRIPTOR pSD = nullptr;
        DWORD result = GetNamedSecurityInfoA(
            buffer,
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            nullptr,
            nullptr,
            &oldDacl,
            nullptr,
            &pSD
        );

        if (result != ERROR_SUCCESS)
        {
            if (usersSid) LocalFree(usersSid);
            return false;
        }

        // Merge new ACE with existing DACL
        PACL newAcl = nullptr;
        result = SetEntriesInAclA(1, &accessPermissions, oldDacl, &newAcl);

        if (pSD)
            LocalFree(pSD);

        if (result != ERROR_SUCCESS)
        {
            if (usersSid) LocalFree(usersSid);
            return false;
        }

        result = SetNamedSecurityInfoA(buffer, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, newAcl, NULL);

        if (usersSid) LocalFree(usersSid);
        if (newAcl) LocalFree(newAcl);

        return result == ERROR_SUCCESS;
    }

    std::string GetSystemStorageDirectory()
    {
        char* programData = nullptr;
        size_t len = 0;
        if (_dupenv_s(&programData, &len, "PROGRAMDATA") == 0 && programData != nullptr)
        {
            std::filesystem::path storagePath = std::filesystem::path(programData) / "shared_memory";
            free(programData);

            std::error_code ec;
            std::filesystem::create_directories(storagePath, ec);
            if (ec)
            {
                return {};
            }

            AssignPermissionsToFilesystemPath(storagePath.string(), UTIL_PERM_READ | UTIL_PERM_WRITE);

            return storagePath.string();
        }
        return {};
    }

    std::string sanitize_name(const std::string& name)
    {
        std::string sanitized = name;
        const std::string invalid = "\\/:*?\"<>|";
        for (size_t idx = 0; idx < sanitized.size(); ++idx)
        {
            const char ch = sanitized[idx];
            if (ch < 32 || invalid.find(ch) != std::string::npos)
            {
                sanitized[idx] = '_';
            }
        }
        return sanitized;
    }

    std::string persistence_file_path(const std::string& name)
    {
        std::string basePath = GetSystemStorageDirectory();
        if (!basePath.empty())
        {
            if (const char last = basePath[basePath.size() - 1];
                last != '\\' && last != '/')
            {
                basePath.push_back('\\');
            }
        }
        basePath += "lsm_";
        basePath += sanitize_name(name);
        basePath += ".shm";
        return basePath;
    }

}; // namespace lsm_windows_detail
}; // namespace lsm
#endif
