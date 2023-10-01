#pragma once

#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>

#define TRJ(key,val)                                         \
if (json.find(key) != json.end()) {                          \
    const nlohmann::json& out = json.at(key);                \
    out.get_to(val);}                                         \

namespace TR {
    extern string addFloatingText;
    extern string deleteFloatingText;
    extern string TextID;
    extern string enterTextID;
    extern string StaticText;
    extern string DynamicText;
    extern string Overworld;
    extern string Nether;
    extern string TheEnd;
    extern string Dimension;
    extern string Coordinate;
    extern string Texttype;
    extern string TextContents;
    extern string enterTextContents;
    extern string OperationCanceled;
    extern string InformationEnterError;
    extern string TextIDRepeated;
    extern string deleteStaticText;
    extern string deleteDynamicText;
    extern string deleteSuccessful;
    extern string addSuccessful;
    extern string deleteConfirm;
    extern string deleteCancel;
    extern string textInfo;
    extern string enterCoordinates;

    nlohmann::json globaljson();
    void initjson(nlohmann::json json);
    void WriteDefaultConfig(const std::string& fileName);
    void LoadConfigFromJson(const std::string& fileName);
}

class FloatingText;

extern Logger logger;
extern int defaultTick;
extern bool logError;
extern nlohmann::json JsonData;
extern std::unordered_map<string, FloatingText> StaticTexts;
extern std::unordered_map<string, FloatingText> DynamicTexts;
extern std::function<std::string(std::string const &str, std::string const &xuid)> translateString;

void RegCommand();

void WriteDefaultData(const std::string &fileName);

void readJson(const std::string &fileName);

void loadData(const std::string &fileName);

void deleteText(bool isDynamic, string &name);

void addText(FloatingText &text, bool isDynamic, string &name);

string m_replace(string strSrc, const string &oldStr, const string &newStr);

unsigned long long getRandomId();

const std::string path = "plugins/Hologram/";