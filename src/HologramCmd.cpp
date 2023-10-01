#include "Global.h"
#include "FloatingText.h"
#include <llapi/DynamicCommandAPI.h>
#include <llapi/FormUI.h>

vector<string> aux;

void sendDeleteConfirm(Player *pl, int option, bool isDynamic) {
    auto target = isDynamic ? DynamicTexts.at(aux[option]) : StaticTexts.at(aux[option]);

    auto ConfirmForm = Form::SimpleForm(TR::textInfo,
                                        TR::TextID + ": " + aux[option] + "\n" + TR::Coordinate + ": \nX:" +
                                        std::to_string(target.pos.x) + " \nY:" + std::to_string(target.pos.y) +
                                        " \nZ:" + std::to_string(target.pos.z) +
                                        "\n" + TR::TextContents + ": " + target.text + "\n");

    ConfirmForm.append(Form::Button(TR::deleteConfirm));
    ConfirmForm.append(Form::Button(TR::deleteCancel));
    ConfirmForm.sendTo(pl, [target, isDynamic, option](Player *pl, int op) {
        if (op != 0) {
            pl->sendText(TR::OperationCanceled);
            return;
        }

        target.remove();
        deleteText(isDynamic, aux[option]);
        pl->sendText(TR::deleteSuccessful);
    });
}

void RegCommand() {
    using ParamType = DynamicCommand::ParameterType;

    auto command =
            DynamicCommand::createCommand("hologram", "§l§bHologram gui", CommandPermissionLevel::GameMasters);

    auto &Option = command->setEnum("gui/reload", {"gui", "reload"});

    command->mandatory("Option", ParamType::Enum, Option);
    command->addOverload({Option});

    command->setCallback([](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                            std::unordered_map<std::string, DynamicCommand::Result> &results) {
        if (results["Option"].isSet) {
            auto action = results["Option"].get<std::string>();
            switch (do_hash(action.c_str())) {
                case do_hash("reload"): {
                    for (auto &it: StaticTexts)
                        it.second.remove();
                    for (auto &it: DynamicTexts)
                        it.second.remove();

                    DynamicTexts.clear();
                    StaticTexts.clear();

                    loadData(path + "data.json");
                    TR::LoadConfigFromJson(path + "lang.json");

                    for (auto &text: StaticTexts)
                        text.second.createForAllPlayers();
                    for (auto &text: DynamicTexts)
                        text.second.createForAllPlayers();

                    output.success("Reload success");
                    break;
                }
                case do_hash("gui"): {
                    ServerPlayer *player = origin.getPlayer();
                    if (player->isPlayer()) {
                        auto MainForm = Form::SimpleForm("§l§1Hologram", "");
                        MainForm.append(Form::Button(TR::addFloatingText));
                        MainForm.append(Form::Button(TR::deleteFloatingText));
                        MainForm.sendTo((Player *) player, [](Player *pl, int option) {
                            if (option == 0) {
                                auto CreateTextForm = Form::CustomForm(TR::addFloatingText);
                                CreateTextForm.append(Form::Input("Name", TR::TextID, TR::enterTextID));

                                vector<string> types = {TR::StaticText};
                                if (translateString) types.push_back(TR::DynamicText);

                                vector<string> local = {TR::Overworld, TR::Nether, TR::TheEnd};
                                CreateTextForm.append(Form::Input("Contents", TR::TextContents, TR::enterTextContents));
                                CreateTextForm.append(Form::Dropdown("Type", TR::Texttype, types));
                                CreateTextForm.append(
                                        Form::Dropdown("Dim", TR::Dimension, local, pl->getDimensionId()));
                                CreateTextForm.append(
                                        Form::Input("PosX", TR::Coordinate + " X", TR::enterCoordinates + " X",
                                                    std::to_string((int)pl->getPos().x)));
                                CreateTextForm.append(
                                        Form::Input("PosY", TR::Coordinate + " Y", TR::enterCoordinates + " Y",
                                                    std::to_string((int)pl->getPos().y)));
                                CreateTextForm.append(
                                        Form::Input("PosZ", TR::Coordinate + " Z", TR::enterCoordinates + " Z",
                                                    std::to_string((int)pl->getPos().z)));
                                CreateTextForm.sendTo(pl, [](Player *pl,
                                                             std::map<string, std::shared_ptr<Form::CustomFormElement>> mp) {
                                    if (mp.empty()) {
                                        pl->sendText(TR::OperationCanceled);
                                        return;
                                    }

                                    bool isDynamic = mp["Type"]->getString() == TR::DynamicText;

                                    if (isDynamic ? !DynamicTexts.contains(mp["Name"]->getString())
                                                  : !StaticTexts.contains(mp["Name"]->getString())) {
                                        try {
                                            int dim = 0;
                                            if (mp["Dim"]->getString() == TR::Nether) dim = 1;
                                            else if (mp["Dim"]->getString() == TR::TheEnd) dim = 2;
                                            string str = m_replace(mp["Contents"]->getString(), "\\n", "\n");
                                            string name = mp["Name"]->getString();
                                            Vec3 pos(mp["PosX"]->getFloat(), mp["PosY"]->getFloat(),
                                                     mp["PosZ"]->getFloat());
                                            FloatingText text(str, pos, dim, isDynamic);
                                            addText(text, isDynamic, name);
                                            text.createForAllPlayers();
                                        }
                                        catch (...) {
                                            pl->sendText(TR::InformationEnterError);
                                            return;
                                        }
                                        pl->sendText(TR::addSuccessful);
                                    } else pl->sendText(TR::TextIDRepeated);
                                });
                            } else if (option == 1) {
                                auto DeleteTextForm = Form::SimpleForm(TR::deleteFloatingText, "");
                                DeleteTextForm.append(Form::Button(TR::deleteStaticText));
                                DeleteTextForm.append(Form::Button(TR::deleteDynamicText));
                                DeleteTextForm.sendTo(pl, [](Player *pl, int option) {
                                    if (option == -1) {
                                        pl->sendText(TR::OperationCanceled);
                                        return;
                                    }

                                    aux.clear();

                                    if (option == 0) {
                                        auto DeleteStaticTextForm = Form::SimpleForm(TR::deleteFloatingText, "");
                                        for (auto &it: JsonData["data"]["static"].items()) {
                                            DeleteStaticTextForm.append(Form::Button(it.key()));
                                            aux.push_back(it.key());
                                        }

                                        DeleteStaticTextForm.sendTo(pl, [](Player *pl, int option) {
                                            if (option != -1)
                                                return sendDeleteConfirm(pl, option, false);

                                            pl->sendText(TR::OperationCanceled);
                                        });
                                    } else if (option == 1) {
                                        auto DeleteDynamicTextForm = Form::SimpleForm(TR::deleteFloatingText, "");
                                        for (auto &it: JsonData["data"]["dynamic"].items()) {
                                            DeleteDynamicTextForm.append(Form::Button(it.key()));
                                            aux.push_back(it.key());
                                        }

                                        DeleteDynamicTextForm.sendTo(pl, [](Player *pl, int option) {
                                            if (option != -1)
                                                return sendDeleteConfirm(pl, option, true);

                                            pl->sendText(TR::OperationCanceled);
                                        });
                                    }
                                });
                            }
                        });
                    }
                }
            }
        } else
            output.error("Unknown command");
    });
    DynamicCommand::setup(std::move(command));
}