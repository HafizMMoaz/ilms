#include <iostream>
#include <windows.h>
#include <conio.h>
#include<ios> //used to get stream size
#include<limits> //used to get numeric limits
using namespace std;
void gotoxy(int, int);                //~ controlling cursor position on console screen
void screenSetup(int, int, int, int); //~ handeling console screen
void setColor(int);                   //~ handling coloured ui on console screen

/* screens */
bool loginScreen(int, int);       //~ x-coord, y-coord
bool splashScreen(int, int);      //~ x-coord, y-coord
bool mainScreen(int, int);        //~ x-coord, y-coord

//! pages
bool dashboardPage(int, int);     //~ x-coord, y-coord
bool specimenPage(int, int);      //~ x-coord, y-coord
bool labDepartmentPage(int, int); //~ x-coord, y-coord

/* structural functionalities */
bool title(int, int);
bool yesNoPopup(int, int);                       //~ x-chord , y-chord
void removePopup(int, int);                      //~ x-chord, y-chord
void menu(string[], string, int, int, int, int); //~ menu choices array , menu heading , no. of choices, choice, x-coord, y-coord
int isLogin(string, string);
bool clear(int, int, int, int);
bool viewData(string, int, int);              //~ page Name , x-coord, y-coord
void sideBars(string, string[], int, int);    //~ page Name , menu name
void contentMenu(string, string[], int, int); //~ page Name , menu name

/* session */
bool isSessionStated = false;
bool sessionStart(int, string, string, string); //~ userID, Fullname , username , role
bool sessionEnd();
string session(string);
string _SESSION[4];

string _ACTIVE_SCREEN = "SPLASH", _ACTIVE_PAGE = "DASHBOARD", _ACTIVE_CONTROL = "SIDEBAR";
const int dataSize = 3;

/* users data */
int userId[dataSize] = {10001, 10002, 10003};
string userFname[dataSize] = {"Hafiz", "Muhammad", "Moaz"}, userName[dataSize] = {"hafiz", "muhammad", "moaz"}, userEmail[dataSize],
       userPassword[dataSize] = {"1234", "5678", "1452"}, userPhone[dataSize], userAddress[dataSize], userRole[dataSize] = {"R000", "R007", "R009"}, userLocation[dataSize];
bool userIsActive[dataSize] = {true, true, true};

/* specimen */
string specimenId[dataSize], specimenName[dataSize], specimenDescription[dataSize];

