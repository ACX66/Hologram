#include "Global.h"
#include "FloatingText.h"
#include <llapi/mc/Dimension.hpp>
#include <llapi/mc/CompoundTag.hpp>
#include <llapi/HookAPI.h>

class ChangeDimensionRequest {
public:
    int mState;
    AutomaticID<Dimension, int> mFromDimensionId;
    AutomaticID<Dimension, int> mToDimensionId;
    Vec3 mPosition;
    bool mUsePortal;
    bool mRespawn;
    std::unique_ptr<CompoundTag> mAgentTag;
};

TClasslessInstanceHook(bool,
                       "?requestPlayerChangeDimension@Level@@UEAAXAEAVPlayer@@V?$unique_ptr@VChangeDimensionRequest@@U?$default_delete@VChangeDimensionRequest@@@std@@@std@@@Z",
                       Player *sp, std::unique_ptr<ChangeDimensionRequest> request) {

    if (request->mToDimensionId == sp->getDimensionId())
        return original(this, sp, std::move(request));

    auto res = original(this, sp, std::move(request));

    for (auto &text: StaticTexts)
        text.second.create(sp);

    for (auto &text: DynamicTexts)
        text.second.create(sp);

    return res;
}


void deleteText(bool isDynamic, string &name) {
    string fileName = path + "data.json";
    std::ofstream file(fileName);
    if (!file.is_open()) {
        logger.warn("Can't open data.json file(" + fileName + ")");
        return;
    }

    if (isDynamic) {
        DynamicTexts.erase(name);
        JsonData.at("data").at("dynamic").erase(name);
    } else {
        StaticTexts.erase(name);
        JsonData.at("data").at("static").erase(name);
    }

    file << JsonData.dump(4);
    file.close();
}

void addText(FloatingText &text, bool isDynamic, string &name) {
    string fileName = path + "data.json";
    std::ofstream file(fileName);
    if (!file.is_open()) {
        logger.warn("Can't open data.json file(" + fileName + ")");
        return;
    }

    if (isDynamic) {
        DynamicTexts.emplace(name, text);
        JsonData["data"]["dynamic"][name] = text.exportAsJson();
    } else {
        StaticTexts.emplace(name, text);
        JsonData["data"]["static"][name] = text.exportAsJson();
    }

    file << JsonData.dump(4);
    file.close();
}