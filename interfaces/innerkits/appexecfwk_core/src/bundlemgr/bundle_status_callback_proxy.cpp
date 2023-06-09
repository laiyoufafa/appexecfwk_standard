/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bundle_status_callback_proxy.h"

#include "ipc_types.h"
#include "parcel.h"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
std::string TransformResult(ErrCode resultCode)
{
    switch (resultCode) {
        case ERR_OK:
            return "SUCCESS";
        case ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR:
            return "ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR";
        case ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED:
            return "ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED";
        case ERR_APPEXECFWK_INSTALL_PARSE_FAILED:
            return "ERR_APPEXECFWK_INSTALL_PARSE_FAILED";
        case ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE:
            return "ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE";
        case ERR_APPEXECFWK_INSTALL_VERIFICATION_FAILED:
            return "ERR_APPEXECFWK_INSTALL_VERIFICATION_FAILED";
        case ERR_APPEXECFWK_INSTALL_FAILED_INVALID_SIGNATURE_FILE_PATH:
            return "ERR_APPEXECFWK_INSTALL_FAILED_INVALID_SIGNATURE_FILE_PATH";
        case ERR_APPEXECFWK_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE_FILE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE_FILE";
        case ERR_APPEXECFWK_INSTALL_FAILED_NO_BUNDLE_SIGNATURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_NO_BUNDLE_SIGNATURE";
        case ERR_APPEXECFWK_INSTALL_FAILED_VERIFY_APP_PKCS7_FAIL:
            return "ERR_APPEXECFWK_INSTALL_FAILED_VERIFY_APP_PKCS7_FAIL";
        case ERR_APPEXECFWK_INSTALL_FAILED_PROFILE_PARSE_FAIL:
            return "ERR_APPEXECFWK_INSTALL_FAILED_PROFILE_PARSE_FAIL";
        case ERR_APPEXECFWK_INSTALL_FAILED_APP_SOURCE_NOT_TRUESTED:
            return "ERR_APPEXECFWK_INSTALL_FAILED_APP_SOURCE_NOT_TRUESTED";
        case ERR_APPEXECFWK_INSTALL_FAILED_BAD_DIGEST:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BAD_DIGEST";
        case ERR_APPEXECFWK_INSTALL_FAILED_BUNDLE_INTEGRITY_VERIFICATION_FAILURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BUNDLE_INTEGRITY_VERIFICATION_FAILURE";
        case ERR_APPEXECFWK_INSTALL_FAILED_FILE_SIZE_TOO_LARGE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_FILE_SIZE_TOO_LARGE";
        case ERR_APPEXECFWK_INSTALL_FAILED_BAD_PUBLICKEY:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BAD_PUBLICKEY";
        case ERR_APPEXECFWK_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE";
        case ERR_APPEXECFWK_INSTALL_FAILED_NO_PROFILE_BLOCK_FAIL:
            return "ERR_APPEXECFWK_INSTALL_FAILED_NO_PROFILE_BLOCK_FAIL";
        case ERR_APPEXECFWK_INSTALL_FAILED_BUNDLE_SIGNATURE_VERIFICATION_FAILURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_BUNDLE_SIGNATURE_VERIFICATION_FAILURE";
        case ERR_APPEXECFWK_INSTALL_FAILED_VERIFY_SOURCE_INIT_FAIL:
            return "ERR_APPEXECFWK_INSTALL_FAILED_VERIFY_SOURCE_INIT_FAIL";
        case ERR_APPEXECFWK_INSTALL_FAILED_INCOMPATIBLE_SIGNATURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_INCOMPATIBLE_SIGNATURE";
        case ERR_APPEXECFWK_INSTALL_FAILED_INCONSISTENT_SIGNATURE:
            return "ERR_APPEXECFWK_INSTALL_FAILED_INCONSISTENT_SIGNATURE";
        case ERR_APPEXECFWK_INSTALL_PARAM_ERROR:
            return "ERR_APPEXECFWK_INSTALL_PARAM_ERROR";
        case ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED:
            return "ERR_APPEXECFWK_INSTALL_PERMISSION_DENIED";
        case ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST:
            return "ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST";
        case ERR_APPEXECFWK_INSTALL_STATE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_STATE_ERROR";
        case ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID:
            return "ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID";
        case ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME:
            return "ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME";
        case ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE:
            return "ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE";
        case ERR_APPEXECFWK_INSTALL_INVALID_HAP_SIZE:
            return "ERR_APPEXECFWK_INSTALL_INVALID_HAP_SIZE";
        case ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR:
            return "ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR";
        case ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR";
        case ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR";
        case ERR_APPEXECFWK_INSTALL_ALREADY_EXIST:
            return "ERR_APPEXECFWK_INSTALL_ALREADY_EXIST";
        case ERR_APPEXECFWK_INSTALL_BUNDLENAME_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_BUNDLENAME_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_VERSIONCODE_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_VERSIONCODE_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_VERSIONNAME_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_VERSIONNAME_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_MINCOMPATIBLE_VERSIONCODE_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_MINCOMPATIBLE_VERSIONCODE_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_VENDOR_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_VENDOR_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_RELEASETYPE_TARGET_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_RELEASETYPE_TARGET_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_RELEASETYPE_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_RELEASETYPE_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_VERSION_NOT_COMPATIBLE:
            return "ERR_APPEXECFWK_INSTALL_VERSION_NOT_COMPATIBLE";
        case ERR_APPEXECFWK_INSTALL_INVALID_NUMBER_OF_ENTRY_HAP:
            return "ERR_APPEXECFWK_INSTALL_INVALID_NUMBER_OF_ENTRY_HAP";
        case ERR_APPEXECFWK_INSTALL_DISK_MEM_INSUFFICIENT:
            return "ERR_APPEXECFWK_INSTALL_DISK_MEM_INSUFFICIENT";
        case ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED:
            return "ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED";
        case ERR_APPEXECFWK_INSTALL_UPDATE_HAP_TOKEN_FAILED:
            return "ERR_APPEXECFWK_INSTALL_UPDATE_HAP_TOKEN_FAILED";
        case ERR_APPEXECFWK_INSTALL_CHECK_SYSCAP_FAILED:
            return "ERR_APPEXECFWK_INSTALL_CHECK_SYSCAP_FAILED";
        case ERR_APPEXECFWK_INSTALL_APPTYPE_NOT_SAME:
            return "ERR_APPEXECFWK_INSTALL_APPTYPE_NOT_SAME";
        case ERR_APPEXECFWK_INSTALL_URI_DUPLICATE:
            return "ERR_APPEXECFWK_INSTALL_URI_DUPLICATE";
        case ERR_APPEXECFWK_INSTALL_TYPE_ERROR:
            return "ERR_APPEXECFWK_INSTALL_TYPE_ERROR";
        case ERR_APPEXECFWK_FAILED_SERVICE_DIED:
            return "ERR_APPEXECFWK_FAILED_SERVICE_DIED";
        case ERR_APPEXECFWK_PARSE_UNEXPECTED:
            return "ERR_APPEXECFWK_PARSE_UNEXPECTED";
        case ERR_APPEXECFWK_PARSE_MISSING_BUNDLE:
            return "ERR_APPEXECFWK_PARSE_MISSING_BUNDLE";
        case ERR_APPEXECFWK_PARSE_MISSING_ABILITY:
            return "ERR_APPEXECFWK_PARSE_MISSING_ABILITY";
        case ERR_APPEXECFWK_PARSE_NO_PROFILE:
            return "ERR_APPEXECFWK_PARSE_NO_PROFILE";
        case ERR_APPEXECFWK_PARSE_BAD_PROFILE:
            return "ERR_APPEXECFWK_PARSE_BAD_PROFILE";
        case ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR:
            return "ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR";
        case ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP:
            return "ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP";
        case ERR_APPEXECFWK_PARSE_PERMISSION_ERROR:
            return "ERR_APPEXECFWK_PARSE_PERMISSION_ERROR";
        case ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR:
            return "ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR";
        case ERR_APPEXECFWK_PARSE_RPCID_FAILED:
            return "ERR_APPEXECFWK_PARSE_RPCID_FAILED";
        case ERR_APPEXECFWK_INSTALLD_PARAM_ERROR:
            return "ERR_APPEXECFWK_INSTALLD_PARAM_ERROR";
        case ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR:
            return "ERR_APPEXECFWK_INSTALLD_GET_PROXY_ERROR";
        case ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CREATE_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST:
            return "ERR_APPEXECFWK_INSTALLD_CREATE_DIR_EXIST";
        case ERR_APPEXECFWK_INSTALLD_CHOWN_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CHOWN_FAILED";
        case ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_REMOVE_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_EXTRACT_FILES_FAILED";
        case ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_RNAME_DIR_FAILED";
        case ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED:
            return "ERR_APPEXECFWK_INSTALLD_CLEAN_DIR_FAILED";
        case ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_INVALID_NAME:
            return "ERR_APPEXECFWK_UNINSTALL_INVALID_NAME";
        case ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_PARAM_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED:
            return "ERR_APPEXECFWK_UNINSTALL_PERMISSION_DENIED";
        case ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR:
            return "ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR";
        case ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE:
            return "ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE";
        case ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE:
            return "ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE";
        case ERR_APPEXECFWK_RECOVER_GET_BUNDLEPATH_ERROR:
            return "ERR_APPEXECFWK_RECOVER_GET_BUNDLEPATH_ERROR";
        case ERR_APPEXECFWK_RECOVER_INVALID_BUNDLE_NAME:
            return "ERR_APPEXECFWK_RECOVER_INVALID_BUNDLE_NAME";
        case ERR_APPEXECFWK_USER_NOT_EXIST:
            return "ERR_APPEXECFWK_USER_NOT_EXIST";
        case ERR_APPEXECFWK_USER_CREATE_FALIED:
            return "ERR_APPEXECFWK_USER_CREATE_FALIED";
        case ERR_APPEXECFWK_USER_REMOVE_FALIED:
            return "ERR_APPEXECFWK_USER_REMOVE_FALIED";
        case ERR_APPEXECFWK_USER_NOT_INSTALL_HAP:
            return "ERR_APPEXECFWK_USER_NOT_INSTALL_HAP";
        default:
            return "";
    }
}
}  // namespace

