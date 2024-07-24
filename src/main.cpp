#include "json.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

void (*mcpelauncher_preinithook)(const char *sym, void *val, void **orig);
void (*mcpelauncher_log)(int level, const char *tag, const char *text);

std::string dataDir = "/data/data/com.mojang.minecraftpe";
std::vector<std::string> shadersList;
std::vector<std::string> folderList;
std::vector<std::string> packIdArray;
std::vector<std::string> subpackArray;

int countCharacterOccurrences(const std::string &str, char character) {
  int count = 0;
  for (char c : str) {
    if (c == character) {
      count++;
    }
  }
  return count;
}

std::string assetsToRoot;

extern "C" void __attribute__((visibility("default"))) mod_preinit() {
  std::string assetsDir;

  // Below is a hack to get the real path to the assets folder and game data
  // folder
  char originalDir[PATH_MAX];
  getcwd(originalDir, sizeof(originalDir));
  chdir(dataDir.c_str());
  char currentDir[PATH_MAX];
  getcwd(currentDir, sizeof(currentDir));
  chdir(".");
  char currentDir2[PATH_MAX];
  getcwd(currentDir2, sizeof(currentDir2));
  chdir(originalDir);
  dataDir = currentDir; // gets the data dir using chdir which uses iorewrite
  assetsDir =
      currentDir2; // gets the assets dir using chdir which uses iorewrite
  chdir("/");

  int occurrences = countCharacterOccurrences(assetsDir, '/');

  for (int i = 0; i < occurrences; i++) {
    assetsToRoot +=
        "../"; // hack to get back to the root directory using the assets folder
               // as the start due to how the asset makager works
  }

  auto h = dlopen("libmcpelauncher_mod.so", 0);

  DIR *dr2;
  struct dirent *en2;
  dr2 = opendir((dataDir + "/games/com.mojang/minecraftpe").c_str());

  if (dr2) {
    std::ifstream file2((dataDir +
                         "/games/com.mojang/minecraftpe/"
                         "global_resource_packs.json")); // open all directory
    if (file2) {
      nlohmann::json j = nlohmann::json::parse(file2);
      auto j_str = to_string(j[0]["pack_id"]);
      packIdArray.push_back(j_str);
      if (j[0]["subpack"] != NULL) {
        auto e_str = to_string(j[0]["subpack"]);
        subpackArray.push_back(e_str);
        std::cout << e_str << std::endl;
      }
      std::cout << j_str << std::endl;
    }
    file2.close();
    closedir(dr2);
  }

  DIR *dir;
  struct dirent *ent;
  struct dirent *en;
  struct dirent *en3;
  dir = opendir((dataDir + "/games/com.mojang/resource_packs").c_str());
  if (dir) {
    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL) {
      DIR *dir2;
      DIR *dir3;
      dir2 = opendir((dataDir + "/games/com.mojang/resource_packs/" +
                      std::string(ent->d_name) + "/renderer/materials")
                         .c_str());

      std::ifstream file((dataDir + "/games/com.mojang/resource_packs/" +
                          std::string(ent->d_name) + "/manifest.json"));
      if (dir2) {
        nlohmann::json j = nlohmann::json::parse(file);
        auto j_str = to_string(j["header"]["uuid"]);
        subpackArray[0].erase(
            std::remove(subpackArray[0].begin(), subpackArray[0].end(), '\"'),
            subpackArray[0].end());

        if (j_str == packIdArray[0]) {
          dir3 = opendir((dataDir + "/games/com.mojang/resource_packs/" +
                          std::string(ent->d_name) + "/subpacks/" +
                          subpackArray[0] + "/renderer/materials")
                             .c_str());
          folderList.push_back(std::string(ent->d_name));
          while ((en = readdir(dir2)) != NULL) {
            if (dir3) {
              while ((en3 = readdir(dir3)) != NULL) {
                if (strstr(en3->d_name, ".material.bin")) {
                  std::string e = folderList[0] + "/subpacks/" +
                                  subpackArray[0] + "/renderer/materials/" +
                                  std::string(en3->d_name);
                  shadersList.push_back(std::string(e));
                  std::cout << std::string(e) << std::endl;
                }
                printf("%s\n", "Subpack Found");
              }
              closedir(dir3);
            }

            if (strstr(en->d_name, ".material.bin")) {
              std::string e = folderList[0] + "/renderer/materials/" +
                              std::string(en->d_name);
              shadersList.push_back(std::string(e));
            }
          }
        } else {
        }

        file.close();
        closedir(dir2);
      }
    }
    closedir(dir);
  }

  mcpelauncher_preinithook =
      (decltype(mcpelauncher_preinithook))dlsym(h, "mcpelauncher_preinithook");
  mcpelauncher_log = (decltype(mcpelauncher_log))dlsym(h, "mcpelauncher_log");

  mcpelauncher_preinithook(
      "AAssetManager_open",
      (void *)+[](AAssetManager *mgr, const char *filename,
                  int mode) -> AAsset * {
        if ((strstr(filename, ".material.bin"))) {
          std::string fName = std::string(filename).substr(
              std::string(filename).find_last_of("/") + 1);
          if (std::find(shadersList.begin(), shadersList.end(), fName) !=
              shadersList.end()) {
            __android_log_print(ANDROID_LOG_VERBOSE, "ShadersMod",
                                "Patched shader %s via AAssetManager",
                                fName.c_str());
            return AAssetManager_open(mgr,
                                      (assetsToRoot + dataDir +
                                       "/games/com.mojang/resource_packs/" +
                                       fName)
                                          .c_str(),
                                      mode); // uses custom asset path like
            // /path/to/assets/../../../ to get to root

          } else {
            return AAssetManager_open(mgr, filename, mode);
          }
        } else {
          return AAssetManager_open(mgr, filename, mode);
        }
      },
      nullptr);

  // dlclose(h);
}

extern "C" __attribute__((visibility("default"))) void mod_init() {}
