#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

#include "../include/cxxopts.hpp"
#include "../include/json.hpp"

#include "permutation.hpp"

cxxopts::Options options("", "");

void exitError(std::string message){

    std::cout << "\033[40;31mERR!\033[0m " << message << "\n";
    std::cout << options.help({""}) << "\n";
    std::exit(1);

}

void printJSONexample(){



}

void printJSONdescription(){



}

std::string jsonValidation(nlohmann::json j, bool encrypt){

    if (!j.count("permutation"))
        return "JSON error: Missing 'permutation' field";
    
    if (encrypt){
        
        if (!j.count("encrypt"))
            return "JSON error: Encrypting mode but 'encrypt' field is missing.";

        if (!j["encrypt"].count("inputFile"))
            return "JSON error: 'inputField' on 'encrypt' is missing.";
        
        if (!j["encrypt"].count("outputFile"))
            return "JSON error: 'outputField' on 'encrypt' is missing.";

    }
    else {

        if (!j.count("decrypt"))
            return "JSON error: Decrypting mode but 'decrypt' field is missing.";

        if (!j["decrypt"].count("inputFile"))
            return "JSON error: 'inputField' on 'decrypt' is missing.";
        
        if (!j["decrypt"].count("outputFile"))
            return "JSON error: 'outputField' on 'decrypt' is missing.";

    }

    return "";

}

void jsonParsing(std::string jsonFile, bool encrypt){

    if (!std::filesystem::exists(jsonFile))
        exitError("JSON file " + jsonFile + " doesn't exists.");
    
    if (std::filesystem::is_directory(jsonFile))
        exitError("JSON file name " + jsonFile + " is a directory.");
    
    std::ifstream reader(jsonFile);
    std::istreambuf_iterator<char> it(reader), end;
    std::string tmp (it, end);
    reader.close();

    try {

        nlohmann::json json = nlohmann::json::parse(tmp);

        if (auto m = jsonValidation(json, encrypt); m != "")
            exitError(m);

        std::vector<int> permutation = json["permutation"];
        if (!perm::hasInverse(permutation))
            exitError("Invalid permutation: No inverse property.");

        std::string inputFile  = json[encrypt ? "encrypt" : "decrypt"]["inputFile"];
        std::string outputFile = json[encrypt ? "encrypt" : "decrypt"]["outputFile"];

        
        

    } catch(std::exception const & e){

        exitError(e.what());

    } 

}

int main(int argc, char * argv[]){

    auto description = "Encrypt / Decrypt files using permutation";
    options = cxxopts::Options("permute", description);

    options
        .add_options()
        ("e,encrypt", "Encrypt using given options in .json file.", cxxopts::value<std::string>(), "[JSON FILE]")
        ("d,decrypt", "Decrypt using given options in .json file.", cxxopts::value<std::string>(), "[JSON FILE]")
        ("json", "View a .json example and/or its description.", cxxopts::value<std::string>(), "[all | example | desc]")
        ("h,help", "Print this help.");

    try {

        auto result = options.parse(argc, argv);

        if (result.count("help"))
            std::cout << options.help({""}) << "\n";
        
        else if (result.count("encrypt")){

            auto jsonFile = result["encrypt"].as<std::string>();
            jsonParsing(jsonFile, true);

        }

        else if (result.count("decrypt")){

            auto jsonFile = result["decrypt"].as<std::string>();
            jsonParsing(jsonFile, false);

        }

        else if (result.count("json")){

            const auto & val = result["json"].as<std::string>();
            if (val == "all"){

                printJSONdescription();
                printJSONexample();

            }

            else if (val == "desc")
                printJSONdescription();

            else if (val == "example") 
                printJSONexample();

            else
                exitError("Invalid --json option:" + val);

        }

        else
            exitError("No arguments provided.");

    } catch(const cxxopts::OptionException & e){

        exitError("Error parsing arguments: " + std::string(e.what()));

    }

    return 0;

}