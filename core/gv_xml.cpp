#include "opengxv.h"
#include "gv_xml.h"
#include "expat.h"

GV_NS_BEGIN

struct XmlLoader {
    bool load(const ptr<Chunk> &data, std::string &errorMsg, unsigned &errorLine) noexcept {
        _parser = XML_ParserCreate(nullptr);
        if (!_parser) {
            return false;
        }
        XML_SetUserData(_parser, this);  
        XML_SetElementHandler(
            _parser, 
            [](void *userData, const XML_Char *name, const XML_Char **atts) {
                reinterpret_cast<XmlLoader*>(userData)->onStartElement(name, atts);
            }, 
            [](void *userData, const XML_Char *name) {
                 reinterpret_cast<XmlLoader*>(userData)->onEndElement(name);
            }
        );
        XML_SetCharacterDataHandler(
            _parser, 
            [](void *userData, const XML_Char *s, int len) {
                reinterpret_cast<XmlLoader*>(userData)->onCharacterData(s, len);
            }
        );

        if (!XML_Parse(_parser, (const char*)data->data(), data->size(), 1)) {
            errorMsg = XML_ErrorString(XML_GetErrorCode(_parser));
            errorLine = XML_GetCurrentLineNumber(_parser);
            return false;
        }
        
        return true;
    }

    void onStartElement(const XML_Char *name, const XML_Char **attrs) noexcept {
        object<XmlElem> node(name);

        if (attrs) {
            while (1) {
                const char *attr_name = *attrs++; 
                if (!attr_name) {
                    break;
                }
                const char *attr_value = *attrs++; 
                if (attr_value) {
                    node->addAttr(attr_name, attr_value);
                }
                else {
                    node->addAttr(attr_name, "");
                    break;
                }
            }
        }
        if (_cur) {
            _cur->addChild(node);
        }
        else {
            _root->addChild(node);
        }
        _cur = node;
    }

    void onEndElement(const XML_Char *name) noexcept {
        _cur = static_cast<XmlNodeContainer*>(_cur->parent());
    }

    void onCharacterData(const XML_Char *s, int len) noexcept {
        object<XmlText> text;
        text->value(s, (size_t)len);
        _cur->addChild(text);
    }

    XmlLoader() noexcept : _parser() { }
    ~XmlLoader() noexcept {
        if (_parser) {
            XML_ParserFree(_parser);  
        }
    }
    XML_Parser            _parser;
    ptr<XmlNodeContainer> _root;
    ptr<XmlNodeContainer> _cur;
};

bool XmlDoc::load(const ptr<Chunk> &data) noexcept {
    XmlLoader loader;
    loader._root = this;
    return loader.load(data, _errorMsg, _errorLine);
}

GV_NS_END
