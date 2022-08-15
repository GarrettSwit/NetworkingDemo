#pragma once
enum PacketHeaderTypes {
	PHT_Invalid = 0,
	PHT_IsDead,
	PHT_Position,
	PHT_Count
};

struct GamePacket {
	GamePacket( ) {}
	PacketHeaderTypes Type = PHT_Invalid;
};

struct PositionPacket : public GamePacket {
	PositionPacket() {
		Type = PHT_Position;
	}
	int playerId = 0;
	int x = 0;
	int y = 0;
};


struct IsDeadPacket : public GamePacket
{
	IsDeadPacket() {
		Type = PHT_IsDead;
	}

	bool IsDead = false;
};