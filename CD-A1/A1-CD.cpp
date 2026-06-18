#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

//--------------------------------------------------------------------------------------------------------------
// Data Structure for General Translator
struct Transition {
    int current_state;
    char input;
    string output;
    int next_state;
};

// Data structure for a Pushdown Translator
struct PDTransition {
    int current_state;
    char input;
    string output;
    string push;
    char pop;
    int next_state;
};

//--------------------------------------------------------------------------------------------------------------
// General translator Class
class GenDT {
    public:
        GenDT(int start_state, vector<Transition> transitions, vector<int> final_states);

        string translate(string word);

    private:
        int q0;
        vector<int> F;
        vector<Transition> T;
};

GenDT::GenDT(int Start_state, vector<Transition> transitions, vector<int> Final_states) {
    q0 = Start_state;
    T = transitions;
    F = Final_states;
}

string GenDT::translate(string word) {
    
    struct Config {
        int state;
        string output;
    };

    vector<Config> current_configs;
    current_configs.push_back({q0, ""});

    for (char c : word)
    {
        vector<Config> next_configs;

        for (const Config& config : current_configs)
        {
            for (const Transition& t : T)
            {
                if (t.current_state == config.state && (t.input == c || t.input == '~'))
                {
                    Config new_config;
                    new_config.state = t.next_state;
                    new_config.output = config.output;

                    if (string(t.output) != "~")
                        new_config.output += t.output;

                    next_configs.push_back(new_config);
                }
            }
        }

        if (next_configs.empty())
            return "word not legal";

        current_configs = next_configs;
    }

    for (const Config& config : current_configs)
    {
        for (int f : F)
        {
            if (config.state == f)
                return config.output;
        }
    }

    return "word not legal";

}

//--------------------------------------------------------------------------------------------------------------
// Pushdown Translator Class
class PDT {
    public:
        PDT(int Start_state, vector<PDTransition> transitions, vector<int> Final_states);

        string translate(string word);

    private:
        int q0;
        vector<PDTransition> T;
        vector<int> F;
        vector<string> S;
};

PDT::PDT(int Start_state, vector<PDTransition> transitions, vector<int> Final_states) {
    q0 = Start_state;
    T = transitions;
    F = Final_states;
}

string PDT::translate(string word) {

    struct Config {
        int state;
        string output;
        string stack;
    };

    vector<Config> current_configs;
    current_configs.push_back({q0, "", ""});

    for (char c : word)
    {
        vector<Config> next_configs;

        for (const Config& config : current_configs)
        {
            for (const PDTransition& t : T)
            {
                if (t.current_state == config.state && (t.input == c || t.input == '~'))
                {

                    if (t.pop != '~')
                    {
                        if (config.stack.empty())
                            continue;

                        if (config.stack.back() != t.pop)
                            continue;
                    }

                    Config new_config;
                    new_config.state = t.next_state;
                    new_config.output = config.output;
                    new_config.stack = config.stack;

                    if (t.pop != '~')
                        new_config.stack.pop_back();

                    if (t.push != "~") {
                        for (char ch : t.push)
                            new_config.stack.push_back(ch);
                        }

                    if (string(t.output) != "~")
                        new_config.output += t.output;

                    next_configs.push_back(new_config);
                }
            }
        }

        if (next_configs.empty())
            return "word not legal";

        current_configs = next_configs;
    }

    for (const Config& config : current_configs)
    {
        for (int f : F)
        {
            if (config.state == f && config.stack.empty())
                return config.output;
        }
    }    

    return "word not legal";
}


//--------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    if (argc != 4) { cout << "Usage: ./main [gen/pdt] [translator file] [input word]" << endl; }

    string ttype = argv[1];
    string filename = argv[2];
    string word = argv[3];
    
    ifstream GDT (filename);

    if (string(ttype) == "gen") {
        int start_state = -1;
        vector<int> final_states;
        vector<Transition> transitions;
        string line;
    
        if (GDT.is_open()) {
            while (getline(GDT, line)) {
            stringstream ss(line);
            
            if (line.rfind("(START)", 0) == 0) {
                string tag;
                ss >> tag >> start_state;
            }
            
            else if (line.rfind("(END)", 0) == 0) {
                string tag;
                int end_state;
                ss >> tag >> end_state;
                final_states.push_back(end_state);
            }
        
            else {
                Transition t;
                ss >> t.current_state
                >> t.input
                >> t.output
                >> t.next_state;
        
                transitions.push_back(t);
                }
            }
        }
        GDT.close();

        GenDT translator(start_state, transitions, final_states);
        string result = translator.translate(word);

        if (string(result) != "word not legal") { cout << "Output: " << result << endl; }
        else { cout << result << "!" << endl; }
        return 0;
    } 

    else if (string(ttype) == "pdt") {
        int start_state = -1;
        vector<int> final_states;
        vector<PDTransition> transitions;

        string line;
        
        if (GDT.is_open()) {
            while (getline(GDT, line)) {
            stringstream ss(line);
            
            if (line.rfind("(START)", 0) == 0) {
                string tag;
                ss >> tag >> start_state;
            }
            
            else if (line.rfind("(END)", 0) == 0) {
                string tag;
                int end_state;
                ss >> tag >> end_state;
                final_states.push_back(end_state);
            }
        
            else {
                PDTransition t;
                ss >> t.current_state
                >> t.input
                >> t.output
                >> t.push
                >> t.pop
                >> t.next_state;
        
                transitions.push_back(t);
                }
            }
        }
        GDT.close();

        PDT translator(start_state, transitions, final_states);
        string result = translator.translate(word);

        if (string(result) != "word not legal") { cout << "Output: " << result << endl; }
        else { cout << result << "!" << endl; }
        return 0;
    }
}