main()
{
    screenSetup(105, 40, 120, 40);
    /* roles */
    string roles_id[11] = {"R000", "R001", "R002", "R003", "R004", "R005", "R006", "R007", "R008", "R009", "R010"};
    string roles_name[11] = {"Receptionist", "Phelbotomist", "Technician", "Companies & Doctors", "Courier", "Home Sampling", "Collection Center", "Manager", "Admin", "Super Admin"};

    /* menues */
    string splashMenu[2] = {"\033[4mL\033[0mOGIN", "\033[4mE\033[0mXIT"};
    string sideBar[10] = {"\033[4mD\033[0mASHBOARD", "\033[4mS\033[0mETUP", "\033[4mP\033[0mATIENT", "\033[4mC\033[0mOORPARATE", "\033[4mT\033[0mEST", "\033[4mW\033[0mORKSHEET", "\033[4mH\033[0mOME SAMPLING", "C\033[4mO\033[0mURIER", "\033[4mF\033[0mINANCE", "\033[4mR\033[0mEPORTS"};
    string setupMenu[8] = {"\033[4mS\033[0mpecimen", "\033[4mL\033[0mab Departments", "Lab \033[4mT\033[0mests", "\033[4mP\033[0mackages", "Test \033[4mR\033[0mate List", "\033[4mM\033[0machines", "S\033[4mO\033[0mPs", "\033[4mB\033[0mack"};
    string innerContent[3] = {"\033[4mA\033[0mDD New", "\033[4mE\033[0mDIT", "\033[4mD\033[0mELETE"};

    int option = 0, size = 0;

    while (true)
    {
        if (_ACTIVE_SCREEN == "SPLASH")
        {
            if (splashScreen(0, 0))
            {
                size = sizeof(splashMenu) / sizeof(splashMenu[0]);
                menu(splashMenu, "Splash", size, option, 55, 26);
                while (true)
                {
                    if (GetAsyncKeyState(VK_DOWN))
                    {
                        if (option < size - 1)
                        {
                            option++;
                        }
                    }
                    else if (GetAsyncKeyState(VK_UP))
                    {
                        if (option > 0)
                        {
                            option--;
                        }
                    }
                    else if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('L'))
                    {
                        option = 0;
                    }
                    else if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('E'))
                    {
                        option = 1;
                    }
                    else if (GetAsyncKeyState(VK_SPACE))
                    {
                        if (option == 0)
                        {
                            _ACTIVE_SCREEN = "LOGIN";
                            Sleep(100);
                            break;
                        }
                        else if (option == 1)
                        {
                            if (yesNoPopup(81, 33))
                                exit(0);
                            else
                            {
                                removePopup(81, 33);
                                continue;
                            }
                        }
                    }
                    menu(splashMenu, "Splash", size, option, 55, 26);
                    Sleep(200);
                }
            }
        }
        else if (_ACTIVE_SCREEN == "LOGIN")
        {
            if (loginScreen(0, 0))
            {
                string username, password;
                gotoxy(55, 24); cout << "LOGIN";
                gotoxy(40, 27); cout << "Enter your username  :";
                gotoxy(65, 27);
                cin.ignore();
                cin >> username;
                gotoxy(40, 29);  cout << "Enter your password  :";
                gotoxy(65, 29);
                cin >> password;

                int userLocation = isLogin(username, password);
                if (userLocation >= 0)
                {
                    int userID = userId[userLocation];
                    string fullname = userFname[userLocation];
                    string username = userName[userLocation];
                    string role = userRole[userLocation];
                    if (sessionStart(userID, fullname, username, role))
                    {
                        _ACTIVE_SCREEN = "MAIN";
                        continue;
                    }
                }
                gotoxy(40, 31); cout << "Invalid Credentials !\b";
                Sleep(1000);
            }
        }
        else if (_ACTIVE_SCREEN == "MAIN")
        {
            if (_ACTIVE_PAGE == "DASHBOARD")
            {
                if (dashboardPage(31, 4))
                {
                    if (clear(2, 3, 24, 37))
                    {
                        option = 0;
                        size = sizeof(sideBar) / sizeof(sideBar[0]);
                        menu(sideBar, "SideBar", size, option, 2, 3);
                        while (true)
                        {
                            if (GetAsyncKeyState(VK_DOWN))
                            {
                                if (option < size - 1)
                                    option++;
                            }
                            else if (GetAsyncKeyState(VK_UP))
                            {
                                if (option > 0)
                                    option--;
                            }
                            else if (GetAsyncKeyState('D') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 0;
                            }
                            else if (GetAsyncKeyState('S') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 1;
                            }
                            else if (GetAsyncKeyState('P') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 2;
                            }
                            else if (GetAsyncKeyState('C') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 3;
                            }
                            else if (GetAsyncKeyState('T') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 4;
                            }
                            else if (GetAsyncKeyState('W') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 5;
                            }
                            else if (GetAsyncKeyState('H') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 6;
                            }
                            else if (GetAsyncKeyState('O') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 7;
                            }
                            else if (GetAsyncKeyState('F') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 8;
                            }
                            else if (GetAsyncKeyState('R') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 9;
                            }
                            else if (GetAsyncKeyState(VK_SPACE))
                            {
                                if (option == 0)
                                {
                                    _ACTIVE_PAGE = "DASHBOARD";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 1)
                                {
                                    _ACTIVE_PAGE = "SPECIMEN";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 2)
                                {
                                    _ACTIVE_PAGE = "PATIENT";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 3)
                                {
                                    _ACTIVE_PAGE = "COORPRATE";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 4)
                                {
                                    _ACTIVE_PAGE = "TEST";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 5)
                                {
                                    _ACTIVE_PAGE = "WORKSHEET";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 6)
                                {
                                    _ACTIVE_PAGE = "HOME SAMPLING";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 7)
                                {
                                    _ACTIVE_PAGE = "COURIER";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 8)
                                {
                                    _ACTIVE_PAGE = "FINANCE";
                                    Sleep(100);
                                    break;
                                }
                                else if (option == 9)
                                {
                                    _ACTIVE_PAGE = "REPORTS";
                                    Sleep(100);
                                    break;
                                }
                            }
                            menu(sideBar, "SideBar", size, option, 2, 3);
                            Sleep(200);
                        }
                    }
                }
            }
            else if (_ACTIVE_PAGE == "SPECIMEN")
            {
                if (clear(2, 3, 24, 37) && specimenPage(27, 4))
                {
                    option = 0;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("SPECIMEN", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        option = 0;
                        size = sizeof(innerContent) / sizeof(innerContent[0]);
                        contentMenu("Specimen", innerContent, option, size);
                    }
                }
            }
            else if (_ACTIVE_PAGE == "LABDEPARTMENT")
            {
                if (clear(2, 3, 24, 37) && labDepartmentPage(27, 4))
                {
                    option = 1;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("LABDEPARTMENT", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        option = 0;
                        size = sizeof(innerContent) / sizeof(innerContent[0]);
                        contentMenu("Lab Department", innerContent, option, size);
                    }
                }
            }
            else
            {
                system("cls");
                gotoxy(2, 3); cout << "Page not found";
                Sleep(500);
                gotoxy(2, 4); cout << "Redirecting to DASHBOARD... \1";
                _ACTIVE_PAGE = "DASHBOARD";
                Sleep(1500);
            }
        }
    }
}
void menu(string menu[], string heading, int size, int option, int x, int y)
{
    char op[size];

    for (int i = 0; i < size; i++)
    {
        if (option == i)
            op[i] = '+';
        else
            op[i] = ' ';
    }

    if (heading == "Splash")
    {
        for (int i = 0; i < size; i++)
        {
            gotoxy(x, y); 
            cout << "[" << op[i] << "] " << menu[i];
            y = y + 2;
        }
    }
    else if (heading == "yesNoPopup")
    {
        gotoxy(x, y);
        cout << "|    [" << op[0] << "] " << menu[0] << "              " << "[" << op[1] << "] " << menu[1] << "    |";
    }
    else if (heading == "SideBar")
    {
        for (int i = 0; i < size; i++)
        {
            y += 2;
            gotoxy(x, y);
            cout << " [" << op[i] << "] " << menu[i];
        }
    }
    else if (heading == "SETUP")
    {
        y += 2;
        gotoxy(x + 8, y);
        cout << "\033[4m" << heading << "\033[0m";
        for (int i = 0; i < size; i++)
        {
            y += 2;
            gotoxy(x, y);
            cout << " [" << op[i] << "] " << menu[i];
        }
    }
    else if (heading == "Lab Department" || heading == "Specimen")
    {
        for (int i = 0; i < size; i++)
        {
            gotoxy(x, y);
            cout << "[" << op[i] << "] " << menu[i] << " " << heading;
            x += 30;
        }
    }
}
void sideBars(string pageName, string subMenu[], int option, int size)
{
    menu(subMenu, "SETUP", size, option, 2, 3);
    while (_ACTIVE_CONTROL != "CONTENT")
    {
        if (GetAsyncKeyState(VK_DOWN))
        {
            if (option < size - 1)
                option++;
        }
        else if (GetAsyncKeyState(VK_UP))
        {
            if (option > 0)
                option--;
        }
        else if (GetAsyncKeyState('S') && GetAsyncKeyState(VK_MENU))
        {
            option = 0;
        }
        else if (GetAsyncKeyState('L') && GetAsyncKeyState(VK_MENU))
        {
            option = 1;
        }
        else if (GetAsyncKeyState('T') && GetAsyncKeyState(VK_MENU))
        {
            option = 2;
        }
        else if (GetAsyncKeyState('P') && GetAsyncKeyState(VK_MENU))
        {
            option = 3;
        }
        else if (GetAsyncKeyState('R') && GetAsyncKeyState(VK_MENU))
        {
            option = 4;
        }
        else if (GetAsyncKeyState('M') && GetAsyncKeyState(VK_MENU))
        {
            option = 5;
        }
        else if (GetAsyncKeyState('O') && GetAsyncKeyState(VK_MENU))
        {
            option = 6;
        }
        else if (GetAsyncKeyState('B') && GetAsyncKeyState(VK_MENU))
        {
            option = 7;
        }
        else if (GetAsyncKeyState(VK_RIGHT))
        {
            _ACTIVE_CONTROL = "CONTENT";
            if (_ACTIVE_PAGE == "SPECIMEN")
                menu(subMenu, "SETUP", size, 0, 2, 3);
            else if (_ACTIVE_PAGE == "LABDEPARTMENT")
                menu(subMenu, "SETUP", size, 1, 2, 3);
            else if (_ACTIVE_PAGE == "LABTEST")
                menu(subMenu, "SETUP", size, 2, 2, 3);
            else if (_ACTIVE_PAGE == "PACKAGES")
                menu(subMenu, "SETUP", size, 3, 2, 3);
            else if (_ACTIVE_PAGE == "TESTRATELIST")
                menu(subMenu, "SETUP", size, 4, 2, 3);
            else if (_ACTIVE_PAGE == "MACHINES")
                menu(subMenu, "SETUP", size, 5, 2, 3);
            else if (_ACTIVE_PAGE == "SOPS")
                menu(subMenu, "SETUP", size, 6, 2, 3);
            Sleep(100);
            break;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            if (option == 0)
            {
                _ACTIVE_PAGE = "SPECIMEN";
                Sleep(100);
                break;
            }
            if (option == 1)
            {
                _ACTIVE_PAGE = "LABDEPARTMENT";
                Sleep(100);
                break;
            }
            if (option == 2)
            {
                _ACTIVE_PAGE = "LABTEST";
                Sleep(100);
                break;
            }
            if (option == 3)
            {
                _ACTIVE_PAGE = "PACKAGES";
                Sleep(100);
                break;
            }
            if (option == 4)
            {
                _ACTIVE_PAGE = "TESTRATELIST";
                Sleep(100);
                break;
            }
            if (option == 5)
            {
                _ACTIVE_PAGE = "MACHINES";
                Sleep(100);
                break;
            }
            if (option == 6)
            {
                _ACTIVE_PAGE = "SOPS";
                Sleep(100);
                break;
            }
            if (option == 7)
            {
                _ACTIVE_PAGE = "DASHBOARD";
                Sleep(100);
                break;
            }
        }
        menu(subMenu, "SETUP", size, option, 2, 3);
        Sleep(200);
    }
}
void contentMenu(string pageName, string subMenu[], int option, int size)
{
    menu(subMenu, pageName, size, option, 27, 4);
    while (true)
    {
        if (GetAsyncKeyState(VK_DOWN))
        {
            if (option < size - 1)
                option++;
        }
        else if (GetAsyncKeyState(VK_UP))
        {
            if (option > 0)
                option--;
        }
        else if (GetAsyncKeyState('A') && GetAsyncKeyState(VK_MENU))
        {
            option = 0;
        }
        else if (GetAsyncKeyState('E') && GetAsyncKeyState(VK_MENU))
        {
            option = 1;
        }
        else if (GetAsyncKeyState('D') && GetAsyncKeyState(VK_MENU))
        {
            option = 2;
        }
        else if (GetAsyncKeyState(VK_LEFT))
        {
            _ACTIVE_CONTROL = "SIDEBAR";
            break;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
        }
        menu(subMenu, pageName, size, option, 27, 4);
        Sleep(200);
    }
}
bool mainScreen(int x, int y)
{
    system("cls");
    gotoxy(x, y); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 1); cout<<"|          ILMS          |                                                                                             |";
    gotoxy(x, y + 2); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 3); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 4); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 5); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 6); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 7); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 8); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 9); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 10); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 11); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 12); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 13); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 14); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 15); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 16); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 17); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 18); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 19); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 20); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 21); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 22); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 23); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 24); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 25); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 26); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 27); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 28); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 29); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 30); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 31); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 32); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 33); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 34); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 35); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 36); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 37); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 38); cout<<"|                        |                                                                                             |";
    gotoxy(x, y + 39); cout<<"------------------------------------------------------------------------------------------------------------------------";

    return true;
}
bool dashboardPage(int x, int y)
{
    if (mainScreen(0, 0))
    {
        gotoxy(x - 4, y); cout<<"WELCOME! \1 " << session("fname");
        gotoxy(x - 4, y + 1); cout<<"------------------------------------------------";

        gotoxy(x, y + 3);  cout<<"---------------------------";
        gotoxy(x, y + 4);  cout<<"|       CASH SUMMARY      |";
        gotoxy(x, y + 5);  cout<<"---------------------------";
        gotoxy(x, y + 6);  cout<<"|          TODAY's        |";
        gotoxy(x, y + 7);  cout<<"|    -----------------    |";
        gotoxy(x, y + 8);  cout<<"|  Patient's : ________   |";
        gotoxy(x, y + 9);  cout<<"|       Paid : ________   |";
        gotoxy(x, y + 10); cout<<"|    Balance : ________   |";
        gotoxy(x, y + 11); cout<<"|   Expenses : ________   |";
        gotoxy(x, y + 12); cout<<"|-------------------------|";
        gotoxy(x, y + 13); cout<<"|           TOTAL         |";
        gotoxy(x, y + 14); cout<<"|    -----------------    |";
        gotoxy(x, y + 15); cout<<"|  Patient's : ________   |";
        gotoxy(x, y + 16); cout<<"|       Paid : ________   |";
        gotoxy(x, y + 17); cout<<"|    Balance : ________   |";
        gotoxy(x, y + 18); cout<<"|   Expenses : ________   |";
        gotoxy(x, y + 19); cout<<"---------------------------";

        gotoxy(x + 55, y + 3);  cout<<"---------------------------";
        gotoxy(x + 55, y + 4);  cout<<"|     PATIENT SUMMARY     |";
        gotoxy(x + 55, y + 5);  cout<<"---------------------------";
        gotoxy(x + 55, y + 6);  cout<<"|          TODAY's        |";
        gotoxy(x + 55, y + 7);  cout<<"|    -----------------    |";
        gotoxy(x + 55, y + 8);  cout<<"|      Patients : ______  |";
        gotoxy(x + 55, y + 9);  cout<<"|         Tests : ______  |";
        gotoxy(x + 55, y + 10); cout<<"|  Home Samples : ______  |";
        gotoxy(x + 55, y + 11); cout<<"|           C-C : ______  |";
        gotoxy(x + 55, y + 12); cout<<"|       Doctors : ______  |";
        gotoxy(x + 55, y + 13); cout<<"|-------------------------|";
        gotoxy(x + 55, y + 14); cout<<"|           TOTAL         |";
        gotoxy(x + 55, y + 15); cout<<"|    -----------------    |";
        gotoxy(x + 55, y + 16); cout<<"|      Patients : ______  |";
        gotoxy(x + 55, y + 17); cout<<"|         Tests : ______  |";
        gotoxy(x + 55, y + 18); cout<<"|  Home Samples : ______  |";
        gotoxy(x + 55, y + 19); cout<<"|           C-C : ______  |";
        gotoxy(x + 55, y + 20); cout<<"|       Doctors : ______  |";
        gotoxy(x + 55, y + 21); cout<<"---------------------------";

        gotoxy(x + 10, y + 22); cout<<"---------------------------";
        gotoxy(x + 10, y + 23); cout<<"|         REVENUE         |";
        gotoxy(x + 10, y + 24); cout<<"---------------------------";
        gotoxy(x + 10, y + 25); cout<<"|          TODAY's        |";
        gotoxy(x + 10, y + 26); cout<<"|    -----------------    |";
        gotoxy(x + 10, y + 27); cout<<"|    Revenue : ________   |";
        gotoxy(x + 10, y + 28); cout<<"|-------------------------|";
        gotoxy(x + 10, y + 29); cout<<"|           TOTAL         |";
        gotoxy(x + 10, y + 30); cout<<"|    -----------------    |";
        gotoxy(x + 10, y + 31); cout<<"|    Revenue : ________   |";
        gotoxy(x + 10, y + 32); cout<<"---------------------------";

        gotoxy(x + 45, y + 23); cout<<"---------------------------";
        gotoxy(x + 45, y + 24); cout<<"|          OTHERS         |";
        gotoxy(x + 45, y + 25); cout<<"---------------------------";
        gotoxy(x + 45, y + 26); cout<<"|       R-R : ________    |";
        gotoxy(x + 45, y + 27); cout<<"|       C-C : ________    |";
        gotoxy(x + 45, y + 28); cout<<"|       H-S : ________    |";
        gotoxy(x + 45, y + 29); cout<<"|   Doctors : ________    |";
        gotoxy(x + 45, y + 30); cout<<"|  Couriers : ________    |";
        gotoxy(x + 45, y + 31); cout<<"| Technicians : ________  |";
        gotoxy(x + 45, y + 32); cout<<"| Phelbotomist : ________ |";
        gotoxy(x + 45, y + 33); cout<<"|   Manager : ________    |";
        gotoxy(x + 45, y + 34); cout<<"---------------------------";
    }
    return true;
}
bool specimenPage(int x, int y)
{
    if (mainScreen(0, 0))
    {
        gotoxy(x, y); cout<<"[ ] ADD New Specimen";
        gotoxy(x + 30, y); cout<<"[ ] EDIT Specimen";
        gotoxy(x + 60, y); cout<<"[ ] DELETE Specimen";
        y++;
        gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
        gotoxy(x, y + 2); cout<<"| Sr |     ID     |              NAME              |              DESCRIPTION             |";
        gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
    }
    return true;
}
bool labDepartmentPage(int x, int y)
{
    if (mainScreen(0, 0))
    {
        gotoxy(x, y); cout<<"[ ] ADD New Lab Department";
        gotoxy(x + 30, y); cout<<"[ ] EDIT Lab Department";
        gotoxy(x + 60, y); cout<<"[ ] DELETE Lab Department";
        y++;
        gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
        gotoxy(x, y + 2); cout<<"| Sr |             NAME             |              DESCRIPTION             |      C-B     |";
        gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
    }
    return true;
}
bool splashScreen(int x, int y)
{
    system("cls");
    gotoxy(x, y); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 1); cout<<"########################################################################################################################";
    gotoxy(x, y + 2); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 3); cout<<".                                                                                                                      .";
    gotoxy(x, y + 4); cout<<".                                                                                                                      .";
    gotoxy(x, y + 5); cout<<".                                                                                                                      .";
    gotoxy(x, y + 6); cout<<".                                                                                                                      .";
    gotoxy(x, y + 7); cout<<".                                                                                                                      .";
    gotoxy(x, y + 8); cout<<".                                                                                                                      .";
    gotoxy(x, y + 9); cout<<".                                                                                                                      .";
    gotoxy(x, y + 10); cout<<".                                                                                                                      .";
    gotoxy(x, y + 11); cout<<".                                                                                                                      .";
    gotoxy(x, y + 12); cout<<".                                                                                                                      .";
    gotoxy(x, y + 13); cout<<".                                                                                                                      .";
    gotoxy(x, y + 14); cout<<".                                                                                                                      .";
    gotoxy(x, y + 15); cout<<".                                                                                                                      .";
    gotoxy(x, y + 16); cout<<".                                                                                                                      .";
    gotoxy(x, y + 17); cout<<".                                                                                                                      .";
    gotoxy(x, y + 18); cout<<".                                                                                                                      .";
    gotoxy(x, y + 19); cout<<".                                                                                                                      .";
    gotoxy(x, y + 20); cout<<".                                                                                                                      .";
    gotoxy(x, y + 21); cout<<".                                                                                                                      .";
    gotoxy(x, y + 22); cout<<".                                                                                                                      .";
    gotoxy(x, y + 23); cout<<".                                                                                                                      .";
    gotoxy(x, y + 24); cout<<".                                                                                                                      .";
    gotoxy(x, y + 25); cout<<".                                                                                                                      .";
    gotoxy(x, y + 26); cout<<".                                                                                                                      .";
    gotoxy(x, y + 27); cout<<".                                                                                                                      .";
    gotoxy(x, y + 28); cout<<".                                                                                                                      .";
    gotoxy(x, y + 29); cout<<".                                                                                                                      .";
    gotoxy(x, y + 30); cout<<".                                                                                                                      .";
    gotoxy(x, y + 31); cout<<".                                                                                                                      .";
    gotoxy(x, y + 32); cout<<".                                                                                                                      .";
    gotoxy(x, y + 33); cout<<".                                                                                                                      .";
    gotoxy(x, y + 34); cout<<".                                                                                                                      .";
    gotoxy(x, y + 35); cout<<".                                                                                                                      .";
    gotoxy(x, y + 36); cout<<".                                                                                                                      .";
    gotoxy(x, y + 37); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 38); cout<<"########################################################################################################################";
    gotoxy(x, y + 39); cout<<"------------------------------------------------------------------------------------------------------------------------";

    if (title(40, y + 8))
        return true;
    return false;
}
bool loginScreen(int x, int y)
{
    system("cls");
    gotoxy(x, y); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 1); cout<<"########################################################################################################################";
    gotoxy(x, y + 2); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 3); cout<<".                                                                                                                      .";
    gotoxy(x, y + 4); cout<<".                                                                                                                      .";
    gotoxy(x, y + 5); cout<<".                                                                                                                      .";
    gotoxy(x, y + 6); cout<<".                                                                                                                      .";
    gotoxy(x, y + 7); cout<<".                                                                                                                      .";
    gotoxy(x, y + 8); cout<<".                                                                                                                      .";
    gotoxy(x, y + 9); cout<<".                                                                                                                      .";
    gotoxy(x, y + 10); cout<<".                                                                                                                      .";
    gotoxy(x, y + 11); cout<<".                                                                                                                      .";
    gotoxy(x, y + 12); cout<<".                                                                                                                      .";
    gotoxy(x, y + 13); cout<<".                                                                                                                      .";
    gotoxy(x, y + 14); cout<<".                                                                                                                      .";
    gotoxy(x, y + 15); cout<<".                                                                                                                      .";
    gotoxy(x, y + 16); cout<<".                                                                                                                      .";
    gotoxy(x, y + 17); cout<<".                                                                                                                      .";
    gotoxy(x, y + 18); cout<<".                                                                                                                      .";
    gotoxy(x, y + 19); cout<<".                                                                                                                      .";
    gotoxy(x, y + 20); cout<<".                                                                                                                      .";
    gotoxy(x, y + 21); cout<<".                                                                                                                      .";
    gotoxy(x, y + 22); cout<<".                                                                                                                      .";
    gotoxy(x, y + 23); cout<<".                                                                                                                      .";
    gotoxy(x, y + 24); cout<<".                                                                                                                      .";
    gotoxy(x, y + 25); cout<<".                                                                                                                      .";
    gotoxy(x, y + 26); cout<<".                                                                                                                      .";
    gotoxy(x, y + 27); cout<<".                                                                                                                      .";
    gotoxy(x, y + 28); cout<<".                                                                                                                      .";
    gotoxy(x, y + 29); cout<<".                                                                                                                      .";
    gotoxy(x, y + 30); cout<<".                                                                                                                      .";
    gotoxy(x, y + 31); cout<<".                                                                                                                      .";
    gotoxy(x, y + 32); cout<<".                                                                                                                      .";
    gotoxy(x, y + 33); cout<<".                                                                                                                      .";
    gotoxy(x, y + 34); cout<<".                                                                                                                      .";
    gotoxy(x, y + 35); cout<<".                                                                                                                      .";
    gotoxy(x, y + 36); cout<<".                                                                                                                      .";
    gotoxy(x, y + 37); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 38); cout<<"########################################################################################################################";
    gotoxy(x, y + 39); cout<<"------------------------------------------------------------------------------------------------------------------------";

    if (title(40, y + 8))
        return true;
    return false;
}
int isLogin(string username, string password)
{
    for (int index = 0; index < dataSize; index++)
    {
        if (userName[index] == username && userPassword[index] == password)
        {
            if (userIsActive[index])
            {
                return index;
            }
        }
    }
    return -1;
}
bool sessionStart(int userID, string fname, string username, string role)
{
    if (!isSessionStated)
    {
        _SESSION[0] = to_string(userID);
        _SESSION[1] = fname;
        _SESSION[2] = username;
        _SESSION[3] = role;
        isSessionStated = true;
        return true;
    }
    return false;
}
string session(string key)
{
    if (isSessionStated)
    {
        int index;
        if (key == "id")
            index = 0;
        else if (key == "fname")
            index = 1;
        else if (key == "uname")
            index = 2;
        else if (key == "role")
            index = 3;
        else
            return "Invalid Key";
        return _SESSION[index];
    }
    return "No session started";
}
bool sessionEnd()
{
    if (isSessionStated)
    {
        for (int i = 0; i < 4; i++)
            _SESSION[i] = "";
        isSessionStated = false;
        return true;
    }
    return false;
}
bool title(int x, int y)
{
    gotoxy(x, y); cout<<"-------------------------------------";
    gotoxy(x, y + 1); cout<<"****** **       **     **   *******  ";
    gotoxy(x, y + 2); cout<<"  **   **       ***   ***  ********* ";
    gotoxy(x, y + 3); cout<<"  **   **       ** * * ** *          ";
    gotoxy(x, y + 4); cout<<"  **   **       **  *  **  ********  ";
    gotoxy(x, y + 5); cout<<"  **   **       **     **          * ";
    gotoxy(x, y + 6); cout<<"  **   ******** **     ** *********  ";
    gotoxy(x, y + 7); cout<<"****** ******** **     **  *******   ";
    gotoxy(x, y + 8); cout<<"-------------------------------------";
    gotoxy(x, y + 9); cout<<"   INTEGRATED LAB MANGEMENT SYSTEM   ";
    gotoxy(x, y + 10); cout<<"-------------------------------------";

    return true;
}
bool yesNoPopup(int x, int y)
{
    int option = 0, size;
    string options[2] = {"\033[4mY\033[0mES", "\033[4mN\033[0mO"};
    gotoxy(x, y); cout<<"-------------------------------------";
    gotoxy(x, y + 1); cout<<"|            Are You Sure?          |";
    gotoxy(x, y + 3); cout<<"-------------------------------------";

    size = sizeof(options) / sizeof(options[0]);
    menu(options, "yesNoPopup", size, option, x, y + 2);
    while (true)
    {
        if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('N'))
        {
            if (option == 1 && GetAsyncKeyState('N'))
                return false;
            option = 1;
        }
        else if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('Y'))
        {
            if (option == 0 && GetAsyncKeyState('Y'))
                return true;
            option = 0;
        }
        menu(options, "yesNoPopup", size, option, x, y + 2);
        Sleep(200);
    }
}
void removePopup(int x, int y)
{
    gotoxy(x, y); cout<<"                                     ";
    gotoxy(x, y + 1); cout<<"                                     ";
    gotoxy(x, y + 2); cout<<"                                     ";
    gotoxy(x, y + 3); cout<<"                                     ";
}
bool clear(int startX, int startY, int endX, int endY)
{
    for (int i = startY; i <= endY; i++)
    {
        gotoxy(startX, i);
        for (int j = startX; j <= endX; j++)
        {
            cout << " ";
        }
    }

    return true;
}
void gotoxy(int x, int y)
{
    COORD coords;
    coords.X = x;
    coords.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coords);
}
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void screenSetup(int bufferX, int bufferY, int width, int height)
{
    COORD bufferSize;
    bufferSize.X = bufferX;
    bufferSize.Y = bufferY;

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1;
    windowSize.Bottom = height - 1;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);

    RECT rect;
    GetWindowRect(GetConsoleWindow(), &rect);
    int winWidth = rect.right - rect.left;
    int winHeight = rect.bottom - rect.top;

    SetWindowPos(GetConsoleWindow(), 0, 250, 100, winWidth, winHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}