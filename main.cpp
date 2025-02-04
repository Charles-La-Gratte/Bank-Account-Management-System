#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <regex>

using namespace std;
//Creation of the class Account
class Account
{
private:
    string accountNo;
    string password;
    double balance;

public:
//constructor to initialize all the attribut
    Account(string id = "", string pwd = "", double bal = 0.0)
        : accountNo(id), password(pwd), balance(bal) {}
//Getter
    string getAccountNo() const { return accountNo; }
    string getPassword() const { return password; }
    double getBalance() const { return balance; }
//Setter
    void setPassword(const string &pwd) { password = pwd; }
//Function for the deposit
    void deposit(double amount)
    {
        balance += amount;
    }
//Function for the withdraw
    bool withdraw(double amount)
    {
        if (amount <= balance)
        {
            balance -= amount;
            return true;
        }
        return false;
    }
//Function to print the account informations
    string toString() const
    {
        return accountNo + " " + password + " " + to_string(balance);
    }
};

//Creation of the Class Bank
class Bank
{
private:
    vector<Account> accounts;
//Initialize FILENAME
    const string FILENAME = "accounts.txt";
/*
Function to verify whether accNo matches a specific pattern for validity.
In this case, the account number must consist of exactly six digits.
accNo cannot contain alphabet or special characters.
*/
    bool isValidAccountNumber(const string &accNo)
    {
        regex pattern("^[0-9]{6}$");
        return regex_match(accNo, pattern);
    }
//Function to determine whether the password pwd is at least 6 characters long.
    bool isValidPassword(const string &pwd)
    {
        return pwd.length() >= 6;
    }
//Method to load account information from a file named FILENAME.
    void loadAccounts()
    {
        ifstream infile(FILENAME);
        if (!infile)
        {
            ofstream createFile(FILENAME);
            createFile.close();
            return;
        }

        string line;
        while (getline(infile, line))
        {
            stringstream ss(line);
            string id, pwd;
            double bal;
            ss >> id >> pwd >> bal;
            accounts.emplace_back(id, pwd, bal);
        }
    }
//Method to save account infos into FILENAME
    void saveAccounts()
    {
        //Open the file for writting
        ofstream outfile(FILENAME);
        for (const auto &account : accounts)
        {
            //Write each account to the file
            outfile << account.toString() << endl;
        }
    }
//Function to check if the account already exist (primary key)
    bool accountExists(const string &id)
    {
      return find_if(accounts.begin(), accounts.end(),
      [&](const Account &acc)
      { return acc.getAccountNo() == id; }) != accounts.end();
    }

public:
    Bank() { loadAccounts(); }
//Method to add a new account
    void addAccount(const string &id, const string &pwd)
    {
        if (!isValidAccountNumber(id))
        {
            cout << "Invalid account number. Must be 6 digits." << endl;
            return;
        }
        if (!isValidPassword(pwd))
        {
            cout << "Invalid password. Must be at least 6 characters." << endl;
            return;
        }
        if (accountExists(id))
        {
            cout << "Account number already exists." << endl;
            return;
        }

        accounts.emplace_back(id, pwd);
        saveAccounts();
        cout << "Account created successfully!" << endl;
    }
//This function is used to find a specific account
    Account *findAccount(const string &id, const string &pwd)
    {
        for (auto &account : accounts)
        {
            if (account.getAccountNo() == id && account.getPassword() == pwd)
            {
                return &account;
            }
        }
        return nullptr;
    }
//Method for view a specific account
    void viewAccount(const string &id)
    {
        for (const auto &account : accounts)
        {
            if (account.getAccountNo() == id)
            {
                cout << "Account No: " << account.getAccountNo()
                     << ", Balance: $" << fixed << account.getBalance() << endl;
                return;
            }
        }
        cout << "Account not found." << endl;
    }
//Method for the deposit money into the account
    void depositAmount(const string &id, double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid amount. Must be greater than 0." << endl;
            return;
        }

        for (auto &account : accounts)
        {
            if (account.getAccountNo() == id)
            {
                account.deposit(amount);
                saveAccounts();
                cout << "Deposit successful! New balance: $" << account.getBalance() << endl;
                return;
            }
        }
        cout << "Account not found." << endl;
    }
