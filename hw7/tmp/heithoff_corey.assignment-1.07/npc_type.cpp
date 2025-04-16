#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib> 
#include <cstring> 
#include <sstream> 

#include "npc_type.h"
#include "dungeon.h"


static int is_valid_color(const std::string& color) {
    // all caps
    if (color != std::string(color.begin(), color.end())) {
        return 0;
    }

    // acceptable color
    return valid_colors.find(color) != valid_colors.end();
}

static int is_valid_ability(const std::string& ability) {
    // all caps
    if (ability != std::string(ability.begin(), ability.end())) {
        return 0;
    }

    // acceptable color
    return valid_ability.find(ability) != valid_ability.end();
}

std::vector<npc_type> read_monster_desc() 
{
    std::vector<npc_type> npcs;
    std::ifstream i;
    const char* home;
    std::string line;

    // getting home path
    if (!(home = std::getenv("HOME"))) {
        std::cerr << "\"HOME\" is undefined. Using working directory." << std::endl;
        home = ".";
    }

    // opening file
    std::string path = std::string(home) + "/" + SAVE_DIR + "/" + MONSTER_DESC_FILE;
    i.open(path);
    if (!i.is_open()) {
        std::cerr << "Failed to open monster description file at: " << path << std::endl;
        return npcs;
    }

    // checking file version
    std::getline(i, line);
    if (line != "RLG327 MONSTER DESCRIPTION 1") {
        std::cerr << "Invalid monster file version." << std::endl;
        return npcs;
    }

    // parsing monster templates
    while (std::getline(i, line)) {
        // moving to next monster definition
        if (line == "BEGIN MONSTER") {
            npc_type npc;
            int bad_npc = 0;

            // parsing monster fields
            while (std::getline(i, line)) {
                if (line == "END") break;

                std::istringstream iss(line);
                std::string field;
                iss >> field;

                // filling in monster fields
                try{
                    if (fill_field(field, npc, i, iss) < 0) {
                        bad_npc = 1;
                        // skip this monster and move to next in file
                        break;
                    }
                }
                // catching exceptions from functions such stoi() when there is a parsing error
                catch (...){
                    bad_npc = 1;
                    break;
                }
                
            }

            // validate monster fields
            for(int j = 0; j < num_fields; j++){
                if (!npc.valid[j]) {
                    bad_npc = 1;
                    break;
                }
            }

            // add valid monsters to output to print
            if(!bad_npc){
                npcs.push_back(std::move(npc));
            }
        }
    }

    return npcs;
}

int fill_field(const std::string& field, npc_type& npc, std::ifstream& i, std::istringstream& iss) 
{
    std::string str;

    if (field == "NAME" && !npc.valid[NAME]) {
        std::getline(iss, str); 
        npc.name = str.substr(1); // don't want leading space
        npc.valid[NAME] = 1;
    }

    else if (field == "DESC" && !npc.valid[DESC]) {
        std::getline(i, str);
        while (str != ".") {
            // ensuring length is less than 78 characters
            if (str.length() > 77) {
                str = str.substr(0, 77);  
            }
            npc.desc += str + "\n";
            std::getline(i, str);
        }
        npc.valid[DESC] = 1;
    }

    else if (field == "COLOR" && !npc.valid[COLOR]) {
        npc.color.clear();
        while (iss >> str) {
            // checking color input
            if(is_valid_color(str)){
                npc.color.push_back(str);
            }else{
                return -1;
            }
        }
        npc.valid[COLOR] = 1;
    }

    else if (field == "SYMB" && !npc.valid[SYMB]) {
        iss >> str;
        // ensuring a single character is given
        if (str.length() == 1 && !(iss >> str)) {
            npc.symbol = str[0];
        }else{
            return -1;
        }
        npc.valid[SYMB] = 1;
    }

    else if (field == "RRTY" && !npc.valid[RRTY]) {
        std::getline(iss, str);
        // ensuring correct rarity range
        if(stoi(str) > 0 && stoi(str) <= 100){
            npc.rarity = std::stoi(str);
        }else{
            return -1;
        }
        npc.valid[RRTY] = 1;
    }

    else if (field == "ABIL" && !npc.valid[ABIL]) {
        npc.abilities.clear();
        while (iss >> str) {
            // checking ability input
            if(is_valid_ability(str)){
                npc.abilities.push_back(str);
            }else{
                return -1;
            }
            
        }
        npc.valid[ABIL] = 1;
    }

    else if ((field == "SPEED" && !npc.valid[SPEED]) || 
             (field == "HP"    && !npc.valid[HP])    ||
             (field == "DAM"   && !npc.valid[DAM])){
        
        dice_t dice;
        // getting dice details and ensuring format
        std::getline(iss, str, '+');
        dice.base = std::stoi(str);
        str = "non-number";
        std::getline(iss, str, 'd');
        dice.dice = std::stoi(str);
        str = "non-number";
        std::getline(iss, str);
        dice.sides = std::stoi(str);

        if (field == "SPEED") {
            npc.speed = dice;
            npc.valid[SPEED] = 1;
        } else if (field == "HP") {
            npc.hitpoints = dice;
            npc.valid[HP] = 1;
        } else if (field == "DAM") {
            npc.att_damage = dice;
            npc.valid[DAM] = 1;
        }
    }
    // incorrect format
    else {
        return -1;
    }

    return 0;
}

void print_monster_desc(const std::vector<npc_type>& npcs) 
{
    // cycling through monster templates
    for (const auto& npc : npcs) {
        // printing each field in order
        std::cout << npc.name << std::endl;
        std::cout << npc.desc;
        for (const auto& color : npc.color) {
            if (!color.empty()) std::cout << color << " ";
        }
        std::cout << std::endl;
        std::cout << npc.speed.base << "+" << npc.speed.dice << "d" << npc.speed.sides << "\n";
        for (const auto& abil : npc.abilities) {
            if (!abil.empty()) std::cout << abil << " ";
        }
        std::cout << std::endl;
        std::cout << npc.hitpoints.base << "+" << npc.hitpoints.dice << "d" << npc.hitpoints.sides << "\n";
        std::cout << npc.att_damage.base << "+" << npc.att_damage.dice << "d" << npc.att_damage.sides << "\n";
        std::cout << npc.symbol << std::endl;
        std::cout << npc.rarity << std::endl;
        std::cout << std::endl;
    }
}


