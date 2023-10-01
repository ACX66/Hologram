#pragma once

#include "Global.h"
#include <llapi/utils/Bstream.h>
#include <llapi/mc/Vec3.hpp>
#include <llapi/mc/BinaryStream.hpp>
#include <llapi/mc/MinecraftPackets.hpp>
#include <llapi/SendPacketAPI.h>


class FloatingText {
public:
    unsigned long long runtimeID = 0;
    string text;
    Vec3 pos;
    int dim = 0;
    bool isDynamic = false;

    FloatingText(const nlohmann::json &obj, bool dynamic) : isDynamic(dynamic) {
        obj.at("text").get_to(this->text);
        obj.at("x").get_to(this->pos.x);
        obj.at("y").get_to(this->pos.y);
        obj.at("z").get_to(this->pos.z);

        if (obj.find("dim") == obj.end()) this->dim = 0;
        else obj.at("dim").get_to(this->dim);

        this->runtimeID = getRandomId();
    }

    FloatingText(string &text, Vec3 &pos, int dim, bool dynamic) : text(text), pos(pos), dim(dim),
                                                                   isDynamic(dynamic) {
        this->runtimeID = getRandomId();
    }

    void create(Player *player) const {
        if (player->isSimulatedPlayer())
            return;

        string entityType = "minecraft:chicken";

        if (dim == player->getDimensionId()) {
            std::vector<std::unique_ptr<DataItem>> mDataItems;
            try {
                mDataItems.emplace_back(DataItem::create(ActorDataIDs::NAMETAG,
                                                         isDynamic && translateString ? translateString(text, player->getXuid()) : text));

                mDataItems.emplace_back(DataItem::create(ActorDataIDs::SCALE, 0.001f));
                mDataItems.emplace_back(DataItem::create<signed char>(ActorDataIDs::ALWAYS_SHOW_NAMETAG, true));
            } catch (...) {
                if(logError) {
                    logger.error("Fail to convert text: " + text);
                    logger.error("Player affected: " + player->getRealName());
                    logger.error("Please check your texts");
                }
                return;
            }

            BinaryStream bs;
            bs.writeVarInt64(runtimeID);
            bs.writeUnsignedVarInt64(runtimeID);
            bs.writeString(entityType);
            bs.writeType(pos);
            bs.writeType(Vec3{0, 0, 0});
            bs.writeType(Vec3{0, 0, 0});
            bs.writeFloat(0.0);
            bs.writeUnsignedVarInt(0);
            bs.writeType(mDataItems);
            bs.writeUnsignedVarInt(0);
            bs.writeUnsignedVarInt(0);
            bs.writeUnsignedVarInt(0);

            NetworkPacket<13> pkt(bs.getAndReleaseData());
            player->sendNetworkPacket(pkt);
        }
    }

    void createForAllPlayers() const {
        for (auto &player: Level::getAllPlayers()) {
            this->create(player);
        }
    }

    void remove() const {
        BinaryStream bs;
        bs.writeUnsignedVarInt64(ZigZag(ActorUniqueID(runtimeID)));
        auto pkt = MinecraftPackets::createPacket(MinecraftPacketIds::RemoveActor);
        pkt->read(bs);
        Level::sendPacketForAllPlayers(*pkt);
    }

    void update() const {
        if (isDynamic && translateString) {
            for (auto &player: Level::getAllPlayers()) {
                if (!player->isSimulatedPlayer()) {
                    std::vector<std::unique_ptr<DataItem>> mDataItems;
                    try {
                        mDataItems.emplace_back(
                                DataItem::create(ActorDataIDs::NAMETAG, translateString(text, player->getXuid())));
                    } catch (...) {
                        if(logError) {
                            logger.error("Fail to convert text: " + text);
                            logger.error("Player affected: " + player->getRealName());
                            logger.error("Please check your texts");
                        }
                        continue;
                    }

                    BinaryStream bs;
                    bs.writeUnsignedVarInt64(runtimeID);
                    bs.writeType(mDataItems);
                    bs.writeUnsignedVarInt(0);
                    bs.writeUnsignedVarInt(0);
                    bs.writeUnsignedVarInt64(0);

                    NetworkPacket<39> pkt(bs.getAndReleaseData());
                    player->sendNetworkPacket(pkt);
                }
            }
        }
    }

    nlohmann::json exportAsJson() {
        nlohmann::json res;
        res["text"] = text;
        res["x"] = pos.x;
        res["y"] = pos.y;
        res["z"] = pos.z;
        res["dim"] = dim;
        return res;
    }
};