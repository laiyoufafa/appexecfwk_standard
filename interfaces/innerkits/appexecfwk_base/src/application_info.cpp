/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "application_info.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "json_util.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string APPLICATION_NAME = "name";
const std::string APPLICATION_BUNDLE_NAME = "bundleName";
const std::string APPLICATION_DESCRIPTION = "description";
const std::string APPLICATION_ICON_PATH = "iconPath";
const std::string APPLICATION_LABEL = "label";
const std::string APPLICATION_LABEL_ID = "labelId";
const std::string APPLICATION_DESCRIPTION_ID = "descriptionId";
const std::string APPLICATION_ICON_ID = "iconId";
const std::string APPLICATION_DEVICE_ID = "deviceId";
const std::string APPLICATION_SIGNATURE_KEY = "signatureKey";
const std::string APPLICATION_IS_SYSTEM_APP = "isSystemApp";
const std::string APPLICATION_IS_LAUNCHER_APP = "isLauncherApp";
const std::string APPLICATION_ENABLED = "enabled";
const std::string APPLICATION_DEBUG = "debug";
const std::string APPLICATION_REMOVABLE = "removable";
const std::string APPLICATION_SUPPORTED_MODES = "supportedModes";
const std::string APPLICATION_PROCESS = "process";
const std::string APPLICATION_PERMISSIONS = "permissions";
const std::string APPLICATION_MODULE_SOURCE_DIRS = "moduleSourceDirs";
const std::string APPLICATION_MODULE_INFOS = "moduleInfos";
const std::string APPLICATION_ENTRY_DIR = "entryDir";
const std::string APPLICATION_CODE_PATH = "codePath";
const std::string APPLICATION_DATA_DIR = "dataDir";
const std::string APPLICATION_DATA_BASE_DIR = "dataBaseDir";
const std::string APPLICATION_CACHE_DIR = "cacheDir";
const std::string APPLICATION_FLAGS = "flags";
}

CustomizeData::CustomizeData(std::string paramName, std::string paramValue, std::string paramExtra)
    :name(paramName), value(paramValue), extra(paramExtra)
{
}

bool CustomizeData::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    value = Str16ToStr8(parcel.ReadString16());
    extra = Str16ToStr8(parcel.ReadString16());
    return true;
}

CustomizeData *CustomizeData::Unmarshalling(Parcel &parcel)
{
    CustomizeData *customizeData = new (std::nothrow) CustomizeData;
    if (customizeData && !customizeData->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete customizeData;
        customizeData = nullptr;
    }
    return customizeData;
}

bool CustomizeData::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(value));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(extra));
    return true;
}

bool ApplicationInfo::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    iconPath = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    deviceId = Str16ToStr8(parcel.ReadString16());
    signatureKey = Str16ToStr8(parcel.ReadString16());
    process = Str16ToStr8(parcel.ReadString16());
    entryDir = Str16ToStr8(parcel.ReadString16());
    codePath = Str16ToStr8(parcel.ReadString16());
    dataDir = Str16ToStr8(parcel.ReadString16());
    dataBaseDir = Str16ToStr8(parcel.ReadString16());
    cacheDir = Str16ToStr8(parcel.ReadString16());
    isSystemApp = parcel.ReadBool();
    isCloned = parcel.ReadBool();
    isLauncherApp = parcel.ReadBool();
    enabled = parcel.ReadBool();
    debug = parcel.ReadBool();
    removable = parcel.ReadBool();
    supportedModes = parcel.ReadInt32();
    labelId = parcel.ReadInt32();
    iconId = parcel.ReadInt32();
    descriptionId = parcel.ReadInt32();
    flags = parcel.ReadInt32();
    uid = parcel.ReadInt32();

    int32_t permissionsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissionsSize);
    for (int32_t i = 0; i < permissionsSize; i++) {
        permissions.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t moduleSourceDirsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleSourceDirsSize);
    for (int32_t i = 0; i < moduleSourceDirsSize; i++) {
        moduleSourceDirs.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t moduleInfosSize = parcel.ReadInt32();
    for (int32_t i = 0; i < moduleInfosSize; i++) {
        std::unique_ptr<ModuleInfo> moduleInfo(parcel.ReadParcelable<ModuleInfo>());
        if (!moduleInfo) {
            APP_LOGE("ReadParcelable<ModuleInfo> failed");
            return false;
        }
        moduleInfos.emplace_back(*moduleInfo);
    }
    if (!ReadMetaDataFromParcel(parcel)) {
        return false;
    }
    return true;
}

