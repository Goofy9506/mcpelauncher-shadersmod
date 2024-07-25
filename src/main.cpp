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
  
  DIR *minecraftDir;
  minecraftDir = opendir((dataDir + "/games/com.mojang/minecraftpe").c_str());
  
  if (minecraftDir) {
    std::ifstream globalResource(dataDir + "/games/com.mojang/minecraftpe/global_resource_packs.json");

    if (globalResource) {
      nlohmann::json j = nlohmann::json::parse(globalResource);

      for (const auto& item : j) {
        auto j_str = to_string(item["pack_id"]);
        packIdArray.push_back(j_str);
        if (!item["subpack"].is_null()) {
          auto e_str = to_string(item["subpack"]);
          subpackArray.push_back(e_str);
          std::cout << e_str << std::endl;
        }
        std::cout << j_str << std::endl;
      }
    }
    globalResource.close();
    closedir(minecraftDir);
  }

  struct dirent *folderEntry;
  struct dirent *renderEntry;
  struct dirent *subpackEntry;
  DIR *resourcePacks;
  resourcePacks = opendir((dataDir + "/games/com.mojang/resource_packs").c_str());
  if (resourcePacks) {
    /* print all the files and directories within directory */
    while ((folderEntry = readdir(resourcePacks)) != NULL) {
      DIR *folderMats;
      folderMats = opendir((dataDir + "/games/com.mojang/resource_packs/" +
                          std::string(folderEntry->d_name) + "/renderer/materials").c_str());
      std::ifstream manifest((dataDir + "/games/com.mojang/resource_packs/" +
                          std::string(folderEntry->d_name) + "/manifest.json"));
      if (folderMats) {
        // loads the manifest = file
        nlohmann::json j = nlohmann::json::parse(manifest);
        std::string j_str = to_string(j["header"]["uuid"]);

        // removes "" from the subpackArray string
        subpackArray[0].erase(std::remove(subpackArray[0].begin(), subpackArray[0].end(), '\"'), subpackArray[0].end());

        // loads the subpack directory
        DIR *subpackDir;
        subpackDir = opendir((dataDir + "/games/com.mojang/resource_packs/" +
                        std::string(folderEntry->d_name) + "/subpacks/" +
                        subpackArray[0] + "/renderer/materials").c_str());

        for (size_t it = 0; it < packIdArray.size(); it++) {
          if (j_str == packIdArray[it]) {
            folderList.push_back(std::string(folderEntry->d_name));
            while ((renderEntry = readdir(folderMats)) != NULL) {
              if (strstr(renderEntry->d_name, ".material.bin")) {
                std::string e = folderList[0] + "/renderer/materials/" + std::string(renderEntry->d_name);
                if (folderList.size() == 1) {
                    shadersList.push_back(std::string(e));
                    std::cout << "Shader Found" << std::endl;
                }
              }
            }

            if (subpackDir) {
              while ((subpackEntry = readdir(subpackDir)) != NULL) {
                if (strstr(subpackEntry->d_name, ".material.bin")) {
                  std::string e = folderList[0] + "/subpacks/" + subpackArray[0] + "/renderer/materials/" + std::string(subpackEntry->d_name);
                  if (folderList.size() == 1) {
                    shadersList.push_back(std::string(e));
                    std::cout << std::string(e) << std::endl;
                  }
                }
                std::cout << "Subpack Found" << std::endl;
              }
            }
          }
        }
        manifest.close();
        closedir(subpackDir);
      }
      closedir(folderMats);
    }
    closedir(resourcePacks);
  }

  mcpelauncher_preinithook = (decltype(mcpelauncher_preinithook))dlsym(h, "mcpelauncher_preinithook");
  mcpelauncher_log = (decltype(mcpelauncher_log))dlsym(h, "mcpelauncher_log");

  mcpelauncher_preinithook(
      "AAssetManager_open",
      (void *)+[](AAssetManager *mgr, const char *filename,
                  int mode) -> AAsset * {
        if (strstr(filename, ".material.bin")) {
          std::string fName = std::string(filename).substr(
              std::string(filename).find_last_of("/") + 1);
          std::string shaderLoc =
              folderList[0] + "/renderer/materials/" + fName;
          std::string shaderLoc2 = folderList[0] + "/subpacks/" +
                                   subpackArray[0] + "/renderer/materials/" +
                                   fName;

          auto openAsset = [&](const std::string &path) {
            __android_log_print(ANDROID_LOG_VERBOSE, "ShadersMod",
                                "Patched shader %s via AAssetManager",
                                fName.c_str());
            return AAssetManager_open(mgr,
                                      (assetsToRoot + dataDir +
                                       "/games/com.mojang/resource_packs/" +
                                       path)
                                          .c_str(),
                                      mode);
          };

          if (std::find(shadersList.begin(), shadersList.end(), shaderLoc) !=
              shadersList.end()) {
            return openAsset(shaderLoc);
          } else if (std::find(shadersList.begin(), shadersList.end(),
                               shaderLoc2) != shadersList.end()) {
            return openAsset(shaderLoc2);
          }
        }
        return AAssetManager_open(mgr, filename, mode);
      },
      nullptr);

  // dlclose(h);
}

extern "C" __attribute__((visibility("default"))) void mod_init() {}
