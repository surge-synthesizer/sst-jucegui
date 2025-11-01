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

#include <string>
#include <set>
#include <juce_core/juce_core.h>
#include "sst/jucegui/layouts/JsonLayoutEngine.h"

namespace sst::jucegui::layouts
{

JsonLayoutEngine::retval_t JsonLayoutEngine::processJsonPath(const std::string &path)
{
    // Get the JSON content from the host
    auto jsonContent = host.resolveJsonPath(path);

    if (jsonContent.empty())
    {
        return "Empty JSON Content";
    }

    // Parse the JSON using JUCE's JSON parser
    juce::var parsedJson;
    auto result = juce::JSON::parse(jsonContent, parsedJson);

    if (!result.wasOk())
    {
        return "Error Parsing: " + result.getErrorMessage().toStdString();
    }

    // Check if the parsed JSON is an object
    if (!parsedJson.isObject())
    {
        return "JSON root is not an object";
    }

    auto *jsonObject = parsedJson.getDynamicObject();
    if (!jsonObject)
    {
        return "Unable to load json dynamic object";
    }

    // Process the "include" field if it exists
    if (jsonObject->hasProperty("include"))
    {
        auto includeValue = jsonObject->getProperty("include");
        if (includeValue.isString())
        {
            auto res = processJsonPath(includeValue.toString().toStdString());
            if (!res)
                return res.withExtraError("Unable to parse included json '" +
                                          includeValue.toString().toStdString() + "'");
        }
    }

    // Process the "classes" field if it exists
    if (jsonObject->hasProperty("classes"))
    {
        auto classes = jsonObject->getProperty("classes");
        if (classes.isObject())
        {
            auto *classesObj = classes.getDynamicObject();
            if (classesObj)
            {
                auto res = parseClasses(classesObj);
                if (!res)
                    return res.withExtraError("Unable to parse classes map");
            }
            else
            {
                return "Classes is not a dynamic object";
            }
        }
        else
        {
            return "Classes is not a map of classes";
        }
    }

    // Process the "controls" field if it exists
    if (jsonObject->hasProperty("controls"))
    {
        auto controls = jsonObject->getProperty("controls");
        if (controls.isObject())
        {
            auto *controlsObj = controls.getDynamicObject();
            if (controlsObj)
            {
                auto res = parseControls(controlsObj);
                if (!res)
                    return res.withExtraError("Unable to parse controls map");
            }
            else
            {
                return "Controls is not a dynamic object";
            }
        }
        else
        {
            return "Controls is not a map of controls";
        }
    }

    for (const auto &[n, cont] : controlMap)
    {
        if (classMap.find(cont.className) == classMap.end())
        {
            return "Unable to find class " + cont.className + " for component " + n;
        }
        host.createBindAndPosition(cont, classMap.at(cont.className));
    }

    return {};
}

JsonLayoutEngine::retval_t JsonLayoutEngine::parseClasses(juce::DynamicObject *classesObj)
{
    for (const auto &prop : classesObj->getProperties())
    {
        auto className = prop.name.toString().toStdString();

        if (prop.value.isObject())
        {
            auto *classObj = prop.value.getDynamicObject();
            if (classObj)
            {
                auto res = parseIndividualClass(classObj, className);
                if (!res)
                    return res.withExtraError("Unable to parse class " + className);
            }
        }
        else
        {
            return "Class " + className + " is not an object";
        }
    }

    return {};
}

JsonLayoutEngine::retval_t JsonLayoutEngine::parseIndividualClass(juce::DynamicObject *classObj,
                                                                  const std::string &className)
{
    json_document::Class c;
    c.name = className;

    // Set of property names that are explicitly handled
    std::set<std::string> handledProperties = {"control-type", "w", "h", "style"};

    // Parse control-type
    if (classObj->hasProperty("control-type"))
    {
        c.controlType = classObj->getProperty("control-type").toString().toStdString();
    }

    // Parse width and height
    if (classObj->hasProperty("w"))
    {
        c.w = static_cast<int>(classObj->getProperty("w"));
    }
    if (classObj->hasProperty("h"))
    {
        c.h = static_cast<int>(classObj->getProperty("h"));
    }

    // Parse style
    if (classObj->hasProperty("style"))
    {
        c.style = classObj->getProperty("style").toString().toStdString();
    }

    // Capture all other string-valued properties in extraKVs
    for (const auto &prop : classObj->getProperties())
    {
        auto propName = prop.name.toString().toStdString();

        // Skip if this property was already handled
        if (handledProperties.find(propName) != handledProperties.end())
            continue;

        // Only add string-valued properties to extraKVs
        if (prop.value.isString())
        {
            c.extraKVs[propName] = prop.value.toString().toStdString();
        }
    }

    classMap[className] = c;
    return {};
}

JsonLayoutEngine::retval_t JsonLayoutEngine::parseControls(juce::DynamicObject *controlsObj)
{
    for (const auto &prop : controlsObj->getProperties())
    {
        auto controlName = prop.name.toString().toStdString();

        if (prop.value.isObject())
        {
            auto *controlObj = prop.value.getDynamicObject();
            if (controlObj)
            {
                auto res = parseSingleControl(controlObj, controlName);
                if (!res)
                    return res.withExtraError("Unable to parse control " + controlName);
            }
        }
        else
        {
            return "Control " + controlName + " is not an object";
        }
    }

    return {};
}

JsonLayoutEngine::retval_t JsonLayoutEngine::parseSingleControl(juce::DynamicObject *controlObj,
                                                                const std::string &cname)
{
    json_document::Control c;
    c.name = cname;
    // Process control class
    if (controlObj->hasProperty("class"))
    {
        auto className = controlObj->getProperty("class").toString().toStdString();
        c.className = className;
    }
    else
    {
        return "Control " + cname + " has no associated class";
    }

    if (classMap.find(c.className) == classMap.end())
    {
        return "Control " + cname + " references unknown class " + c.className;
    }

    if (controlObj->hasProperty("label"))
    {
        c.label = controlObj->getProperty("label").toString().toStdString();
    }

    // Process control binding
    if (controlObj->hasProperty("binding"))
    {
        auto binding = controlObj->getProperty("binding");
        if (binding.isObject())
        {
            auto *bindingObj = binding.getDynamicObject();
            if (bindingObj)
            {
                json_document::Binding b;
                auto res = parseSingleBinding(bindingObj, b);
                if (!res)
                    return res.withExtraError("Unable to parse control " + cname + " binding");
                c.binding = b;
            }
        }
        else
        {
            return "Control " + cname + " binding is not an object";
        }
    }

    // Process control enabled-if
    if (controlObj->hasProperty("enabled-if"))
    {
        auto enabledIf = controlObj->getProperty("enabled-if");
        if (enabledIf.isObject())
        {
            auto *enabledIfObj = enabledIf.getDynamicObject();
            if (enabledIfObj)
            {
                json_document::EnabledIf e;
                auto res = parseSingleEnabledIf(enabledIfObj, e);
                if (!res)
                    return res.withExtraError("Unable to parse control " + cname + " enabled-if");
                c.enabledIf = e;
            }
        }
        else
        {
            return "Control " + cname + " enabled-if is not an object";
        }
    }

    // Process control visible-if
    if (controlObj->hasProperty("visible-if"))
    {
        auto visibleIf = controlObj->getProperty("visible-if");
        if (visibleIf.isObject())
        {
            auto *visibleIfObj = visibleIf.getDynamicObject();
            if (visibleIfObj)
            {
                json_document::VisibleIf v;
                auto res = parseSingleVisibleIf(visibleIfObj, v);
                if (!res)
                    return res.withExtraError("Unable to parse control " + cname + " visible-if");
                c.visibleIf = v;
            }
        }
        else
        {
            return "Control " + cname + " visible-if is not an object";
        }
    }

    // Process control position
    if (controlObj->hasProperty("position"))
    {
        auto position = controlObj->getProperty("position");
        if (position.isObject())
        {
            auto *posObj = position.getDynamicObject();
            if (posObj)
            {
                auto get = [posObj](const std::string &n, int &v) {
                    if (posObj->hasProperty(n.c_str()))
                        v = static_cast<int>(posObj->getProperty(n.c_str()));
                };
                get("x", c.position.x);
                get("y", c.position.y);
                get("w", c.position.w);
                get("h", c.position.h);
                if (c.position.w < 0)
                {
                    c.position.w = classMap.at(c.className).w;
                }
                if (c.position.h < 0)
                {
                    c.position.h = classMap.at(c.className).h;
                }
            }
        }
        else
        {
            return "Control " + cname + " position is not an object";
        }
    }
    else
    {
        return "Control " + cname + " must have a position";
    }

    controlMap[cname] = c;
    return {};
}

JsonLayoutEngine::retval_t JsonLayoutEngine::parseSingleBinding(juce::DynamicObject *bindingObj,
                                                                json_document::Binding &binding)
{
    if (bindingObj->hasProperty("index"))
    {
        auto index = bindingObj->getProperty("index");
        binding.index = index;
    }
    if (bindingObj->hasProperty("type"))
    {
        auto type = bindingObj->getProperty("type").toString().toStdString();
        binding.type = type;
    }
    return {};
}

JsonLayoutEngine::retval_t
JsonLayoutEngine::parseSingleEnabledIf(juce::DynamicObject *enabledIfObj,
                                       json_document::EnabledIf &enabledIf)
{
    if (enabledIfObj->hasProperty("index"))
    {
        auto index = enabledIfObj->getProperty("index");
        enabledIf.index = index;
    }
    if (enabledIfObj->hasProperty("type"))
    {
        auto type = enabledIfObj->getProperty("type").toString().toStdString();
        enabledIf.type = type;
    }
    if (enabledIfObj->hasProperty("values"))
    {
        auto values = enabledIfObj->getProperty("values");
        if (values.isArray())
        {
            auto *valuesArray = values.getArray();
            if (valuesArray)
            {
                for (int i = 0; i < valuesArray->size(); ++i)
                {
                    auto val = valuesArray->getUnchecked(i);
                    enabledIf.values.push_back(static_cast<int>(val));
                }
            }
        }
    }
    return {};
}

JsonLayoutEngine::retval_t
JsonLayoutEngine::parseSingleVisibleIf(juce::DynamicObject *visibleIfObj,
                                       json_document::VisibleIf &visibleIf)
{
    if (visibleIfObj->hasProperty("index"))
    {
        auto index = visibleIfObj->getProperty("index");
        visibleIf.index = index;
    }
    if (visibleIfObj->hasProperty("type"))
    {
        auto type = visibleIfObj->getProperty("type").toString().toStdString();
        visibleIf.type = type;
    }
    if (visibleIfObj->hasProperty("values"))
    {
        auto values = visibleIfObj->getProperty("values");
        if (values.isArray())
        {
            auto *valuesArray = values.getArray();
            if (valuesArray)
            {
                for (int i = 0; i < valuesArray->size(); ++i)
                {
                    auto val = valuesArray->getUnchecked(i);
                    visibleIf.values.push_back(static_cast<int>(val));
                }
            }
        }
    }
    return {};
}

} // namespace sst::jucegui::layouts