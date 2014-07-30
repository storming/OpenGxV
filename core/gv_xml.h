#ifndef __GV_XML_H__
#define __GV_XML_H__

#include <string>
#include "gv_object.h"

GV_NS_BEGIN

enum class XmlNodeType {
    DOC,
    ELEM,
    ATTR,
    TEXT,
};

class XmlElem;

class XmlNode : public Object {
    friend class XmlNodeContainer;
    friend class XmlElem;
public:
    XmlNode *parent() const {
        return _parent;
    }
    XmlNodeType type() const {
        return _type;
    }
protected:
    XmlNode(XmlNodeType type) noexcept : _type(type), _parent() { }

    XmlNode    *_parent;
    XmlNodeType _type;
};

class XmlNodeContainer : public XmlNode {
public:
    const std::vector<ptr<XmlNode>> &children() const noexcept {
        return _children;
    }
    XmlNode *addChild(const ptr<XmlNode> &node) noexcept {
        gv_assert(node->type() != XmlNodeType::ATTR && 
                  node->type() != XmlNodeType::DOC, 
                  "invalid child type.");
        _children.emplace_back(node);
        node->_parent = this;
        return node;
    }

    XmlElem *child(const char *name) const noexcept;
protected:
    XmlNodeContainer(XmlNodeType type) noexcept : XmlNode(type) { }

    std::vector<ptr<XmlNode>> _children;
};

class XmlText : public XmlNode {
    friend class Object;
public:
    const char *value() const noexcept {
        return _value.c_str();
    }
    void value(const char *str, size_t size = 0) noexcept {
        if (!size) {
            size = strlen(str);
        }
        _value.assign(str, size); 
    }
protected:
    XmlText() noexcept : XmlNode(XmlNodeType::TEXT) { }

    std::string _value;
};

class XmlAttr : public XmlNode {
    friend class Object;
public:
    const char *name() const noexcept {
        return _name.c_str();
    }
    const char *value() const noexcept {
        return _value.c_str();
    }
    void value(const char *newValue) noexcept {
        _value = newValue;
    }
    int toInt() const {
        return std::stoi(_value);
    }
    float toFloat() const {
        return std::stof(_value);
    }
    operator int() const {
        return std::stoi(_value);
    }
    operator float() const {
        return std::stof(_value);
    }
    operator const char*() const {
        return _value.c_str();
    }
protected:
    XmlAttr(const char *name) noexcept 
    : XmlNode(XmlNodeType::ATTR), _name(name) {}
    XmlAttr(const char *name, const char *value) noexcept 
    : XmlNode(XmlNodeType::ATTR), _name(name), _value(value) {}
    std::string _name;
    std::string _value;
};

class XmlElem : public XmlNodeContainer {
    friend class Object;
public:
    const char *name() const noexcept {
        return _name.c_str();
    }
    const std::vector<ptr<XmlAttr>> &attrs() const noexcept {
        return _attrs;
    }
    XmlAttr *attr(const char *name) const noexcept {
        for (XmlAttr *a : _attrs) {
            if (name == a->name()) {
                return a;
            }
        }
        return nullptr;
    }
    int attr(const char *name, int defaultValue) const noexcept {
        const XmlAttr *a = attr(name);
        return a ? (int)*a : defaultValue;
    }
    float attr(const char *name, float defaultValue) const noexcept {
        const XmlAttr *a = attr(name);
        return a ? (float)*a : defaultValue;
    }
    const char *attr(const char *name, const char *defaultValue) const noexcept {
        const XmlAttr *a = attr(name);
        return a ? a->value() : defaultValue;
    }
    XmlAttr *addAttr(const ptr<XmlAttr> &attr) noexcept {
        gv_assert(attr->type() == XmlNodeType::ATTR, "type is not attr.");
        _attrs.emplace_back(attr);
        attr->_parent = this;
        return attr;
    }
    XmlAttr *addAttr(const char *name, const char *value) noexcept {
        if (!value) {
            value = "";
        }
        return addAttr(object<XmlAttr>(name, value));
    }
protected:
    std::string _name;
    std::vector<ptr<XmlAttr>> _attrs;
    XmlElem(const char *name) : XmlNodeContainer(XmlNodeType::ELEM), _name(name) {}
};

class XmlDoc : public XmlNodeContainer {
    friend class Object;
public:
    bool load(const ptr<Chunk> &data) noexcept;
    const std::string &errorMsg() const noexcept {
        return _errorMsg;
    }
    unsigned errorLine() const noexcept {
        return _errorLine;
    }
private:
    XmlDoc() noexcept : XmlNodeContainer(XmlNodeType::DOC), _errorLine() { }
    std::string _errorMsg;
    unsigned _errorLine;
};


inline XmlElem *XmlNodeContainer::child(const char *name) const noexcept {
    for (XmlNode *node : _children) {
        if (node->type() == XmlNodeType::ELEM && !strcmp(name, static_cast<XmlElem*>(node)->name())) {
            return static_cast<XmlElem*>(node);
        }
    }
    return nullptr;
}

GV_NS_END


#endif
