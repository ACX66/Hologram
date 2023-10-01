#include "Global.h"
#include "FloatingText.h"
#include <llapi/ScheduleAPI.h>
#include <llapi/RemoteCallAPI.h>

nlohmann::json JsonData;
int defaultTick = 20;
bool logError = true;
std::unordered_map<string, FloatingText> StaticTexts;
std::unordered_map<string, FloatingText> DynamicTexts;
std::function<std::string(std::string const &str, std::string const &xuid)> translateString;

void WriteDefaultData(const std::string &fileName) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        logger.warn("Can't open data.json file(" + fileName + ")");
        return;
    }

    JsonData["updateTick"] = defaultTick;
    JsonData["logError"] = logError;
    file << JsonData.dump(4);
    file.close();
}

void readJson(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        logger.warn("Can't open data.json file(" + fileName + ")");
        return;
    }
    file >> JsonData;
    file.close();

    if (JsonData.find("updateTick") != JsonData.end()) {
        defaultTick = JsonData["updateTick"];
    } else {
        JsonData["updateTick"] = defaultTick;
        std::ofstream ofile(fileName);
        ofile << JsonData.dump(4);
        ofile.close();
    }

    if (JsonData.find("logError") != JsonData.end()) {
        logError = JsonData["logError"];
    } else {
        JsonData["logError"] = logError;
        std::ofstream ofile(fileName);
        ofile << JsonData.dump(4);
        ofile.close();
    }

    if (JsonData.find("data") != JsonData.end()) {
        if (JsonData.at("data").find("static") != JsonData.at("data").end()) {
            for (auto &it: JsonData["data"]["static"].items()) {
                StaticTexts.emplace(it.key(), FloatingText(it.value(), false));
            }
        }

        if (JsonData.at("data").find("dynamic") != JsonData.at("data").end()) {
            for (auto &it: JsonData["data"]["dynamic"].items()) {
                DynamicTexts.emplace(it.key(), FloatingText(it.value(), true));
            }
        }
    }
}

void loadData(const std::string &fileName) {
    //data
    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);
    //tr
    if (std::filesystem::exists(fileName)) {
        try {
            readJson(fileName);
        }
        catch (std::exception &e) {
            logger.error("Data File isInvalid, Err {}", e.what());
        }
        catch (...) {
            logger.error("Data File isInvalid");
        }
    } else {
        logger.info("Data File with default values created");
        WriteDefaultData(fileName);
    }
}

string m_replace(string strSrc, const string &oldStr, const string &newStr) {
    size_t pos = 0;
    int l_count = 0;
    unsigned int count = strSrc.size();
    while ((pos = strSrc.find(oldStr, pos)) != string::npos) {
        strSrc.replace(pos, oldStr.size(), newStr);
        if (++l_count >= count) break;
        pos += newStr.size();
    }
    return strSrc;
}

unsigned long long getRandomId() {
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<unsigned long long> distribution(std::numeric_limits<unsigned long long>::min(),
                                                                   std::numeric_limits<long long>::max());
    return distribution(generator);
}

void PluginInit() {
    RegCommand();

    loadData(path + "data.json");
    TR::LoadConfigFromJson(path + "lang.json");

    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) {
        if (ll::getPlugin("BEPlaceholderAPI")) {
            Schedule::repeat([]() {
                for (auto &text: DynamicTexts)
                    text.second.update();
            }, defaultTick);

            translateString = RemoteCall::importAs<std::string(std::string const &str, std::string const &xuid)>(
                    "BEPlaceholderAPI", "translateString");
        } else
            logger.warn("BEPlaceholderAPI not found! Dynamic Floating Texts will not work");

        return true;
    });

    Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent &ev) {
        for (auto &text: StaticTexts)
            text.second.create(ev.mPlayer);
        for (auto &text: DynamicTexts)
            text.second.create(ev.mPlayer);

        return true;
    });

    logger.info("Hologram load successful!");
    logger.info("Made by CanXue");
}
