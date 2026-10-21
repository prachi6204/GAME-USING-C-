#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>
#include <random>
#include <cctype>

using namespace std;

// ==================== BASE CLASS ====================
class Question {
protected:
    string text, category, difficulty;
    int points;

public:
    Question(string t, string c, string d, int p)
        : text(t), category(c), difficulty(d), points(p) {}

    virtual bool ask() = 0;

    string getCategory() const { return category; }
    string getDifficulty() const { return difficulty; }
    int getPoints() const { return points; }

    virtual ~Question() {}
};

// ==================== MULTIPLE-CHOICE QUESTION ====================
class MCQQuestion : public Question {
private:
    string options[4];
    char correctAnswer;

public:
    MCQQuestion(string t, string c, string d, int p,
                string a, string b, string optC, string optD,
                char answer)
        : Question(t, c, d, p), options{a, b, optC, optD},
          correctAnswer(answer) {}

    bool ask() override {
        cout << "\n" << text << "\n";
        for (int i = 0; i < 4; i++)
            cout << char('A' + i) << ". " << options[i] << "\n";

        char answer;
        while (true) {
            cout << "Your answer (A/B/C/D): ";
            cin >> answer;
            answer = static_cast<char>(toupper(static_cast<unsigned char>(answer)));

            if (answer >= 'A' && answer <= 'D')
                break;

            cout << "Invalid choice! Please enter A, B, C, or D.\n";
        }
        return answer == correctAnswer;
    }
};

// ==================== TRUE/FALSE QUESTION ====================
class TrueFalseQuestion : public Question {
private:
    char correctAnswer;

public:
    TrueFalseQuestion(string t, string c, string d, int p, char answer)
        : Question(t, c, d, p), correctAnswer(answer) {}

    bool ask() override {
        cout << "\n" << text << "\n";
        cout << "T. True\nF. False\n";

        char answer;
        while (true) {
            cout << "Your answer (T/F): ";
            cin >> answer;
            answer = static_cast<char>(toupper(static_cast<unsigned char>(answer)));

            if (answer == 'T' || answer == 'F')
                break;

            cout << "Invalid choice! Please enter T or F.\n";
        }
        return answer == correctAnswer;
    }
};

// ==================== PLAYER CLASS ====================
class Player {
private:
    string name;
    int score;

public:
    explicit Player(string n) : name(n), score(0) {}

    void addScore(int points) { score += points; }
    string getName() const { return name; }
    int getScore() const { return score; }
};

// ==================== LEADERBOARD CLASS ====================
class Leaderboard {
public:
    void saveScore(const string& name, int score) {
        ofstream file("leaderboard.txt", ios::app);
        if (file.is_open()) {
            file << name << " " << score << "\n";
        } else {
            cout << "Unable to save leaderboard.\n";
        }
    }

    void showLeaderboard() const {
        ifstream file("leaderboard.txt");
        string name;
        int score;
        vector<pair<string, int>> records;

        if (!file.is_open()) {
            cout << "\nNo previous scores found.\n";
            return;
        }

        while (file >> name >> score)
            records.push_back({name, score});
        file.close();

        sort(records.begin(), records.end(),
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });

        cout << "\n========== LEADERBOARD ==========\n";
        cout << left << setw(20) << "Player" << "Score\n";
        cout << "---------------------------------\n";

        for (int i = 0; i < static_cast<int>(records.size()) && i < 10; i++)
            cout << left << setw(20) << records[i].first
                 << records[i].second << "\n";

        cout << "=================================\n";
    }
};

// ==================== QUIZ GAME CLASS ====================
class QuizGame {
private:
    vector<Question*> questions;
    Leaderboard leaderboard;

    static int difficultyPoints(const string& difficulty) {
        if (difficulty == "Easy") return 10;
        if (difficulty == "Medium") return 20;
        return 30;
    }

    void addMCQ(const string& text, const string& category,
                const string& difficulty, const string& a,
                const string& b, const string& c, const string& d,
                char answer) {
        questions.push_back(new MCQQuestion(
            text, category, difficulty, difficultyPoints(difficulty),
            a, b, c, d, answer));
    }

