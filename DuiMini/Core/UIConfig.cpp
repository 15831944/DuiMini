/**
 * Copyright (c) 2018-2050
 * All rights reserved.
 *
 * @Author:MXWXZ
 * @Date:2018/03/20
 */
#include "stdafx.h"
#include "UIConfig.h"

namespace DuiMini {
UICFGItem UIConfig::dlg_;
CFGID     UIConfig::shownskin_ = 0;
CFGID     UIConfig::shownlang_ = 0;
CFGID     UIConfig::shownfont_ = 0;
UICFGItem UIConfig::lang_;
UICFGItem UIConfig::skin_;
UICFGItem UIConfig::font_;
UICFGItem UIConfig::sys_res_id_;
UICFGItem UIConfig::res_id_;
UICFGItem UIConfig::lang_str_;
UIFont    UIConfig::font_style_;

#define CFG_BeginAttr  UIAttr _now
#define CFG_EndAttr(x) x.push_back(_now)
#define CFG_AddAttrStatic(x, y) _now[x] = y
#define CFG_AddAttr(x) _now[x] = tmp.GetAttrValue(x)
#define CFG_AddAttrDef(x, y) _now[x] = tmp.GetAttrValue(x, y)
#define CFG_CmpAttr(x, y) _now[x] == y

void UIConfig::LoadConfig(LPCTSTR v_relativepath/* = DEFAULT_RESFILE*/) {
    UIXmlLoader *config = (UIXmlLoader*)UIResource::LoadRes(kFT_XML, v_relativepath);
    for (UIXmlNode node = config->GetRoot().FirstChild();
         node;
         node = node.NextSibling()) {
        UIXmlNode tmp(node);
        // style config
        if (tmp.CmpNodeName(_T("style"))) {
            // skin config
            if (tmp.CmpAttrValue(_T("type"), _T("skin"))) {
                CFG_BeginAttr;
                CFG_AddAttr(_T("name"));
                CFG_AddAttr(_T("value"));
                CFG_AddAttrDef(_T("default"), 0);
                CFG_AddAttrDef(_T("system"), 0);
                if (CFG_CmpAttr(_T("system"), _T("1")))
                    CFG_AddAttrStatic(_T("default"), _T("0"));
                CFG_EndAttr(skin_);
                if (CFG_CmpAttr(_T("system"), _T("1")))
                    AddSystemSkin(skin_.size() - 1);
                if (CFG_CmpAttr(_T("system"), _T("0")) &&
                    CFG_CmpAttr(_T("default"), _T("1")))
                    SetShownSkin(skin_.size() - 1);
            }

            // font config
            if (tmp.CmpAttrValue(_T("type"), _T("font"))) {
                CFG_BeginAttr;
                CFG_AddAttr(_T("lang"));
                CFG_AddAttr(_T("name"));
                CFG_AddAttr(_T("font"));
                CFG_AddAttr(_T("size"));
                CFG_AddAttrDef(_T("bold"), 0);
                CFG_AddAttrDef(_T("italic"), 0);
                CFG_AddAttrDef(_T("underline"), 0);
                CFG_AddAttrDef(_T("strikeout"), 0);
                CFG_AddAttrDef(_T("default"), 0);
                CFG_EndAttr(font_);
                if (CFG_CmpAttr(_T("default"), _T("1")) &&
                    CFG_CmpAttr(_T("lang"), lang_[shownlang_][_T("lang")]))
                    SetShownFont(font_.size() - 1);
            }
        }

        // res config
        if (tmp.CmpNodeName(_T("res"))) {
            // dlg config
            if (tmp.CmpAttrValue(_T("type"), _T("dlg"))) {
                CFG_BeginAttr;
                CFG_AddAttr(_T("name"));
                CFG_AddAttr(_T("file"));
                CFG_EndAttr(dlg_);
            }
            // lang config
            if (tmp.CmpAttrValue(_T("type"), _T("lang"))) {
                CFG_BeginAttr;
                CFG_AddAttr(_T("lang"));
                CFG_AddAttr(_T("file"));
                CFG_AddAttrDef(_T("default"), 0);
                CFG_EndAttr(lang_);
                if (CFG_CmpAttr(_T("default"), _T("1")))
                    SetShownLang(lang_.size() - 1);
            }
        }
    }
    UIResource::ReleaseRes(config);
}

CFGID UIConfig::FindLangID(LPCTSTR v_name) {
    return FindItemID(lang_, v_name);
}

CFGID UIConfig::FindFontID(LPCTSTR v_name) {
    return FindItemID(font_, v_name);
}

CFGID UIConfig::FindSkinID(LPCTSTR v_name) {
    return FindItemID(skin_, v_name);
}

bool UIConfig::SetShownLang(CFGID v_id) {
    if (v_id >= lang_.size()) {
        ErrorMsg_IDInvalid(v_id);
        return false;
    }

    lang_str_.clear();
    UIXmlLoader *file = (UIXmlLoader*)UIResource::LoadRes(kFT_XML, lang_[v_id][_T("file")]);
    for (UIXmlNode node = file->GetRoot().FirstChild();
         node;
         node = node.NextSibling()) {
        UIXmlNode tmp(node);
        CFG_BeginAttr;
        CFG_AddAttr(_T("name"));
        CFG_AddAttr(_T("value"));
        CFG_EndAttr(lang_str_);
    }
    shownlang_ = v_id;
    UIResource::ReleaseRes(file);
    return true;
}

bool UIConfig::SetShownFont(CFGID v_id) {
    if (v_id >= font_.size()) {
        ErrorMsg_IDInvalid(v_id);
        return false;
    }

    UIAttr &nowfont = font_[v_id];
    if (nowfont[_T("lang")] != lang_[shownlang_][_T("lang")])
        ErrorMsg_FormatInvalid(nowfont[_T("font")].GetData());

    font_style_.name_ = nowfont[_T("name")];
    font_style_.lang_ = nowfont[_T("lang")];
    font_style_.font_ = nowfont[_T("font")];
    font_style_.size_ = nowfont[_T("size")].Str2LL();
    font_style_.bold_ = nowfont[_T("bold")].Str2LL();
    font_style_.italic_ = nowfont[_T("italic")].Str2LL();
    font_style_.underline_ = nowfont[_T("underline")].Str2LL();
    font_style_.strikeout_ = nowfont[_T("strikeout")].Str2LL();
    shownfont_ = v_id;
    return true;
}

bool UIConfig::SetShownSkin(CFGID v_id) {
    if (v_id >= skin_.size()) {
        ErrorMsg_IDInvalid(v_id);
        return false;
    }

    res_id_.clear();
    UIXmlLoader *file = (UIXmlLoader*)UIResource::LoadRes(kFT_XML, skin_[v_id][_T("value")] + _T("\\resid.xml"));
    for (UIXmlNode node = file->GetRoot().FirstChild();
         node;
         node = node.NextSibling()) {
        UIXmlNode tmp(node);
        CFG_BeginAttr;
        CFG_AddAttr(_T("type"));
        CFG_AddAttr(_T("name"));
        CFG_AddAttr(_T("file"));
        CFG_EndAttr(res_id_);
    }
    shownskin_ = v_id;
    UIResource::ReleaseRes(file);
    return true;
}

bool UIConfig::AddSystemSkin(CFGID v_id) {
    if (v_id >= skin_.size()) {
        ErrorMsg_IDInvalid(v_id);
        return false;
    }

    UIXmlLoader *file = (UIXmlLoader*)UIResource::LoadRes(kFT_XML, skin_[v_id][_T("value")] + _T("\\resid.xml"));
    for (UIXmlNode node = file->GetRoot().FirstChild();
         node;
         node = node.NextSibling()) {
        UIXmlNode tmp(node);
        CFG_BeginAttr;
        CFG_AddAttr(_T("type"));
        CFG_AddAttr(_T("name"));
        CFG_AddAttr(_T("file"));
        CFG_EndAttr(sys_res_id_);
    }
    UIResource::ReleaseRes(file);
    return true;
}

UIAttr* UIConfig::FindDlg(LPCTSTR v_name) {
    return FindItem(dlg_, v_name);
}

CUStr UIConfig::FindDlgFile(LPCTSTR v_name) {
    UIAttr* tmp = FindDlg(v_name);
    return tmp ? (*tmp)[_T("file")] : CUStr();
}

UIAttr* UIConfig::FindResid(LPCTSTR v_name) {
    UIAttr* ret = FindNextCFGItem(res_id_, 0, _T("name"), v_name);
    if (ret)
        return ret;
    ret = FindNextCFGItem(sys_res_id_, 0, _T("name"), v_name);
    if (ret)
        return ret;
    ErrorMsg_IDInvalid(v_name);
    return nullptr;
}

CUStr UIConfig::FindResidFile(LPCTSTR v_name) {
    UIAttr* resid = FindResid(v_name);
    return resid ? (*resid)[_T("file")] : CUStr();
}

CUStr UIConfig::GetStrPath(LPCTSTR v_str) {
    CUStr str = v_str;
    size_t len = str.GetLength();
    if (!len)
        return CUStr();
    if (str[0] == '[' && str[len - 1] == ']')
        return FindResidFile(str.Mid(1, len - 2));
    else
        return str;
}

UIAttr* UIConfig::FindLang(LPCTSTR v_name) {
    return FindItem(lang_str_, v_name);
}

CUStr UIConfig::FindLangValue(LPCTSTR v_name) {
    UIAttr* langstr = FindLang(v_name);
    return langstr ? (*langstr)[_T("value")] : CUStr();
}

UIAttr* UIConfig::FindFont(LPCTSTR v_name) {
    return FindItem(font_, v_name);
}

UIFont UIConfig::FindFontValue(LPCTSTR v_name) {
    UIAttr* tmp = FindFont(v_name);
    if (!tmp)
        return UIFont();
    UIAttr &retfont = *tmp;
    UIFont ret;
    ret.name_ = retfont[_T("name")];
    ret.lang_ = retfont[_T("lang")];
    ret.font_ = retfont[_T("font")];
    ret.size_ = (USHORT)retfont[_T("size")].Str2LL();
    ret.bold_ = retfont[_T("bold")].Str2LL();
    ret.italic_ = retfont[_T("italic")].Str2LL();
    ret.underline_ = retfont[_T("underline")].Str2LL();
    ret.strikeout_ = retfont[_T("strikeout")].Str2LL();
    return ret;
}

CUStr UIConfig::TranslateStr(LPCTSTR v_str) {
    UStr str = v_str;
    size_t len = str.GetLength();
    if (len == 0)
        return CUStr();
    if (str[0] == '[' && str[len - 1] == ']')
        return FindLangValue(str.Mid(1, len - 2));
    else if (str[0] == '\\' && str[1] == '[' && str[len - 1] == ']')
        return str.Mid(1, len - 1);
    else
        return str;
}

UIAttr* UIConfig::FindNextCFGItem(UICFGItem& v_item, UINT v_start,
                                  LPCTSTR v_name, LPCTSTR v_value) {
    if (v_start >= v_item.size())
        return nullptr;
    for (auto i = v_item.begin() + v_start; i != v_item.end(); ++i)
        if (CmpStr((*i).at(v_name), v_value))
            return &(*i);
    return nullptr;
}

CFGID UIConfig::FindItemID(const UICFGItem& v_item, LPCTSTR v_name) {
    for (auto i = v_item.begin(); i != v_item.end(); ++i)
        if (CmpStr((*i).at(_T("name")), v_name))
            return i - v_item.begin();
    ErrorMsg_IDInvalid(v_name);
    return 0;
}

UIAttr* UIConfig::FindItem(UICFGItem &v_item, LPCTSTR v_name) {
    UIAttr *ret = FindNextCFGItem(v_item, 0, _T("name"), v_name);
    if (ret)
        return ret;
    ErrorMsg_IDInvalid(v_name);
    return nullptr;
}
}   // namespace DuiMini
