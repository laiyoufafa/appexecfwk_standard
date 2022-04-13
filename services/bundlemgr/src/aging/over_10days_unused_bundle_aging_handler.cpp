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

#include "aging/aging_constants.h"
#include "aging/aging_handler.h"

namespace OHOS {
namespace AppExecFwk {
bool Over10DaysUnusedBundleAgingHandler::CheckBundle(const AgingBundleInfo &bundle) const
{
    return (AgingUtil::GetNowSysTimeMs() - bundle.GetRecentlyUsedTime()) >
        (AgingConstants::TIME_10_DAYS * AgingConstants::ONE_DAYS_MS);
}

const std::string &Over10DaysUnusedBundleAgingHandler::GetName() const
{
    return AgingConstants::UNUSED_FOR_10_DAYS_BUNDLE_AGING_HANDLER;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS