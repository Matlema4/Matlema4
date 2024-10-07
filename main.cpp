#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

struct BookManagement {
    string ISBN;
    string title;
    string author;
    int PublicationYear;
    bool availabilityStatus;
    double cost; // Cost of the book

    string toFileString() const {
        stringstream ss;
        ss << ISBN << "|" << title << "|" << author << "|" << PublicationYear << "|"
           << availabilityStatus << "|" << cost;
        return ss.str();
    }

    static BookManagement fromFileString(const string& line) {
        BookManagement book;
        stringstream ss(line);
        string availabilityStatus;
        string costStr;

        getline(ss, book.ISBN, '|');
        getline(ss, book.title, '|');
        getline(ss, book.author, '|');
        ss >> book.PublicationYear;
        ss.ignore(); // Skip the delimiter '|'
        getline(ss, availabilityStatus, '|');
        book.availabilityStatus = (availabilityStatus == "1");
        getline(ss, costStr, '|');

        try {
            book.cost = stod(costStr);
        } catch (const invalid_argument& e) {
            cerr << "Error: Invalid cost value for book " << book.title << " -> " << e.what() << endl;
            book.cost = 0.0; // Default cost value in case of an error
        }

        return book;
    }
};

struct User {
    int ID;
    string userName;
    string contactInfo;
    char membershipType;
    double balance; // Add balance attribute for student

    string toFileString() const {
        stringstream ss;
        ss << userName << "|" << contactInfo << "|" << ID << "|" << membershipType;
        if (membershipType == 'P') {
            ss << "|" << balance;
        }
        return ss.str();
    }

    static User fromFileString(const string& line) {
        User user;
        stringstream ss(line);
        string membershipType;

        getline(ss, user.userName, '|');
        getline(ss, user.contactInfo, '|');
        ss >> user.ID;
        ss.ignore(); // Skip the delimiter '|'
        getline(ss, membershipType, '|');
        user.membershipType = membershipType[0];
        user.balance = (user.membershipType == 'P') ? 0.0 : 0.0; // Default for librarians
        if (user.membershipType == 'P') {
            ss >> user.balance;
        }

        return user;
    }
};

class Library {
private:
    vector<BookManagement> books;
    vector<User> users;
    const string bookFilename = "books.txt";
    const string userFilename = "users.txt";

    void loadBooks() {
        ifstream infile(bookFilename);
        if (!infile) return;

        string line;
        while (getline(infile, line)) {
            books.push_back(BookManagement::fromFileString(line));
        }
    }

    void saveBooks() const {
        ofstream outfile(bookFilename);
        for (const BookManagement& book : books) {
            outfile << book.toFileString() << endl;
        }
    }

    void loadUsers() {
        ifstream infile(userFilename);
        if (!infile) return;

        string line;
        while (getline(infile, line)) {
            users.push_back(User::fromFileString(line));
        }
    }

    void saveUsers() const {
        ofstream outfile(userFilename);
        for (const User& user : users) {
            outfile << user.toFileString() << endl;
        }
    }

public:
    Library() {
        srand(static_cast<unsigned>(time(nullptr)));
        loadBooks();
        loadUsers();

        // Initial data
        vector<string> titles = {
            "Clean Code", "The C++ Programming Language", "Introduction to Algorithms",
            "Design Patterns", "Effective C++", "Head First Design Patterns",
            "The Pragmatic Programmer", "Code Complete", "Refactoring: Improving the Design of Existing Code",
            "Modern C++ Programming"
        };
        vector<string> authors = {
            "Robert C. Martin", "Bjarne Stroustrup", "Thomas H. Cormen",
            "Erich Gamma", "Scott Meyers", "Elisabeth Freeman",
            "Andrew Hunt", "Steve McConnell", "Martin Fowler",
            "Meier, Josuttis, Sutter"
        };

        for (int i = 0; i < 10; ++i) {
            BookManagement book;
            book.ISBN = "ISBN-" + to_string(i + 1);
            book.title = titles[i];
            book.author = authors[i];
            book.PublicationYear = 1990 + rand() % 30;
            book.availabilityStatus = true;
            book.cost = 10.0 + rand() % 30;
            books.push_back(book);
        }

        saveBooks();
    }