BundleStatusCallbackProxy::BundleStatusCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IBundleStatusCallback>(object)
{
    APP_LOGI("create bundle status callback proxy instance");
}

BundleStatusCallbackProxy::~BundleStatusCallbackProxy()
{
    APP_LOGI("destroy bundle status callback proxy instance");
}

void BundleStatusCallbackProxy::OnBundleStateChanged(
    const uint8_t installType, const int32_t resultCode, const std::string &resultMsg, const std::string &bundleName)
{
    APP_LOGI("bundle state changed %{public}s", bundleName.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(BundleStatusCallbackProxy::GetDescriptor())) {
        APP_LOGE("fail to OnBundleStateChanged due to write MessageParcel fail");
        return;
    }
    if (!data.WriteUint8(installType)) {
        APP_LOGE("fail to call OnBundleStateChanged, for write installType failed");
        return;
    }
    if (!data.WriteInt32(resultCode)) {
        APP_LOGE("fail to call OnBundleStateChanged, for write resultCode failed");
        return;
    }
    std::string msg = TransformResult(resultCode);
    if (!data.WriteString16(Str8ToStr16(msg))) {
        APP_LOGE("fail to call OnBundleStateChanged, for write resultMsg failed");
        return;
    }
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        APP_LOGE("fail to call OnBundleStateChanged, for write bundleName failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to call OnBundleStateChanged, for Remote() is nullptr");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IBundleStatusCallback::Message::ON_BUNDLE_STATE_CHANGED), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("fail to call OnBundleStateChanged, for transact is failed, error code is: %{public}d", ret);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