    void addTF(const string& text, const string& category,
               const string& difficulty, char answer) {
        questions.push_back(new TrueFalseQuestion(
            text, category, difficulty, difficultyPoints(difficulty), answer));
    }

    void createQuestions() {
        // C++
        addMCQ("Which symbol ends a C++ statement?", "C++", "Easy",
               ".", ";", ":", ",", 'B');
        addTF("C++ supports classes and objects.", "C++", "Easy", 'T');
        addMCQ("Which keyword creates an object dynamically?", "C++", "Medium",
               "create", "malloc", "new", "object", 'C');
        addTF("A constructor has the same name as its class.", "C++", "Medium", 'T');
        addMCQ("Which concept allows the same function name with different parameters?",
               "C++", "Hard", "Encapsulation", "Function overloading",
               "Inheritance", "Abstraction", 'B');
        addTF("A virtual function can support runtime polymorphism.", "C++", "Hard", 'T');

        // Python
        addMCQ("Which keyword defines a function in Python?", "Python", "Easy",
               "function", "define", "def", "fun", 'C');
        addTF("Python is a case-sensitive programming language.", "Python", "Easy", 'T');
        addMCQ("Which data type stores key-value pairs in Python?", "Python", "Medium",
               "List", "Tuple", "Set", "Dictionary", 'D');
        addTF("Python lists are immutable.", "Python", "Medium", 'F');
        addMCQ("Which keyword is used to handle an exception in Python?", "Python", "Hard",
               "catch", "except", "error", "handle", 'B');
        addTF("A Python dictionary can store values associated with keys.", "Python", "Hard", 'T');

        // DBMS
        addMCQ("Which SQL command retrieves data from a table?", "DBMS", "Easy",
               "INSERT", "UPDATE", "SELECT", "DELETE", 'C');
        addTF("A primary key uniquely identifies each row in a table.", "DBMS", "Easy", 'T');
        addMCQ("Which SQL command removes a table and its definition?", "DBMS", "Medium",
               "DELETE", "DROP", "UPDATE", "SELECT", 'B');
        addTF("A foreign key can establish a relationship between tables.", "DBMS", "Medium", 'T');
        addMCQ("Which normal form removes partial dependency on a composite key?",
               "DBMS", "Hard", "1NF", "2NF", "3NF", "BCNF", 'B');
        addTF("A B+ tree stores all its data records only in internal nodes.", "DBMS", "Hard", 'F');

        // General Knowledge
        addMCQ("What is the capital of India?", "General", "Easy",
               "Mumbai", "New Delhi", "Chennai", "Kolkata", 'B');
        addTF("The Sun is a star.", "General", "Easy", 'T');
        addMCQ("Which planet is known as the Red Planet?", "General", "Medium",
               "Venus", "Jupiter", "Mars", "Mercury", 'C');
        addTF("The Pacific Ocean is the largest ocean on Earth.", "General", "Medium", 'T');
        addMCQ("Which gas do plants absorb during photosynthesis?", "General", "Hard",
               "Oxygen", "Nitrogen", "Carbon dioxide", "Hydrogen", 'C');
        addTF("Sound travels faster in air than in water.", "General", "Hard", 'F');
    }

    int playComputer(int numberOfQuestions, const string& difficulty) const {
        int computerScore = 0;
        int chance = 50;

        if (difficulty == "Easy") chance = 70;
        else if (difficulty == "Hard") chance = 35;

        int points = difficultyPoints(difficulty);
        for (int i = 0; i < numberOfQuestions; i++) {
            if (rand() % 100 < chance)
                computerScore += points;
        }
        return computerScore;
    }

