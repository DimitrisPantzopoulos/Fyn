#include "uci.hpp"
#include "../search/search.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace UCI {
    // Helper Functions
    std::vector<std::string> split(const std::string& str, char delimiter = ' ') {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string word;

        while (std::getline(ss, word, delimiter)) {
            if (!word.empty())
                tokens.push_back(word);
        }

        return tokens;
    }

    Command find_keyword(std::string& cmd) {
        if      (cmd == "uci")        return Command::UCI;
        else if (cmd == "isready")    return Command::ISREADY;
        else if (cmd == "ucinewgame") return Command::UCINEWGAME;
        else if (cmd == "position")   return Command::POSITION;
        else if (cmd == "go")         return Command::GO;
        else if (cmd == "stop")       return Command::STOP;
        else if (cmd == "quit")       return Command::QUIT;

        return Command::NONE;
    }

    uint32_t calculate_tts(uint32_t time_remaining, uint32_t inc) {
        return (time_remaining / 20) + (inc / 2);
    }

    void print_banner() {
        static constexpr char banner[] = R"BANNER(
        ________                  
        |_   __  |                 
          | |_ \_|_   __  _ .--.   
          |  _|  [ \ [  ][ `.-. |  
         _| |_    \ '/ /  | | | |  
        |_____| [\_:  /  [___||__] 
                \__.'                    
        )BANNER";

        std::cout << banner << '\n';
    }

    void go_info(Info& info, const std::vector<std::string>& tokens) {
        int idx = 0;

        auto has_value = [&](int i) {
            return (i + 1) < static_cast<int>(tokens.size());
        };

        while (idx < static_cast<int>(tokens.size())) {
            if (tokens[idx] == "wtime" && has_value(idx)) {
                info.w_time = static_cast<uint32_t>(std::stoi(tokens[idx + 1]));
                idx += 2;
            } else if (tokens[idx] == "btime" && has_value(idx)) {
                info.b_time = static_cast<uint32_t>(std::stoi(tokens[idx + 1]));
                idx += 2;
            } else if (tokens[idx] == "winc" && has_value(idx)) {
                info.w_inc = static_cast<uint32_t>(std::stoi(tokens[idx + 1]));
                idx += 2;
            } else if (tokens[idx] == "binc" && has_value(idx)) {
                info.b_inc = static_cast<uint32_t>(std::stoi(tokens[idx + 1]));
                idx += 2;
            } else if (tokens[idx] == "depth" && has_value(idx)) {
                info.depth = static_cast<uint32_t>(std::stoi(tokens[idx + 1]));
                idx += 2;
            } else {
                idx++;
            }
        }

        // Calculate TTS using the provided information. min is 100 milliseconds(assumes the time control information has been provided)
        uint32_t time_remaining;
        uint32_t inc;

        if (info.board.sideToMove() == chess::Color::WHITE) 
                { time_remaining = info.w_time; inc = info.w_inc; }
        else    { time_remaining = info.b_time; inc = info.b_inc; }

        info.milliseconds = std::max<uint32_t>(calculate_tts(time_remaining, inc), 100);
    }

    // UCI Command Implementations
    void uci() {
        print_banner();

        // Standard UCI info: engine name, and author
        std::cout << "id name Fyn" << std::endl;
        std::cout << "id author Dimiboi " << std::endl;
        std::cout << "uciok" << std::endl;
    }

    void is_ready() {
        std::cout << "readyok" << std::endl;
    }

    void stop() {
        // TODO: Make the stop implementation
        std::cout << "[Fyn] Stop has not been implemented yet, to stop execution type 'quit'." << std::endl;
    }

    void quit() {
        std::cout << "[Fyn] Goodbye!" << std::endl;
        std::exit(0);
    }

    void uci_new_game(Search::Search& fyn, Info& info) {
        info.board = chess::Board();
        info.moves.clear();

        // Reset internal tables
        fyn.clear_tables();

        // Reset timing info
        info.w_time = 0;
        info.b_time = 0;
        info.w_inc  = 0;
        info.b_inc  = 0;
    }

    void position(Info& info, const std::vector<std::string>& tokens) {
        // Guard Clause: This command cannot have less than 2 tokens
        if (tokens.size() < 2) return;

        std::string   fen;
        std::uint32_t idx = 1;
        std::string   position_token = tokens[idx];

        if (position_token == "startpos") {
            fen = chess::constants::STARTPOS;
            idx++;
        }else if (position_token == "fen") {
            idx++;
            // Collect the FEN string
            while(idx < tokens.size() && tokens[idx] != "moves") {
                fen += tokens[idx] + " ";
                idx++;
            }
        }else {
            std::cout << "[Fyn] Error: Invalid position command." << std::endl;
            return;
        }

        // Apply the FEN to the board and clear the move list
        info.moves.clear();
        
        if (!info.board.setFen(fen)) {
            std::cout << "[Fyn] Error: Invalid FEN in position command: " << fen << std::endl;
            return;
        }

        // Parse and apply moves if any
        if (idx < tokens.size() && tokens[idx] == "moves") {
            idx++;

            while (idx < tokens.size()) {
                chess::Move move = chess::uci::uciToMove(info.board, tokens[idx]);
                info.board.makeMove(move);
                info.moves.add(move);
                idx++;
            }
        }
    }

    void go(Info& info, const std::vector<std::string>& tokens, Search::Search& fyn) {
        // Collect the Search Info from the tokens
        go_info(info, tokens);

        fyn.search_position(info);
    }

    void uci_protocol() {
        std::cout.sync_with_stdio(false);

        Info info = Info();
        std::string cmd;

        Search::Search fyn = Search::Search();

        // Wait for input then execute the command
        while (std::getline(std::cin, cmd)) {
            std::vector<std::string> tokens = split(cmd);

            if (tokens.empty()) {
                continue;
            }

            std::string init_token = tokens[0];
            Command keyword = find_keyword(init_token);
            
            switch(keyword) {
                case Command::UCI:
                    uci();
                    break;
                case Command::ISREADY:
                    is_ready();
                    break;
                case Command::UCINEWGAME:
                    uci_new_game(fyn, info);
                    break;
                case Command::POSITION:
                    position(info, tokens);
                    break;
                case Command::GO:
                    go(info, tokens, fyn);
                    break;
                case Command::STOP:
                    stop();
                    break;
                case Command::QUIT:
                    quit();
                    break;
                default:
                    std::cout << "[Fyn] Unknown Keyword: " << init_token << std::endl;
                    break;
            }
        }   
    }
} // namespace UCI