    bool authenticateUser(int ID, const string& userName) const {
        for (const User& user : users) {
            if (user.ID == ID && user.userName == userName) {
                return true;
            }
        }
        return false;
    }

    bool isLibrarian(int ID, const string& userName) const {
        for (const User& user : users) {
            if (user.ID == ID && user.userName == userName && user.membershipType == 'L') {
                return true;
            }
        }
        return false;
    }

    void searchBook() {
        system("cls");
        int choice;
        cout << "Menu: " << endl;
        cout << "1. Search using book title." << endl;
        cout << "2. Search using Author name." << endl;
        cout << "3. Back to Menu." << endl;
        cin >> choice;
        cin.ignore();
        string searchTitle;
        string authorname;

        switch (choice) {
            case 1:
                cout << "Enter book title to search: ";
                getline(cin, searchTitle);
                break;
            case 2:
                cout << "Enter author name to search: ";
                getline(cin, authorname);
                break;
            case 3:
                return;
            default:
                cout << "ERROR!\n";
                return;
        }

        bool found = false;
        for (const BookManagement& book : books) {
            if (book.title == searchTitle || book.author == authorname) {
                cout << "Book Found: " << book.title << " by " << book.author << endl;
                cout << "Availability: " << (book.availabilityStatus ? "Available" : "Borrowed") << endl;
                cout << "Cost: R" << book.cost << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Book not found." << endl;
        }
    }

    void borrowBook(int userID) {
        system("cls");
        string titleToBorrow;
        cout << "Enter book title to borrow: ";
        getline(cin, titleToBorrow);

        User* user = nullptr;
        for (User& u : users) {
            if (u.ID == userID) {
                user = &u;
                break;
            }
        }

        if (user == nullptr || user->membershipType != 'P') {
            cout << "Only students can borrow books." << endl;
            return;
        }

        for (BookManagement& book : books) {
            if (book.title == titleToBorrow && book.availabilityStatus) {
                if (user->balance >= book.cost) {
                    book.availabilityStatus = false;
                    user->balance -= book.cost;
                    saveBooks();
                    saveUsers();
                    cout << "Book borrowed. Remaining balance: R" << user->balance << endl;
                } else {
                    cout << "Insufficient balance. Please deposit funds before borrowing." << endl;
                }
                return;
            }
        }
        cout << "Book not available or not found." << endl;
    }

    void returnBook() {
        system("cls");
        string titleToReturn;
        cout << "Enter book title to return: ";
        getline(cin, titleToReturn);

        for (BookManagement& book : books) {
            if (book.title == titleToReturn && !book.availabilityStatus) {
                book.availabilityStatus = true;
                saveBooks();
                cout << "Book returned." << endl;
                return;
            }
        }
        cout << "Book not found or already available." << endl;
    }

    void addBook(const string& ISBN, const string& title, const string& author, int PublicationYear, double cost) {
        system("cls");
        BookManagement newBook = {ISBN, title, author, PublicationYear, true, cost};
        books.push_back(newBook);
        saveBooks();
        cout << "Book is successfully added." << endl;
    }

    void deposit(int userID, double amount) {
        for (User& user : users) {
            if (user.ID == userID && user.membershipType == 'P') {
                user.balance += amount;
                saveUsers();
                cout << "Deposit successful. New balance: R" << user.balance << endl;
                return;
            }
        }
        cout << "User not found or not a student." << endl;
    }

    void registerUser() {
        system("cls");
        User newUser;
        cout << "Enter your username: ";
        getline(cin, newUser.userName);
        cout << "Enter your contact info: ";
                getline(cin, newUser.contactInfo);
        cout << "Enter your ID: ";
        cin >> newUser.ID;
        cin.ignore();
        cout << "Enter your membership type (P for Students, L for Librarian): ";
        cin >> newUser.membershipType;
        cin.ignore();

        if (newUser.membershipType == 'P') {
            newUser.balance = 0.0; // Initial balance for new students
        }

        users.push_back(newUser);
        saveUsers();

        cout << "Registration successful. You can now log in." << endl;
    }

    void viewBalance(int userID) const {
        for (const User& user : users) {
            if (user.ID == userID && user.membershipType == 'P') {
                cout << "Your current balance is: R" << user.balance << endl;
                return;
            }
        }
        cout << "User not found or not a student." << endl;
    }

    void viewAvailableBooks() const {
        system("cls"); // Clears the screen for a cleaner output (optional)
        cout << "Available Books:\n";
        bool hasAvailableBooks = false;

        for (const BookManagement& book : books) {
            if (book.availabilityStatus) {
                cout << "Title: " << book.title << "\n";
                cout << "Author: " << book.author << "\n";
                cout << "Publication Year: " << book.PublicationYear << "\n";
                cout << "Cost: R" << book.cost << "\n";
                cout << "---------------------------\n";
                hasAvailableBooks = true;
            }
        }

        if (!hasAvailableBooks) {
            cout << "No books are currently available.\n";
        }
    }
};

int main() {
    Library library;

    int ID, choice;
    string userName;

    while (true) {
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "Enter your ID: ";
            cin >> ID;
            cin.ignore();
            cout << "Enter your username: ";
            getline(cin, userName);

            if (library.authenticateUser(ID, userName)) {
                bool isLibrarian = library.isLibrarian(ID, userName);

                do {
                    cout << "\nLibrary Menu:\n";
                    cout << "1. Add Book (Librarian Only)\n";
                    cout << "2. Search Book\n";
                    cout << "3. Borrow Book\n";
                    cout << "4. Return Book\n";
                    cout << "5. Deposit Funds\n";
                    cout << "6. View Balance\n";
                    cout << "7. View Available Books\n";
                    cout << "8. Logout\n";
                    cout << "9. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;
                    cin.ignore();

                    switch (choice) {
                        case 1:
                            if (isLibrarian) {
                                string ISBN, title, author;
                                int year;
                                double cost;
                                cout << "Enter ISBN: ";
                                getline(cin, ISBN);
                                cout << "Enter Title: ";
                                getline(cin, title);
                                cout << "Enter Author: ";
                                getline(cin, author);
                                cout << "Enter Publication Year: ";
                                cin >> year;
                                cout << "Enter Cost: ";
                                cin >> cost;
                                cin.ignore();
                                library.addBook(ISBN, title, author, year, cost);
                            } else {
                                cout << "Access denied. Only librarians can add books." << endl;
                            }
                            break;
                        case 2:
                            library.searchBook();
                            break;
                        case 3:
                            if (!isLibrarian) {
                                library.borrowBook(ID);
                            } else {
                                cout << "Librarians cannot borrow books." << endl;
                            }
                            break;
                        case 4:
                            if (!isLibrarian) {
                                library.returnBook();
                            } else {
                                cout << "Librarians cannot return books." << endl;
                            }
                            break;
                        case 5:
                            if (!isLibrarian) {
                                double depositAmount;
                                cout << "Enter amount to deposit: ";
                                cin >> depositAmount;
                                cin.ignore();
                                library.deposit(ID, depositAmount);
                            } else {
                                cout << "Librarians cannot deposit funds." << endl;
                            }
                            break;
                        case 6:
                            if (!isLibrarian) {
                                library.viewBalance(ID);
                            } else {
                                cout << "Librarians do not have a balance." << endl;
                            }
                            break;
                        case 7:
                            library.viewAvailableBooks();
                            break;
                        case 8:
                            cout << "Logging out..." << endl;
                            break;
                        case 9:
                            cout << "Exiting library system. Thanks for visiting!" << endl;
                            return 0;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                } while (choice != 8 && choice != 9);
            } else {
                cout << "Invalid credentials. Access denied." << endl;
            }
        } else if (choice == 2) {
            library.registerUser();
        } else if (choice == 3) {
            cout << "Exiting library system. Thanks for visiting!" << endl;
            return 0;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}

