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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_ON_PERMISSION_CHANGED_CALLBACK_PROXY_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_ON_PERMISSION_CHANGED_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "on_permission_changed_callback_interface.h"

namespace OHOS {
namespace AppExecFwk {
class OnPermissionChangedCallbackProxy : public IRemoteProxy<OnPermissionChangedCallback> {
public:
    explicit OnPermissionChangedCallbackProxy(const sptr<IRemoteObject> &object);
    virtual ~OnPermissionChangedCallbackProxy() override;
    /**
     * @brief Called when an application's permission changed.
     * @param uid Indicates the uid of the application which permission changed.
     */
    virtual void OnChanged(const int32_t uid) override;

private:
    static inline BrokerDelegator<OnPermissionChangedCallbackProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_ON_PERMISSION_CHANGED_CALLBACK_PROXY_H
