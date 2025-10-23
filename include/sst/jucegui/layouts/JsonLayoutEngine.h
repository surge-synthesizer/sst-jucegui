/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-jucegui available at
 * https://github.com/surge-synthesizer/sst-jucegui
 */

#ifndef INCLUDE_SST_JUCEGUI_LAYOUTS_JSONLAYOUTENGINE_H
#define INCLUDE_SST_JUCEGUI_LAYOUTS_JSONLAYOUTENGINE_H

#include <string>
#include <utility>
#include <optional>
#include <unordered_map>
#include <sstream>
#include <vector>

namespace sst::jucegui::layouts
{

namespace json_document
{

struct Class
{
    std::string name;
    std::string controlType;
    int w{-1}, h{-1};
    std::string style;
    bool allowsLabel{false};

    std::unordered_map<std::string, std::string> extraKVs;

    std::string toString() const
    {
        std::ostringstream ss;
        ss << "Class " << name << " type=" << controlType << " " << w << "x" << h
           << " style=" << style;
        return ss.str();
    }
};

struct Binding
{
    int index{0};
    std::string type;

    std::string toString() const { return type + " " + std::to_string(index); }
};

struct Position
{
    int x{-1}, y{-1}, w{-1}, h{-1};

    std::string toString() const
    {
        return "@" + std::to_string(x) + "," + std::to_string(y) + " " + std::to_string(w) + "x" +
               std::to_string(h);
    }
};

struct EnabledIf
{
    int index{0};
    std::string type;
    std::vector<int> values;

    std::string toString() const
    {
        std::ostringstream ss;
        ss << "EnabledIf " << type << " " << index << " values=[";
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i > 0)
                ss << ",";
            ss << values[i];
        }
        ss << "]";
        return ss.str();
    }
};

struct VisibleIf
{
    int index{0};
    std::string type;
    std::vector<int> values;

    std::string toString() const
    {
        std::ostringstream ss;
        ss << "VisibleIf " << type << " " << index << " values=[";
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i > 0)
                ss << ",";
            ss << values[i];
        }
        ss << "]";
        return ss.str();
    }
};

struct Control
{
    std::string name;
    std::string className;
    std::optional<Binding> binding;
    std::optional<EnabledIf> enabledIf;
    std::optional<VisibleIf> visibleIf;
    Position position;
    std::optional<std::string> fixedValue;
    std::optional<std::string> label;

    std::string toString() const
    {
        std::ostringstream ss;
        ss << "Control " << name << " " << className;
        if (binding.has_value())
            ss << " Bound: " << binding->toString();
        if (enabledIf.has_value())
            ss << " " << enabledIf->toString();
        if (visibleIf.has_value())
            ss << " " << visibleIf->toString();
        ss << " " << position.toString();
        if (fixedValue.has_value())
            ss << " With Value: " << *fixedValue;
        if (label.has_value())
            ss << " Label:" << *label;
        return ss.str();
    }
};
} // namespace json_document

struct JsonLayoutRetVal
{
    JsonLayoutRetVal() : success(true) {}
    JsonLayoutRetVal(const std::string &e) : success(false), error(e) {}
    JsonLayoutRetVal(const char *e) : success(false), error(e) {}

    bool success;
    std::optional<std::string> error;

    JsonLayoutRetVal withExtraError(const std::string &e) const
    {
        return JsonLayoutRetVal(error.value_or("") + "\n" + e);
    }

    operator bool() const { return success; }
};

struct JsonLayoutHost
{
    virtual ~JsonLayoutHost() = default;

    // These return types both suck
    virtual std::string resolveJsonPath(const std::string &path) const = 0;
    virtual void createBindAndPosition(const json_document::Control &,
                                       const json_document::Class &) = 0;
};

struct JsonLayoutEngine
{
    JsonLayoutHost &host;
    JsonLayoutEngine(JsonLayoutHost &h) : host(h) {}

    using retval_t = JsonLayoutRetVal;

    [[nodiscard]] retval_t processJsonPath(const std::string &path);

    std::unordered_map<std::string, json_document::Class> classMap;
    std::unordered_map<std::string, json_document::Control> controlMap;

  private:
    [[nodiscard]] retval_t parseClasses(juce::DynamicObject *);
    [[nodiscard]] retval_t parseIndividualClass(juce::DynamicObject *,
                                                const std::string &className);
    [[nodiscard]] retval_t parseControls(juce::DynamicObject *);
    [[nodiscard]] retval_t parseSingleControl(juce::DynamicObject *, const std::string &cname);
    [[nodiscard]] retval_t parseSingleBinding(juce::DynamicObject *, json_document::Binding &);
    [[nodiscard]] retval_t parseSingleEnabledIf(juce::DynamicObject *, json_document::EnabledIf &);
    [[nodiscard]] retval_t parseSingleVisibleIf(juce::DynamicObject *, json_document::VisibleIf &);
};

} // namespace sst::jucegui::layouts
#endif // SHORTCIRCUITXT_JSONLAYOUTENGINE_H
