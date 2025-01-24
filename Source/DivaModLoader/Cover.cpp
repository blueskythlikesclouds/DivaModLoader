#include "Cover.h"

#include "ModLoader.h"
#include "Context.h"
#include "Utilities.h"
#include "Types.h"

static FUNCTION_PTR(int, __fastcall, getLanguage, 0x1402C8D20);

struct PvDbAnotherSong {
    prj::string name;
    prj::string file;
    prj::string vocalDispName;
    int vocalCharaNum;

    PvDbAnotherSong() : name(), file(), vocalDispName(), vocalCharaNum() {
        this->vocalCharaNum = 10;
    }

    PvDbAnotherSong(prj::string name, prj::string file, prj::string vocalDispName, int vocalCharaNum) {
        this->name = name;
        this->file = file;
        this->vocalDispName = vocalDispName;
        this->vocalCharaNum = vocalCharaNum;
    }
};

struct PvDbExSong {
    int chara[6];
    prj::string file;
    prj::string name;
    INSERT_PADDING(0x16);

    PvDbExSong(int chara, prj::string file) : name() {
        this->chara[0] = chara;
        this->file = file;
    }
};

struct PvDbPerfomer {
    int type;
    int chara;
    INSERT_PADDING(0x3C);
};

struct PvDbEntry {
    int id;
    int date;
    prj::string name;
    INSERT_PADDING(0x70);
    prj::vector<PvDbPerfomer> performers;
    INSERT_PADDING(0x78);
    int exSongCount;
    prj::vector<PvDbExSong> exSong;
    INSERT_PADDING(0x140);
    prj::vector<PvDbAnotherSong> anotherSong;
    INSERT_PADDING(0x13C);

    ~PvDbEntry() = delete;
};

std::map<int, std::vector<PvDbAnotherSong>> pendingAnotherSong;
std::map<int, std::vector<PvDbExSong>> pendingExSong;

int chara_index_from_name(std::string name) {
    if (name == "MIKU") {
        return 0;
    }
    else if (name == "RIN") {
        return 1;
    }
    else if (name == "LEN") {
        return 2;
    }
    else if (name == "LUKA") {
        return 3;
    }
    else if (name == "NERU") {
        return 4;
    }
    else if (name == "HAKU") {
        return 5;
    }
    else if (name == "KAITO") {
        return 6;
    }
    else if (name == "MEIKO") {
        return 7;
    }
    else if (name == "SAKINE") {
        return 8;
    }
    else if (name == "TETO") {
        return 9;
    }
    else {
        throw std::exception(name.c_str());
    }
}

HOOK(bool, __fastcall, PvDbCtrl, 0x1404BB290, uint64_t a1) {
    auto res = originalPvDbCtrl(a1);
    if (*(int*)(a1 + 0x68) == 0 && (pendingAnotherSong.size() > 0 || pendingExSong.size() > 0)) {
        auto pvs = (prj::vector<PvDbEntry *> *)(a1 + 0x98);
        for (size_t i = 0; i < pvs->size(); i++) {
            auto pv = pvs->at(i);
            if (pendingAnotherSong.find(pv->id) != pendingAnotherSong.end()) {
                if (pv->anotherSong.size() == 0) {
                    pv->anotherSong.push_back(PvDbAnotherSong(pv->name, prj::string(), prj::string(), pv->performers[0].chara));
                }

                for (auto& anotherSong : pendingAnotherSong[pv->id]) {
                    pv->anotherSong.push_back(anotherSong);
                }

                pendingAnotherSong.erase(pv->id);
            }

            if (pendingExSong.find(pv->id) != pendingExSong.end()) {
                for (auto& exSong : pendingExSong[pv->id]) {
                    exSong.name = pv->name;
                    pv->exSong.push_back(exSong);
                    pv->exSongCount++;
                }

                pendingExSong.erase(pv->id);
            }
        }
    }

    return res;
}

void Cover::postInit() {
	for (auto& dir : ModLoader::modDirectoryPaths) {
		const std::string dbFilePath = dir + "\\cover_db.toml";

        toml::table coverDb;

        try
        {
            coverDb = toml::parse_file(dbFilePath);
        }
        catch (...) {
            continue;
        }

        try {
            if (toml::array* arr = coverDb["another_song"].as_array()) {
                for (auto& elem : *arr) {
                    toml::table anotherSong = *elem.as_table();
                    const auto pv = anotherSong["pv"].value<int>();
                    auto name = anotherSong["name_en"].value<std::string>();
                    auto vocalDispName = anotherSong["vocal_disp_name_en"].value<std::string>();
                    const auto vocalChara = anotherSong["vocal_chara"].value<std::string>();
                    const auto file = anotherSong["file"].value<std::string>();

                    if (getLanguage() == 0 || !name.has_value()) name = anotherSong["name"].value<std::string>();
                    if (getLanguage() == 0 || !vocalDispName.has_value()) vocalDispName = anotherSong["vocal_disp_name"].value<std::string>();

                    if (!pv.has_value() || !name.has_value() || (!vocalDispName.has_value() && !vocalChara.has_value()) || !file.has_value()) {
                        LOG(" - Failed to load \"%s\": incomplete another_song entry", getRelativePath(dbFilePath).c_str());
                        continue;
                    }

                    PvDbAnotherSong newPending;
                    newPending.name = prj::string(*name);
                    newPending.file = prj::string(*file);
                    if (vocalDispName.has_value()) {
                        newPending.vocalDispName = prj::string(*vocalDispName);
                    }
                    if (vocalChara.has_value()) {
                        newPending.vocalCharaNum = chara_index_from_name(*vocalChara);
                    }

                    if (pendingAnotherSong.find(*pv) != pendingAnotherSong.end()) {
                        auto& pending = pendingAnotherSong[*pv];
                        pending.push_back(newPending);
                    }
                    else {
                        pendingAnotherSong[*pv] = { newPending };
                    }
                }
            }

            if (toml::array* arr = coverDb["ex_song"].as_array()) {
                for (auto& elem : *arr) {
                    toml::table exSong = *elem.as_table();
                    const auto pv = exSong["pv"].value<int>();
                    const auto file = exSong["file"].value<std::string>();
                    const auto chara = exSong["chara"].value<std::string>();

                    if (!pv.has_value() || !chara.has_value() || !file.has_value()) {
                        LOG(" - Failed to load \"%s\": incomplete ex_song entry", getRelativePath(dbFilePath).c_str());
                        continue;
                    }

                    PvDbExSong newPending(chara_index_from_name(*chara), prj::string(*file));

                    if (pendingExSong.find(*pv) != pendingExSong.end()) {
                        auto& pending = pendingExSong[*pv];
                        pending.push_back(newPending);
                    }
                    else {
                        pendingExSong[*pv] = { newPending };
                    }
                }
            }
        }
        catch (...)
        {
            LOG(" - Failed to load \"%s\"", getRelativePath(dbFilePath).c_str());
            continue;
        }
	}

    if (pendingAnotherSong.size() > 0 || pendingExSong.size() > 0) INSTALL_HOOK(PvDbCtrl);
}