/** @file
 * Raw resource loader exposed to user.
 * This loader will return the resource in binary form.
 *
 * @author  MXWXZ
 * @date    2019/03/08
 */
#ifndef DUI_CORE_RESOURCE_UIRESLOADER_HPP_
#define DUI_CORE_RESOURCE_UIRESLOADER_HPP_

#include "UIDefine.h"
#include "Utils/UIUtils.h"
#include "Core/UIInterface.h"
#include "Core/Resource/UIResource.h"

namespace DuiMini {
class DUIMINI_API UIRawLoader : public IUILoadFile {
public:
    const void* GetFile() const { return buffer_; }
    long GetFileSize() const { return buffersize_; }
    bool LoadFile(const void* buffer, long size) override {
        buffer_ = buffer;
        buffersize_ = size;
        return true;
    }

protected:
    const void* buffer_;
    long buffersize_;
};
}  // namespace DuiMini

#endif  // !DUI_CORE_RESOURCE_UIRESLOADER_HPP_
