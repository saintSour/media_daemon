#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

namespace fs = std::filesystem;

bool has_extension(const fs::path& p, 
    const std::unordered_set<std::string>& extensions)
{
    std::string ext = p.extension().string();

    if (ext.empty())
    {
        return false;
    }

    return extensions.count(ext) > 0;
}

std::string escape_json(const std::string& s)
{
    std::string out;
    out.reserve(s.size() + 10);
    for (char c : s)
    {
        switch (c)
        {
        case '\"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\b':
            out += "\\b";
            break;
        case '\f':
            out += "\\f";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) 
            {
                char buf[7];
                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                out += buf;
            }
            else 
            {
                out += c;
            }
            break;
        }
    }

    return out;
}

void scan_media(const fs::path& home, const fs::path& out_file)
{
    std::unordered_set<std::string> video_exts = {".mp4", ".avi", ".mov", ".mpg", ".mpeg", ".mkv"};
    std::unordered_set<std::string> audio_exts = {".mp3", ".wav", ".flac", ".aac", ".m4a", ".ogg", ".aiff"};
    std::unordered_set<std::string> image_exts = {".mbp", ".png", ".jpeg", ".jpg", ".gif", ".webp"};

    std::vector<std::string> video;
    std::vector<std::string> audio;
    std::vector<std::string> image;

    for (const auto& entry : fs::recursive_directory_iterator(home, fs::directory_options::skip_permission_denied))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const fs::path& p = entry.path();

        try
        {
            if (has_extension(p, video_exts))
            {
                video.push_back(p.string());
            }
            else if (has_extension(p, audio_exts))
            {
                audio.push_back(p.string());
            }
            else if (has_extension(p, image_exts))
            {
                image.push_back(p.string());
            }
        }
        catch (...) {}
    }

    std::ofstream output(out_file);

    if (!output) 
    {
        std::cerr << "Failed to open output file: " << out_file.string() << std::endl;
        return;
    }

    auto write_vector_to_json = [&](const char* key, const std::vector<std::string>& vec, bool is_last)
    {
        output << " \"" << key << "\": [";
        
        if (!vec.empty())
        {
            output << "\n";
        }

        for (size_t i = 0; i < vec.size(); ++i)
        {
            output << "    \"" << escape_json(vec[i]) << "\"";

            if (i + 1 < vec.size())
            {
                output << ",";
            }

            output << "\n";
        }

        output << "  ]";

        if (!is_last)
        {
            output << ",";
        }

        output << "\n";
    };

    output << "{\n";

    write_vector_to_json("video", video, false);
    write_vector_to_json("audio", audio, false);
    write_vector_to_json("image", image, true);

    output << "}\n";
}

int main(int argc, char** argv)
{
    const char* env_home = std::getenv("HOME");

    if (!env_home)
    {
        std::cerr << "HOME env variable is not set" << std::endl;
        return 1;
    }

    int interval_sec = 60;

    if (argc > 1)
    {
        try
        {
            interval_sec = std::stoi(argv[1]);

            if (interval_sec <= 0)
            {
                interval_sec = 60;
            }
        }
        catch(...)
        {
            std::cerr << "Invalid interval, using default: 60 sec" << std::endl;
        }
    }

    fs::path home(env_home);
    fs::path output_file = home / ".media_files";

    while (true)
    {
        try
        {
            scan_media(home, output_file);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(interval_sec));
    }

    return 0;
}