bool ApplicationInfo::ReadMetaDataFromParcel(Parcel &parcel)
{
    int32_t metaDataSize = parcel.ReadInt32();
    for (int32_t i = 0; i < metaDataSize; i++) {
        std::string mouduleName = Str16ToStr8(parcel.ReadString16());
        int32_t customizeDataSize = parcel.ReadInt32();
        std::vector<CustomizeData> customizeDatas;
        metaData[mouduleName] = customizeDatas;
        for (int j = 0; j < customizeDataSize; j++) {
            std::unique_ptr<CustomizeData> customizeData(parcel.ReadParcelable<CustomizeData>());
            if (!customizeData) {
                APP_LOGE("ReadParcelable<CustomizeData> failed");
                return false;
            }
            metaData[mouduleName].emplace_back(*customizeData);
        }
    }
    return true;
}

ApplicationInfo *ApplicationInfo::Unmarshalling(Parcel &parcel)
{
    ApplicationInfo *info = new (std::nothrow) ApplicationInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool ApplicationInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(iconPath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deviceId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(signatureKey));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(process));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(entryDir));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(codePath));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(dataDir));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(dataBaseDir));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(cacheDir));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isSystemApp);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isCloned);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isLauncherApp);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enabled);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, debug);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, removable);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, supportedModes);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, iconId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, flags);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uid);

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, permissions.size());
    for (auto &permission : permissions) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permission));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleSourceDirs.size());
    for (auto &moduleSourceDir : moduleSourceDirs) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleSourceDir));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, moduleInfos.size());
    for (auto &moduleInfo : moduleInfos) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &moduleInfo);
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, metaData.size());
    for (auto &item : metaData) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(item.first));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, item.second.size());
        for (auto &customizeData : item.second) {
            WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &customizeData);
        }
    }
    return true;
}

