#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <set>
#include <map>

#include <zip.h>

// return the value of the given environment variable name, or empty if not exist.
// please notice the value of an existing env var could also be empty.
std::string getEnv(std::string env_name) {
    std::string envname = env_name.substr(1, env_name.length() - 2);
    const char * envcstr = std::getenv(envname.c_str());
    std::string env(envcstr == nullptr ? "" : envcstr);
    return env;
}

std::string_view getBaseDirName(std::string_view path) {
    std::string_view::size_type pos = path.find_first_of('/'); // won't be npos
    if (pos == std::string_view::npos) {
        return {};
    }
    std::string_view dirname = path.substr(0, pos);

    return dirname;
}

// get the base dir name of a given path, and also return if it's name is an environment variable name
std::pair<bool, std::string> getEnvBaseDirName(std::string_view path) {
    if (path.length() <= 2) return std::make_pair(false, std::string());

    std::string_view dirname = getBaseDirName(path);

    bool isEnvDir = (dirname.length() > 2 && dirname.at(0) == '%' && dirname.at(dirname.length() - 1) == '%');
    return std::make_pair(isEnvDir, std::string(dirname));
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "[Usage] " << argv[0] << " archive.zip" << std::endl;
        return -1;
    }

    const char* fileName = argv[1];

    // Open the ZIP archive
    int err = 0;
    zip *z = zip_open(fileName, 0, &err);
    if (!z) {
        zip_error_t et;
        zip_error_init_with_code(&et, err);
        printf("zip_open failed (%d: %s)", err, zip_error_strerror(&et));
        zip_error_fini(&et);
        return -1;
    }

    // get all dirs at archive root with %SAMPLE% as name
    const zip_int64_t num_entries = zip_get_num_entries(z, 0);
    std::set<std::string> paths;
    for (zip_uint64_t i = 0; i < (zip_uint64_t)num_entries; i++) {
        const char *name = zip_get_name(z, i, 0);

        bool isEnvDir;
        std::string dirName;
        std::tie(isEnvDir, dirName) = getEnvBaseDirName(name);

        if (isEnvDir) {
            paths.insert(dirName);
        }
    }

    // Check if all environment variable paths actually exists
    bool checkingPassed = true;
    std::map<std::string, std::string> env_var_map;
    for (std::string singlePath : paths) {
        std::string env_val = getEnv(singlePath);
        bool ok = true;
        std::string state;
        if (env_val.empty()) {
            ok = false;
            state = "(empty)";
        } else {
            bool exist = std::filesystem::exists(env_val);
            if (!exist) {
                state = "(not existed)";
                ok = false;
            }
        }

        if (ok) {
            env_var_map.insert_or_assign(singlePath, env_val);
        } else {
            checkingPassed = false;
        }
        std::cout << (ok ? "[OK] " : "[ERROR] ") << singlePath << " : " << env_val << " " << state << std::endl;
    }

    if (!checkingPassed) {
        std::cout << "Check failed!" << std::endl;
        return -1;
    }

    // Extract files
    for (zip_uint64_t i = 0; i < (zip_uint64_t)num_entries; i++) {
        const char *name = zip_get_name(z, i, 0);

        std::string path(name);
        bool isDir = path.at(path.length() - 1) == '/';
        std::string dirName(getBaseDirName(name));
        if (env_var_map.count(dirName) != 0) { // C++20: should use contains()
            path.replace(0, dirName.length(), env_var_map.at(dirName));
            std::filesystem::path filepath(path);
            filepath.make_preferred();
            if (isDir) {
                std::filesystem::create_directories(filepath);
            } else {
                // zip archive file list could be unordered,
                // and it may even don't have a directly related directory item.
                // e.g. /something/file.txt may exist but there is no /something/
                // so we still need to ensure the base dir is created
                std::filesystem::path path{filepath};
                std::filesystem::create_directories(path.parent_path());
                struct zip_stat st;
                zip_stat_init(&st);
                int index = zip_stat_index(z, i, 0, &st);
                char *contents = new char[st.size];

                // Read the compressed file
                zip_file *f = zip_fopen_index(z, i, 0);
                zip_fread(f, contents, st.size);

                std::ofstream ostrm(filepath, std::ios::binary);
                ostrm.write(contents, st.size);
                ostrm.flush();
                ostrm.close();

                zip_fclose(f);
            }
            std::cout << "[Extracted] " << filepath << std::endl;
        }
    }

    // And close the archive
    zip_close(z);

    return 0;
}