//Method for withdraw from the account
    void withdrawAmount(const string &id, const string &pwd, double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid amount. Must be greater than 0." << endl;
            return;
        }

        Account *account = findAccount(id, pwd);
        if (account && account->withdraw(amount))
        {
            saveAccounts();
            cout << "Withdrawal successful! New balance: $" << account->getBalance() << endl;
        }
        else
        {
            cout << "Insufficient balance or account not found." << endl;
        }
    }
//Method to display all the account in the list 
    void viewAllAccounts()
    {
        if (accounts.empty())
        {
            cout << "No accounts exist." << endl;
            return;
        }
        for (const auto &account : accounts)
        {
            cout << "Account No: " << account.getAccountNo()
                 << ", Balance: $" << fixed << account.getBalance() << endl;
        }
    }
//Method for closed a specific account
    void closeAccount(const string &id, const string &pwd)
    {
        auto it = remove_if(accounts.begin(), accounts.end(),
                            [&](const Account &account)
                            {
                                return account.getAccountNo() == id && account.getPassword() == pwd;
                            });

        if (it != accounts.end())
        {
            accounts.erase(it, accounts.end());
            saveAccounts();
            cout << "Account closed successfully." << endl;
        }
        else
        {
            cout << "Account not found or incorrect password." << endl;
        }
    }
//Method for modify password of the specific account 
    void modifyAccount(const string &id, const string &oldPwd, const string &newPwd)
    {
        if (!isValidPassword(newPwd))
        {
            cout << "Invalid new password. Must be at least 6 characters." << endl;
            return;
        }

        Account *account = findAccount(id, oldPwd);
        if (account)
        {
            account->setPassword(newPwd);
            saveAccounts();
            cout << "Password changed successfully." << endl;
        }
        else
        {
            cout << "Account not found or incorrect password." << endl;
        }
    }
};
//This function clears such unwanted input from the buffer.
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
//Create a new object bank
    Bank bank;
    int choice;
//Menu
    while (true)
    {
        cout << "\n******************************" << endl;
        cout << "Bank Account Management System" << endl;
        cout << "******************************" << endl;
        cout << "1. Add New Account" << endl;
        cout << "2. View Account" << endl;
        cout << "3. Deposit Amount" << endl;
        cout << "4. Withdraw Amount" << endl;
        cout << "5. View All Accounts" << endl;
        cout << "6. Close An Account" << endl;
        cout << "7. Modify An Account" << endl;
        cout << "8. Exit" << endl;
        cout << "******************************" << endl;

        cout << "Enter your choice (1-8): ";
        if (!(cin >> choice))
        {
            clearInputBuffer();
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        clearInputBuffer();

        switch (choice)
        {
        case 1:
        {
            string id, pwd;
            cout << "Enter Account Number (6 digits): ";
            getline(cin, id);
            cout << "Enter Password (min 6 characters): ";
            getline(cin, pwd);
            bank.addAccount(id, pwd);
            break;
        }
        case 2:
        {
            string id;
            cout << "Enter Account Number: ";
            getline(cin, id);
            bank.viewAccount(id);
            break;
        }
        case 3:
        {
            string id;
            double amount;
            cout << "Enter Account Number: ";
            getline(cin, id);
            cout << "Enter Deposit Amount: ";
            if (cin >> amount)
            {
                bank.depositAmount(id, amount);
            }
            else
            {
                cout << "Invalid amount entered." << endl;
            }
            clearInputBuffer();
            break;
        }
        case 4:
        {
            string id, pwd;
            double amount;
            cout << "Enter Account Number: ";
            getline(cin, id);
            cout << "Enter Password: ";
            getline(cin, pwd);
            cout << "Enter Withdraw Amount: ";
            if (cin >> amount)
            {
                bank.withdrawAmount(id, pwd, amount);
            }
            else
            {
                cout << "Invalid amount entered." << endl;
            }
            clearInputBuffer();
            break;
        }
        case 5:
            bank.viewAllAccounts();
            break;
        case 6:
        {
            string id, pwd;
            cout << "Enter Account Number: ";
            getline(cin, id);
            cout << "Enter Password: ";
            getline(cin, pwd);
            bank.closeAccount(id, pwd);
            break;
        }
        case 7:
        {
            string id, oldPwd, newPwd;
            cout << "Enter Account Number: ";
            getline(cin, id);
            cout << "Enter Old Password: ";
            getline(cin, oldPwd);
            cout << "Enter New Password: ";
            getline(cin, newPwd);
            bank.modifyAccount(id, oldPwd, newPwd);
            break;
        }
        case 8:
            cout << "Thank you for banking with us!" << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}