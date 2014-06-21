#pragma once

#include "stdafx.h"

enum class MirroringType
{
	Horizontal,
	Vertical,
	FourScreens,
};

struct NESHeader
{
	char NES[4];
	uint8_t ROMCount;
	uint8_t VROMCount;
	uint8_t Flags1;
	uint8_t Flags2;
	uint8_t RAMCount;
	uint8_t CartType;
	uint8_t Reserved[6];

	uint8_t GetMapperID()
	{
		return (Flags2 & 0xF0) | (Flags1 >> 4);
	}

	MirroringType GetMirroringType()
	{
		if(Flags1 & 0x08) {
			return MirroringType::FourScreens;
		} else {
			return Flags1 & 0x01 ? MirroringType::Vertical : MirroringType::Horizontal;
		}
	}
};

typedef vector<uint8_t> MemoryBank;
class ROMLoader
{
	private:
		NESHeader _header;
		vector<MemoryBank> _romBanks;
		vector<MemoryBank> _vromBanks;

	public:
		const static int ROMBankSize = 0x4000;
		const static int VROMBankSize = 0x2000;

		ROMLoader(wstring filename)
		{
			_romBanks.clear();
			_vromBanks.clear();

			ifstream romFile(filename, ios::in | ios::binary);

			if(!romFile) {
				throw std::exception("File could not be read");
			}

			romFile.read((char*)&_header, sizeof(NESHeader));

			uint8_t *buffer = new uint8_t[max(ROMBankSize, VROMBankSize)];
			for(int i = 0; i < _header.ROMCount; i++) {
				romFile.read((char*)buffer, ROMBankSize);
				_romBanks.push_back(MemoryBank(buffer, buffer + ROMBankSize));
			}

			for(int i = 0; i < _header.VROMCount; i++) {
				romFile.read((char*)buffer, VROMBankSize);
				_vromBanks.push_back(MemoryBank(buffer, buffer + VROMBankSize));
			}
			
			delete[] buffer;

			romFile.close();
		}

		vector<MemoryBank> GetROMBanks()
		{
			return _romBanks;
		}

		vector<MemoryBank> GetVROMBanks()
		{
			return _vromBanks;
		}

		NESHeader GetHeader()
		{
			return _header;
		}
};