    void playQuiz(const Player& registeredPlayer) {
        int categoryChoice, difficultyChoice;

        cout << "\n========== SELECT CATEGORY ==========\n"
             << "1. C++\n2. Python\n3. DBMS\n4. General Knowledge\n"
             << "5. Mixed Quiz (All Categories)\n"
             << "Enter category: ";

        if (!(cin >> categoryChoice) || categoryChoice < 1 || categoryChoice > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid category.\n";
            return;
        }

        cout << "\n========== SELECT DIFFICULTY ==========\n"
             << "1. Easy (10 points per correct answer)\n"
             << "2. Medium (20 points per correct answer)\n"
             << "3. Hard (30 points per correct answer)\n"
             << "Enter difficulty: ";

        if (!(cin >> difficultyChoice) || difficultyChoice < 1 || difficultyChoice > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid difficulty.\n";
            return;
        }

        const string categories[] = {"", "C++", "Python", "DBMS", "General", "Mixed"};
        const string difficulties[] = {"", "Easy", "Medium", "Hard"};
        const string selectedCategory = categories[categoryChoice];
        const string selectedDifficulty = difficulties[difficultyChoice];

        vector<Question*> selectedQuestions;
        for (Question* q : questions) {
            bool categoryMatch = selectedCategory == "Mixed" ||
                                 q->getCategory() == selectedCategory;
            bool difficultyMatch = q->getDifficulty() == selectedDifficulty;
            if (categoryMatch && difficultyMatch)
                selectedQuestions.push_back(q);
        }

        random_device rd;
        mt19937 generator(rd());
        shuffle(selectedQuestions.begin(), selectedQuestions.end(), generator);

        if (selectedQuestions.empty()) {
            cout << "No questions available.\n";
            return;
        }

        Player roundPlayer(registeredPlayer.getName());

        cout << "\n============================================\n"
             << "              QUIZ STARTED!\n"
             << "Player: " << roundPlayer.getName() << "\n"
             << "Category: " << selectedCategory << "\n"
             << "Difficulty: " << selectedDifficulty << "\n"
             << "============================================\n";

        for (size_t i = 0; i < selectedQuestions.size(); i++) {
            cout << "\nQuestion " << i + 1 << " of "
                 << selectedQuestions.size() << "\n";

            // Runtime polymorphism: the derived class's ask() is called.
            if (selectedQuestions[i]->ask()) {
                cout << "Correct answer! Well done.\n";
                roundPlayer.addScore(selectedQuestions[i]->getPoints());
            } else {
                cout << "Wrong answer!\n";
            }
        }

        int computerScore = playComputer(
            static_cast<int>(selectedQuestions.size()), selectedDifficulty);

        cout << "\n=============== FINAL RESULTS ===============\n"
             << "Player: " << roundPlayer.getName() << "\n"
             << "Your score: " << roundPlayer.getScore() << "\n"
             << "Computer score: " << computerScore << "\n";

        if (roundPlayer.getScore() > computerScore)
            cout << "Congratulations! YOU WIN!\n";
        else if (roundPlayer.getScore() < computerScore)
            cout << "Computer wins! Better luck next time.\n";
        else
            cout << "It's a DRAW!\n";

        cout << "=============================================\n";
        leaderboard.saveScore(roundPlayer.getName(), roundPlayer.getScore());
        cout << "Your score has been saved to the leaderboard.\n";
    }

public:
    QuizGame() {
        createQuestions();
    }

    ~QuizGame() {
        for (Question* q : questions)
            delete q;
    }

    void start() {
        cout << "\n============================================\n"
             << "           QUIZ BATTLE ARENA\n"
             << "      Object-Oriented Gaming in C++\n"
             << "============================================\n";

        string name;
        cout << "\nEnter your player name (no spaces): ";
        cin >> name;
        Player player(name);

        while (true) {
            int choice;
            cout << "\n=============== MAIN MENU ===============\n"
                 << "1. Start New Quiz\n"
                 << "2. View Leaderboard\n"
                 << "3. Exit Game\n"
                 << "Enter your choice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a valid number.\n";
                continue;
            }

            if (choice == 1) {
                playQuiz(player);
            } else if (choice == 2) {
                leaderboard.showLeaderboard();
            } else if (choice == 3) {
                cout << "\nThank you for playing Quiz Battle Arena!\n";
                break;
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    QuizGame game;
    game.start();
    return 0;
}
