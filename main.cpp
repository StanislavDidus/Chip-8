#include <iostream>
#include <filesystem>
#include <fstream>
#include <array>
#include <unordered_map>

// Steps
// 1. Load file
// 2. Read file
// 3. Translate characters into decimal values
// 4. Write into a new file


struct app_context
{
    std::string file_name{};
    std::stringstream buffer{};
    std::vector<uint8_t> bytes{};
    uint16_t address_offset = 0x200;

    uint32_t current_line = 0;

    std::unordered_map<std::string, uint16_t> stored_addresses;
};

int hex_char_to_val(char c, const app_context& context) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::runtime_error{std::format("Wrong character: {} on line: {}", c, context.current_line)};
}

std::string get_address(const std::string& text)
{
    // If string doesn't contain &
    if (text.find('&') == std::string::npos)
    {
        size_t first = text.find('*');
        if (first == std::string::npos)
            return "";

        size_t second = text.find('*', first + 1);
        if (second == std::string::npos)
            return "";

        return text.substr(first + 1, second - first - 1);
    }

    size_t first = text.find('&');
    if (first == std::string::npos)
        return "";

    size_t second = text.find('&', first + 1);
    if (second == std::string::npos)
        return "";

    return text.substr(first + 1, second - first - 1);
}



void load_file(app_context& context, const std::filesystem::path& input_path)
{
    context.file_name = input_path.stem().string();
   std::ifstream file{input_path};
    if (!file)
       throw std::runtime_error{"Could not open file for reading."};

    context.buffer << file.rdbuf();

    std::cout << "Loaded file." << std::endl;
}

void read_file(app_context& context)
{
    uint32_t characters_read = 0;
    uint8_t current_byte = 0;

    // Read buffer line by line
    // First parsing
    // Remember all addresses
    for (std::string line; std::getline(context.buffer, line); )
    {
        // Read line character by character
        for (int i = 0; i < line.length(); ++i)
        {
            auto& letter = line[i];

            if (letter == ' ') continue;
            if (letter == '#') break;

            if (letter == '*')
            {
                context.address_offset += 2;
                characters_read = 0;
                break;
            }

            // If encountered address declaration
            if (letter == '&')
            {
                std::string address = get_address(line);
                context.stored_addresses[address] = context.address_offset;
                characters_read = 0;

                break;
            }

            characters_read += 1;

            if (characters_read >= 2)
            {
                characters_read = 0;

                context.address_offset += 1;
            }
        }
    }

    context.buffer.clear();
    context.buffer.seekg(0);

    characters_read = 0;
    // Second pass
    // Write all bytes to the file and paste the addresses
    for (std::string line; std::getline(context.buffer, line); )
    {
        // Read line character by character
        for (int i = 0; i < line.length(); ++i)
        {
            auto& letter = line[i];

            if (letter == ' ') continue;
            if (letter == '#') break;
            if (letter == '&') break;

            if (letter == '*')
            {
                std::string address = get_address(line);
                if (context.stored_addresses.contains(address))
                {
                    uint16_t stored_address = context.stored_addresses.at(get_address(line));

                    current_byte = (current_byte << 4) | ((stored_address & 0xF00) >> 8);
                    context.bytes.push_back(current_byte);
                    current_byte = stored_address & 0xFF;
                    context.bytes.push_back(current_byte);

                    characters_read = 0;
                    current_byte = 0;
                    break;
                }
            }

            current_byte = (current_byte << 4) | hex_char_to_val(letter, context);
            characters_read += 1;

            if (characters_read >= 2)
            {
                context.bytes.push_back(current_byte);
                characters_read = 0;
                current_byte = 0;
            }
        }

        context.current_line += 1;
    }

    std::cout << "Read file." << std::endl;
}

void write_file(app_context& context, const std::filesystem::path& output_path)
{
    if (context.bytes.empty())
    {
        std::cerr << "File is considered empty. Please verify the contents." << std::endl;
        return;
    }

    if (!output_path.empty()) {
        std::filesystem::create_directories(output_path);
    }

    std::filesystem::path file_path = output_path / (context.file_name + ".ch8");
    std::ofstream file(file_path, std::ios::binary);
    if (!file)
        throw std::runtime_error{"Could not open file for writing."};

    file.write(reinterpret_cast<const char*>(context.bytes.data()), context.bytes.size() * sizeof(uint8_t));

    if (file.bad())
        throw std::runtime_error{"Failed writing to a file."};

    file.close();

    std::cout << "Wrote file." << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cerr << "Not enough arguments." << std::endl;
    }

    app_context context{};

    try
    {
        load_file(context, argv[1]);

        read_file(context);

        std::filesystem::path output_path = argc == 3 ? argv[2] : std::filesystem::current_path();
        write_file(context, output_path);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
