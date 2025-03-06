#include "Cover.h"

#include "ModLoader.h"
#include "Context.h"
#include "Utilities.h"
#include "Types.h"
#include "SigScan.h"

constexpr char MAGIC = 0x02;

SIG_SCAN(sigGetLanguage, 0x1402A2FFB, "\xE8\xCC\xCC\xCC\xCC\x83\xE8\x01\x74\x7F", "x????xxxxx");
static FUNCTION_PTR(int, __fastcall, getLanguage, readInstrPtr(sigGetLanguage(), 0, 5));

struct AnotherSongEntry {
    std::string name;
    std::string name_en;
    std::string vocal_disp_name;
    std::string vocal_disp_name_en;
    std::string file;
    int vocal_chara_num;

    AnotherSongEntry() = default;
};

struct PvDbAnotherSong {
    prj::string name;
    prj::string file;
    prj::string vocalDispName;
    int vocalCharaNum;

    PvDbAnotherSong(prj::string name, prj::string file, prj::string vocalDispName, int vocalCharaNum) {
        this->name = name;
        this->file = file;
        this->vocalDispName = vocalDispName;
        this->vocalCharaNum = vocalCharaNum;
    }

    PvDbAnotherSong(AnotherSongEntry entry) {
        if (getLanguage() == 0 || entry.name_en.length() == 0) this->name = entry.name;
        else this->name = entry.name_en;
        if (getLanguage() == 0 || entry.vocal_disp_name_en.length() == 0) this->vocalDispName = entry.vocal_disp_name;
        else this->vocalDispName = entry.vocal_disp_name_en;

        this->file = entry.file;
        this->vocalCharaNum = entry.vocal_chara_num;
    }
};

struct PvDbExSong {
    int chara[6];
    prj::string file;
    prj::string name;
    prj::vector<prj::string> auth;

    PvDbExSong(int chara, prj::string file) : name(), auth() {
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

std::map<int, std::vector<AnotherSongEntry>> pendingAnotherSong;
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

SIG_SCAN(
    sigPvDbCtrl,
    0x1404BB290,
    "\x48\x89\x5C\x24\x10\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x70\xFC",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
);
HOOK(bool, __fastcall, PvDbCtrl, sigPvDbCtrl(), uint64_t a1) {
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

bool Cover::resolveModFilePath(const prj::string& filePath, prj::string& destFilePath) {
    const size_t magicIdx0 = filePath.find(MAGIC);
    if (magicIdx0 == std::string::npos)
        return false;

    const size_t magicIdx1 = filePath.find(MAGIC, magicIdx0 + 1);
    if (magicIdx1 == std::string::npos)
        return false;

    const prj::string center = filePath.substr(magicIdx0 + 1, magicIdx1 - magicIdx0 - 1); // mod folder
    const prj::string right = filePath.substr(magicIdx1 + 1); // file

    destFilePath = center;
    destFilePath += "/";
    destFilePath += right;

    return true;
}

void Cover::init(const std::vector<std::string>& modRomDirectoryPaths) {
	for (auto it = modRomDirectoryPaths.rbegin(); it != modRomDirectoryPaths.rend(); ++it) {
		const std::string dbFilePath = *it + "\\cover_db.toml";

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
                    const auto name = anotherSong["name"].value<std::string>();
                    const auto nameEn = anotherSong["name_en"].value<std::string>();
                    const auto vocalDispName = anotherSong["vocal_disp_name"].value<std::string>();
                    const auto vocalDispNameEn = anotherSong["vocal_disp_name_en"].value<std::string>();
                    const auto vocalChara = anotherSong["vocal_chara"].value<std::string>();
                    const auto file = anotherSong["file"].value<std::string>();

                    if (!pv.has_value() || (!name.has_value() && !nameEn.has_value()) || ((!vocalDispName.has_value() && !vocalDispNameEn.has_value()) && !vocalChara.has_value()) || !file.has_value()) {
                        LOG(" - Failed to load \"%s\": incomplete another_song entry", getRelativePath(dbFilePath).c_str());
                        continue;
                    }

                    AnotherSongEntry newEntry;
                    if (name.has_value()) {
                        newEntry.name = *name;
                    }
                    if (nameEn.has_value()) {
                        newEntry.name_en = *nameEn;
                    }
                    if (vocalDispName.has_value()) {
                        newEntry.vocal_disp_name = *vocalDispName;
                    }
                    if (vocalDispNameEn.has_value()) {
                        newEntry.vocal_disp_name_en = *vocalDispNameEn;
                    }
                    if (vocalChara.has_value()) {
                        newEntry.vocal_chara_num = chara_index_from_name(*vocalChara);
                    }
                    else {
                        newEntry.vocal_chara_num = 10;
                    }
                    newEntry.file = MAGIC + *it + MAGIC + *file;

                    if (pendingAnotherSong.find(*pv) != pendingAnotherSong.end()) {
                        auto& pending = pendingAnotherSong[*pv];
                        pending.push_back(newEntry);
                    }
                    else {
                        pendingAnotherSong[*pv] = { newEntry };
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

                    PvDbExSong newPending(chara_index_from_name(*chara), prj::string(MAGIC + *it + MAGIC + *file));

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

    // Patches to make sure another_song and ex_song can co-exist without crashing
    WRITE_MEMORY(0x1406F2CC4, uint8_t, 0xEB);
    WRITE_NOP(0x1402611CF, 2);

    WRITE_MEMORY(0x15E99EF9A, uint8_t, 0xEB, 0x34);
    WRITE_MEMORY(0x15E99EFF1, uint8_t, 0xEB, 0xB7);
    WRITE_MEMORY(0x15E99F004, uint8_t, 0xEB, 0xA4);

    WRITE_MEMORY(0x15E99EFBB, uint8_t, 0x48, 0x29, 0xD1, 0x48, 0x85, 0xC9, 0x7E, 0x46, 0x4C, 0x01, 0xCA, 0xEB, 0x45);

    WRITE_MEMORY(0x14024A6DD, uint8_t, 0x90, 0x4C, 0x8B, 0x96, 0xB8, 0xD2, 0x02, 0x00, 0x4D, 0x8B, 0x82, 0x88, 0x02, 0x00, 0x00, 0x49, 0x8B, 0x8A, 0x90, 0x02, 0x00, 0x00, 0x4C, 0x29, 0xC1, 0x48, 0xB8, 0xC5, 0x4E, 0xEC, 0xC4, 0x4E, 0xEC, 0xC4, 0x4E, 0x48, 0xF7, 0xE9, 0x48, 0xC1, 0xFA, 0x05, 0x8B, 0x8E, 0x18, 0xD1, 0x02, 0x00, 0x85, 0xD2, 0x74, 0x04, 0x48, 0x83, 0xEA, 0x01, 0x29, 0xD1, 0x8B, 0xD1, 0x85, 0xD2, 0x0F, 0x8E, 0xD7, 0x00, 0x00, 0x00);
}