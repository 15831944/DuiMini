/**
 * Copyright (c) 2018-2050
 * All rights reserved.
 *
 * @Author:MXWXZ
 * @Date:2018/01/28
 *
 * @Description:
 */
#include "stdafx.h"
#include "UIImage.h"

namespace DuiMini {
UIImage::UIImage() {}

UIImage::~UIImage() {
    delete img_;
    img_ = nullptr;
}

bool UIImage::SetFile(LPCTSTR v_path) {
    UIRenderImage* tmp = new UIRenderImage();
    if (tmp->Load(v_path)) {
        delete img_;
        img_ = tmp;
        SetAttribute(_T("file"), v_path);
        return true;
    }
    delete tmp;
    tmp = nullptr;
    return false;
}

void UIImage::BeforeSetAttribute() {
    SetAttribute(_T("file"), _T(""));
    UIControl::BeforeSetAttribute();
}

LPVOID UIImage::GetInterface(LPCTSTR v_name) {
    if (CmpStr(v_name, CTRLNAME_IMAGE))
        return this;
    return UIControl::GetInterface(v_name);
}

void UIImage::Paint() {
    basewnd_->GetRender()->DrawImage(img_, rect_.left, rect_.top,
                                     rect_.right - rect_.left,
                                     rect_.bottom - rect_.top);
}

void UIImage::LoadResAttr() {
    SetFile(GetAttrFile(_T("file")));
    UIControl::LoadResAttr();
}

}   // namespace DuiMini
