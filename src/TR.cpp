#pragma once

#include "Global.h"

string TR::addFloatingText = "§l§1添加悬浮字";
string TR::deleteFloatingText = "§l§1删除悬浮字";
string TR::TextID = "§l§a悬浮字名称";
string TR::enterTextID = "输入悬浮字名（不可重复）";
string TR::StaticText = "静态悬浮字";
string TR::DynamicText = "动态悬浮字";
string TR::Overworld = "主世界";
string TR::Nether = "地狱";
string TR::TheEnd = "末地";
string TR::Dimension = "§l§a显示维度";
string TR::Coordinate = "§l§a坐标";
string TR::Texttype = "§l§a悬浮字种类";
string TR::TextContents = "§l§a悬浮字内容";
string TR::enterTextContents = "输入悬浮字内容（使用\\n换行）";
string TR::OperationCanceled = "§l[§bHologram§r§l] §l§6您选择了取消";
string TR::InformationEnterError = "§l§c信息输入错误";
string TR::TextIDRepeated = "§l§6悬浮字名称重复";
string TR::deleteStaticText = "删除静态悬浮字";
string TR::deleteDynamicText = "删除动态悬浮字";
string TR::deleteSuccessful = "§l[§bHologram§r§l] §l§a删除成功";
string TR::addSuccessful = "§l[§bHologram§r§l] §l§a添加成功";
string TR::deleteConfirm = "确认删除";
string TR::deleteCancel = "取消删除";
string TR::textInfo = "§l§1悬浮字信息";
string TR::enterCoordinates = "输入悬浮字坐标";

nlohmann::json TR::globaljson() {
    nlohmann::json json;
    json["add.floating.text"] = addFloatingText;
    json["delete.floating.text"] = deleteFloatingText;
    json["text.ID"] = TextID;
    json["enter.text.ID"] = enterTextID;
    json["text.contents"] = TextContents;
    json["enter.text.contents"] = enterTextContents;
    json["static.text"] = StaticText;
    json["dynamic.text"] = DynamicText;
    json["Overworld"] = Overworld;
    json["Nether"] = Nether;
    json["TheEnd"] = TheEnd;
    json["Dimension"] = Dimension;
    json["Coordinate"] = Coordinate;
    json["enter.coordinates"] = enterCoordinates;
    json["text.type"] = Texttype;
    json["operation.canceled"] = OperationCanceled;
    json["information.enter.error"] = InformationEnterError;
    json["text.ID.repeated"] = TextIDRepeated;
    json["delete.static.text"] = deleteStaticText;
    json["delete.dynamic.text"] = deleteDynamicText;
    json["delete.successful"] = deleteSuccessful;
    json["add.successful"] = addSuccessful;
    json["delete.confirm"] = deleteConfirm;
    json["delete.cancel"] = deleteCancel;
    json["text.info"] = textInfo;
    return json;
}

void TR::initjson(nlohmann::json json) {
    TRJ("add.floating.text", addFloatingText);
    TRJ("delete.floating.text", deleteFloatingText);
    TRJ("text.ID", TextID);
    TRJ("enter.text.ID", enterTextID);
    TRJ("text.contents", TextContents);
    TRJ("enter.text.contents", enterTextContents);
    TRJ("static.text", StaticText);
    TRJ("dynamic.text", DynamicText);
    TRJ("Overworld", Overworld);
    TRJ("Nether", Nether);
    TRJ("TheEnd", TheEnd);
    TRJ("Dimension", Dimension);
    TRJ("Coordinate", Coordinate);
    TRJ("enter.coordinates", enterCoordinates);
    TRJ("text.type", Texttype);
    TRJ("operation.canceled", OperationCanceled);
    TRJ("information.enter.error", InformationEnterError);
    TRJ("text.ID.repeated", TextIDRepeated);
    TRJ("delete.static.text", deleteStaticText);
    TRJ("delete.dynamic.text", deleteDynamicText);
    TRJ("delete.successful", deleteSuccessful);
    TRJ("add.successful", addSuccessful);
    TRJ("delete.confirm", deleteConfirm);
    TRJ("delete.cancel", deleteCancel);
    TRJ("text.info", textInfo);
}

void TR::WriteDefaultConfig(const std::string &fileName) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Can't open file " << fileName << std::endl;
        return;
    }
    auto json = globaljson();
    file << json.dump(4);
    file.close();
}

void TR::LoadConfigFromJson(const std::string &fileName) {
    std::ifstream file(fileName);
    if (std::filesystem::exists(fileName)) {
        try {
            nlohmann::json json;
            file >> json;
            file.close();
            initjson(json);
        }
        catch (std::exception &e) {
            logger.error("Lang File isInvalid, Err {}", e.what());
        }
        catch (...) {
            logger.error("Lang File isInvalid");
        }
    } else {
        logger.info("Lang File with default values created");
        WriteDefaultConfig(fileName);
    }
}

