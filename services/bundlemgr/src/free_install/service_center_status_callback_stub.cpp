/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "service_center_status_callback_stub.h"

#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "json_util.h"
#include "message_parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
ServiceCenterStatusCallbackStub::ServiceCenterStatusCallbackStub()
{
    APP_LOGI("ServiceCenterStatusCallbackStub is created");
}

int32_t ServiceCenterStatusCallbackStub::OnInstallFinishedInner(MessageParcel &data, MessageParcel &reply)
{
    auto result = data.ReadString16();
    OnInstallFinished(Str16ToStr8(result));
    return NO_ERROR;
}

int32_t ServiceCenterStatusCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    data.ReadInterfaceToken();
    return (this->OnInstallFinishedInner)(data, reply);
}
}  // namespace AppExecFwk
}  // namespace OHOS