void ApplicationInfo::Dump(std::string prefix, int fd)
{
    APP_LOGI("called dump ApplicationInfo");
    if (fd < 0) {
        APP_LOGE("dump ApplicationInfo fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("dump ApplicationInfo fcntl error %{public}s", strerror(errno));
        return;
    }
    uint uflags = static_cast<uint>(flags);
    uflags &= O_ACCMODE;
    if ((uflags == O_WRONLY) || (uflags == O_RDWR)) {
        nlohmann::json jsonObject = *this;
        std::string result;
        result.append(prefix);
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        int ret = TEMP_FAILURE_RETRY(write(fd, result.c_str(), result.size()));
        if (ret < 0) {
            APP_LOGE("dump ApplicationInfo write error %{public}s", strerror(errno));
        }
    }
    return;
}

void to_json(nlohmann::json &jsonObject, const ApplicationInfo &applicationInfo)
{
    jsonObject = nlohmann::json {
        {APPLICATION_NAME, applicationInfo.name},
        {APPLICATION_BUNDLE_NAME, applicationInfo.bundleName},
        {APPLICATION_DESCRIPTION, applicationInfo.description},
        {APPLICATION_ICON_PATH, applicationInfo.iconPath},
        {APPLICATION_LABEL, applicationInfo.label},
        {APPLICATION_LABEL_ID, applicationInfo.labelId},
        {APPLICATION_DESCRIPTION_ID, applicationInfo.descriptionId},
        {APPLICATION_ICON_ID, applicationInfo.iconId},
        {APPLICATION_DEVICE_ID, applicationInfo.deviceId},
        {APPLICATION_SIGNATURE_KEY, applicationInfo.signatureKey},
        {APPLICATION_IS_SYSTEM_APP, applicationInfo.isSystemApp},
        {"isCloned", applicationInfo.isCloned},
        {APPLICATION_IS_LAUNCHER_APP, applicationInfo.isLauncherApp},
        {APPLICATION_ENABLED, applicationInfo.enabled},
        {APPLICATION_DEBUG, applicationInfo.debug},
        {APPLICATION_REMOVABLE, applicationInfo.removable},
        {APPLICATION_SUPPORTED_MODES, applicationInfo.supportedModes},
        {APPLICATION_PROCESS, applicationInfo.process},
        {APPLICATION_PERMISSIONS, applicationInfo.permissions},
        {APPLICATION_MODULE_SOURCE_DIRS, applicationInfo.moduleSourceDirs},
        {APPLICATION_MODULE_INFOS, applicationInfo.moduleInfos},
        {APPLICATION_ENTRY_DIR, applicationInfo.entryDir},
        {APPLICATION_CODE_PATH, applicationInfo.codePath},
        {APPLICATION_DATA_DIR, applicationInfo.dataDir},
        {APPLICATION_DATA_BASE_DIR, applicationInfo.dataBaseDir},
        {APPLICATION_CACHE_DIR, applicationInfo.cacheDir},
        {APPLICATION_FLAGS, applicationInfo.flags},
        {"uid", applicationInfo.uid}
    };
}

void from_json(const nlohmann::json &jsonObject, ApplicationInfo &applicationInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_NAME,
        applicationInfo.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_BUNDLE_NAME,
        applicationInfo.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DESCRIPTION,
        applicationInfo.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_ICON_PATH,
        applicationInfo.iconPath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_LABEL,
        applicationInfo.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APPLICATION_LABEL_ID,
        applicationInfo.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DESCRIPTION_ID,
        applicationInfo.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APPLICATION_ICON_ID,
        applicationInfo.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DEVICE_ID,
        applicationInfo.deviceId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_SIGNATURE_KEY,
        applicationInfo.signatureKey,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APPLICATION_IS_SYSTEM_APP,
        applicationInfo.isSystemApp,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        "isCloned",
        applicationInfo.isCloned,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APPLICATION_IS_LAUNCHER_APP,
        applicationInfo.isLauncherApp,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APPLICATION_ENABLED,
        applicationInfo.enabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DEBUG,
        applicationInfo.debug,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    // To be compatible with the data of the old image.
    // if the app is a system app and is not configured with removable,
    // the application cannot be uninstalled.
    parseResult = ERR_OK;
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APPLICATION_REMOVABLE,
        applicationInfo.removable,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult == ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP
        && applicationInfo.isSystemApp) {
        applicationInfo.removable = false;
    }
    parseResult = ERR_OK;
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        APPLICATION_SUPPORTED_MODES,
        applicationInfo.supportedModes,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_PROCESS,
        applicationInfo.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        APPLICATION_PERMISSIONS,
        applicationInfo.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        APPLICATION_MODULE_SOURCE_DIRS,
        applicationInfo.moduleSourceDirs,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<ModuleInfo>>(jsonObject,
        jsonObjectEnd,
        APPLICATION_MODULE_INFOS,
        applicationInfo.moduleInfos,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_ENTRY_DIR,
        applicationInfo.entryDir,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_CODE_PATH,
        applicationInfo.codePath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DATA_DIR,
        applicationInfo.dataDir,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_DATA_BASE_DIR,
        applicationInfo.dataBaseDir,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APPLICATION_CACHE_DIR,
        applicationInfo.cacheDir,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        APPLICATION_FLAGS,
        applicationInfo.flags,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int>(jsonObject,
        jsonObjectEnd,
        "uid",
        applicationInfo.uid,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void ApplicationInfo::ConvertToCompatibleApplicationInfo(CompatibleApplicationInfo& compatibleApplicationInfo) const
{
    APP_LOGD("ApplicationInfo::ConvertToCompatibleApplicationInfo called");
    compatibleApplicationInfo.name = name;
    compatibleApplicationInfo.icon = icon;
    compatibleApplicationInfo.label = label;
    compatibleApplicationInfo.description = description;
    compatibleApplicationInfo.cpuAbi = cpuAbi;
    compatibleApplicationInfo.process = process;
    compatibleApplicationInfo.systemApp = isSystemApp;
    compatibleApplicationInfo.isCompressNativeLibs = isCompressNativeLibs;
    compatibleApplicationInfo.iconId = iconId;
    compatibleApplicationInfo.labelId = labelId;
    compatibleApplicationInfo.descriptionId = descriptionId;
    compatibleApplicationInfo.permissions = permissions;
    compatibleApplicationInfo.moduleInfos = moduleInfos;
    compatibleApplicationInfo.supportedModes = supportedModes;
    compatibleApplicationInfo.enabled = enabled;
    compatibleApplicationInfo.debug = debug;
}
}  // namespace AppExecFwk
}  // namespace OHOS
