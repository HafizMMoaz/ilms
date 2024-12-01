#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <cmath>
#include <ios>
#include <limits>
using namespace std;

//! global functions
string dateTime(string);                    //~ date or time or both
void gotoxy(int, int);                      //* controlling cursor position on console screen
void screenSetup(int, int, int, int);       //~ handeling console screen
void setColor(int);                         //* handling coloured ui on console screen

//! screens
bool loginScreen(int, int);                 //~ x-coord, y-coord
bool splashScreen(int, int);                //* x-coord, y-coord
bool mainScreen(int, int);                  //~ x-coord, y-coord

//! pages
bool dashboardPage(int, int);                                   //* x-coord, y-coord
bool specimenPage(int, int, string, string[], int, int);        //~ x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices
bool labDepartmentPage(int, int, string, string[], int, int);   //* x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices
bool labTestPage(int, int, string, string[], int, int);         //~ x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices
bool packagesPage(int, int, string, string[], int, int);        //* x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices
bool rateListPage(int, int);                                    //~ x-coord, y-coord
bool machinesPage(int, int, string, string[], int, int);        //* x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices
bool sopsPage(int, int, string, string[], int, int);            //~ x-coord, y-coord, menu heading  , menu choices array, choice , no. of choices

//! structural functionalities
bool title(int, int);
bool yesNoPopup(int, int);                                      //* x-chord , y-chord
void removePopup(int, int);                                     //~ x-chord, y-chord
void menu(string[], string, int, int, int, int);                //* menu choices array , menu heading , no. of choices, choice, x-coord, y-coord
int isLogin(string, string);                                    //~ username, password
bool clear(int, int, int, int);                                 //* x-start, y-start, x-end, y-end
bool viewData(string, int, int);                                //~ page Name , x-coord, y-coord
void sideBars(string, string[], int, int);                      //* page Name , menu name
void contentMenu(string, string[], int, int);                   //~ page Name , menu name
bool isValidate(string, string);
string labTestIDValid(string tests[], string id, int i){
    for(int x = 0; x < i ; x++)
    {
        if(id == tests[x]){
            return  "00000";
        }
    }
    return id;
}

//! session
bool isSessionStated = false;
bool sessionStart(string, string, string, string);              //* userID, Fullname , username , role
bool sessionEnd();
string session(string);
string _SESSION[4];

//& control variables
string _ACTIVE_SCREEN = "SPLASH", _ACTIVE_PAGE = "DASHBOARD", _ACTIVE_CONTROL = "SIDEBAR", _ACTIVE_ACTION = "VIEW", _LOGIN_TIME = "";

//& datasize that can be stored
const int dataSize = 3;

//^ users data
string userID[dataSize]= {"U000", "U001", "U002"}, userFname[dataSize] = {"Hafiz", "Muhammad", "Moaz"}, userName[dataSize] = {"hafiz", "muhammad", "moaz"}, userEmail[dataSize],
userPassword[dataSize] = {"1234", "5678", "1452"}, userPhone[dataSize], userAddress[dataSize], userRole[dataSize] = {"R000", "R007", "R009"}, userLocation[dataSize];
bool userIsActive[dataSize] = {true, true, true};

//^ specimen
string specimenID[dataSize], specimenName[dataSize], specimenDescription[dataSize];
int specimenCount = 0;

//^ lab department
string labDepartemtID[dataSize], labDepartmentName[dataSize], labDepartmentDate[dataSize];
int labDepartmentCount = 0;

//^ lab Test
string labTestID[dataSize], labTestName[dataSize], labTestRate[dataSize], labTestGroup[dataSize], labTestMachine[dataSize], labTestUnit[dataSize], labTestFreq[dataSize], labTestTime[dataSize], labTestComments[dataSize], labTestSpecimen[dataSize];
int labTestCount = 2;

//^ Machines
string machineID[dataSize], machineName[dataSize], machineDescription[dataSize], machineQuantity[dataSize];
int machineCount = 0;

//^ sops
string sopID[dataSize], sop[dataSize], sopDate[dataSize]; int sopCount = 0;

//^ packages
string packageID[dataSize], packageName[dataSize], packageTestCount[dataSize], packageTests[dataSize][5], packageRate[dataSize], packageDisc[dataSize];
int packageCount;

main()
{
    screenSetup(105, 40, 120, 40);

    labTestID[0] = "LT001"; labTestRate[0] = "250";
    labTestID[1] = "LT002"; labTestRate[1] = "300";

    //& roles
    string roles_id[11] = {"R000", "R001", "R002", "R003", "R004", "R005", "R006", "R007", "R008", "R009", "R010"};
    string roles_name[11] = {"Receptionist", "Phelbotomist", "Technician", "Companies & Doctors", "Courier", "Home Sampling", "Collection Center", "Manager", "Admin", "Super Admin"};

    //& menus
    string splashMenu[2] = {"\033[4mL\033[0mOGIN", "\033[4mE\033[0mXIT"};
    string sideBar[11] = {"\033[4mD\033[0mASHBOARD", "\033[4mS\033[0mETUP", "\033[4mP\033[0mATIENT", "\033[4mC\033[0mOORPARATE", "\033[4mT\033[0mEST", "\033[4mW\033[0mORKSHEET", "\033[4mH\033[0mOME SAMPLING", "C\033[4mO\033[0mURIER", "\033[4mF\033[0mINANCE", "\033[4mR\033[0mEPORTS", "\033[4mL\033[0mOGOUT"};
    string setupMenu[8] = {"\033[4mS\033[0mpecimen", "\033[4mL\033[0mab Departments", "Lab \033[4mT\033[0mests", "\033[4mP\033[0mackages", "Test \033[4mR\033[0mate List", "\033[4mM\033[0machines", "S\033[4mO\033[0mPs", "\033[4mB\033[0mack"};
    string innerContent[3] = {"\033[4mA\033[0mDD New", "\033[4mE\033[0mDIT", "\033[4mD\033[0mELETE"};

    int option = 0, size = 0;

    while (true)
    {
        if (_ACTIVE_SCREEN == "SPLASH")
        {
            if (splashScreen(0, 0))
            {
                option = 0;
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
                gotoxy(65, 27); cin >> username;
                while(!isValidate("username", username)){
                    gotoxy(65, 27); cout << "                                 ";
                    gotoxy(65, 27); cin >> username;
                }
                gotoxy(40, 29); cout << "Enter your password  :";
                gotoxy(65, 29); cin >> password;
                int userLocation = isLogin(username, password);
                if (userLocation >= 0)
                {
                    string userId = userID[userLocation];
                    string fullname = userFname[userLocation];
                    string username = userName[userLocation];
                    string role = userRole[userLocation];
                    if (sessionStart(userId, fullname, username, role))
                    {
                        _ACTIVE_SCREEN = "MAIN";
                        _ACTIVE_PAGE = "DASHBOARD";
                        _LOGIN_TIME = dateTime("date") + " " + dateTime("time");
                        continue;
                    }
                }
                gotoxy(40, 31); cout << "Invalid Credentials !";
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
                            else if (GetAsyncKeyState('L') && GetAsyncKeyState(VK_MENU))
                            {
                                option = 10;
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
                                else if (option == 10)
                                {
                                    _ACTIVE_PAGE = "LOGOUT";
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
                option = 0;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && specimenPage(27, 4, "SPECIMEN", setupMenu, option, size))
                {
                    sideBars("SPECIMEN", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT" )
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("Specimen", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another Specimen", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "EDIT")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mE\033[0mDIT Another Specimen", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "DELETE")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mD\033[0mELETE Another Specimen", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                }
            }
            else if (_ACTIVE_PAGE == "LABDEPARTMENT")
            {
                option = 1;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && labDepartmentPage(27, 4, "LAB DEPARTMENT", setupMenu, option, size))
                {
                    option = 1;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("LABDEPARTMENT", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("Lab Department", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD"){
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another Lab Department", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "DELETE")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mD\033[0mELETE Another Lab Department", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "EDIT")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mE\033[0mDIT Another Lab Department", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                }
            }
            else if (_ACTIVE_PAGE == "LABTEST")
            {
                option = 2;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && labTestPage(27, 4, "LABTEST", setupMenu, option, size))
                {
                    option = 2;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("LABTEST", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("Lab Test", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD"){
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another Lab Test", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "EDIT")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mE\033[0mDIT Another LABTEST", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "DELETE")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mD\033[0mELETE Another LABTEST", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true)
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                    
                }
            }
            else if (_ACTIVE_PAGE == "PACKAGES")
            {
                option = 3;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && packagesPage(27, 4, "PACKAGES", setupMenu, option, size))
                {
                    option = 3;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("PACKAGES", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("Packages", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD"){
                            option = 0;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another Packages", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                    
                }
            }
            else if (_ACTIVE_PAGE == "TESTRATELIST")
            {
                option = 4;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && rateListPage(27, 3))
                {
                    option = 4;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("TESTRATELIST", setupMenu, option, size);
                }
            }
            else if (_ACTIVE_PAGE == "MACHINES")
            {
                option = 5;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && machinesPage(27, 4, "MACHINES", setupMenu, option, size))
                {
                    option = 5;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("MACHINES", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("Machine", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another Machine", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "EDIT")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mE\033[0mDIT Another Machine", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "DELETE")
                        {
                            option = 1;
                            string innerMenuADD[2] = {"\033[4mD\033[0mELETE Another Machine", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                    
                }
            }
            else if (_ACTIVE_PAGE == "SOPS")
            {
                option = 6;
                size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                if (clear(2, 3, 24, 37) && sopsPage(27, 4, "SOPS", setupMenu, option, size))
                {
                    option = 6;
                    size = sizeof(setupMenu) / sizeof(setupMenu[0]);
                    sideBars("SOPS", setupMenu, option, size);
                    if (_ACTIVE_CONTROL == "CONTENT")
                    {
                        if (_ACTIVE_ACTION == "VIEW")
                        {
                            option = 0;
                            size = sizeof(innerContent) / sizeof(innerContent[0]);
                            contentMenu("SOPS", innerContent, option, size);
                        }
                        else if(_ACTIVE_ACTION == "ADD"){
                            option = 0;
                            string innerMenuADD[2] = {"\033[4mA\033[0mDD Another SOPS", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "EDIT"){
                            option = 0;
                            string innerMenuADD[2] = {"\033[4mE\033[0mDIT Another SOPS", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                        else if(_ACTIVE_ACTION == "DELETE"){
                            option = 0;
                            string innerMenuADD[2] = {"\033[4mD\033[0mELETE Another SOPS", "\033[4mB\033[0mack"};
                            size = sizeof(innerMenuADD) / sizeof(innerMenuADD[0]);
                            menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                            while(true){
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
                                else if(GetAsyncKeyState(VK_SPACE))
                                {
                                    if(option == 0)
                                    {
                                        break;
                                    }
                                    else if(option == 1)
                                    {
                                        _ACTIVE_ACTION = "VIEW";
                                        break;
                                    }
                                }
                                menu(innerMenuADD, "innerMenuADD", size, option, 27, 35);
                                Sleep(200);
                            }
                        }
                    }
                    
                }
            }
            else if (_ACTIVE_PAGE == "LOGOUT")
            {
                system("cls");
                string name = session("fname");
                if(sessionEnd())
                {
                    gotoxy(3,3); cout << "Logging out ...";
                    Sleep(300);
                    gotoxy(3,5); cout << "We will MISS you " << name;
                    _ACTIVE_SCREEN = "SPLASH";
                    Sleep(2000);
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
            if(i == size - 1)
                y = 37;
            else
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
    else if (heading == "Specimen" || heading == "Lab Department" || heading == "Lab Test" || heading == "Packages" || heading == "Rate List" || heading == "Machine" || heading == "SOPS")
    {
        for (int i = 0; i < size; i++)
        {
            gotoxy(x, y);
            cout << "[" << op[i] << "] " << menu[i] << " " << heading;
            x += 30;
        }
    }
    else if (heading == "innerMenuADD")
    {
        for (int i = 0; i < size; i++)
        {
            gotoxy(x, y);
            cout << "[" << op[i] << "] " << menu[i];
            y += 2;
        }
    }
    else if (heading == "selection")
    {
        for (int i = 0; i < size; i++)
        {
            gotoxy(x, y);
            cout << "[" << op[i] << "] " << menu[i];
            y += 2;
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
            if(_ACTIVE_ACTION == "VIEW")
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
            if(option == 0){
                _ACTIVE_ACTION = "ADD";
                break;
            }
            else if(option == 1){
                _ACTIVE_ACTION = "EDIT";
                break;
            }
            else if(option == 2){
                _ACTIVE_ACTION = "DELETE";
                break;
            }
        }
        menu(subMenu, pageName, size, option, 27, 4);
        Sleep(200);
    }
}
bool mainScreen(int x, int y)
{
    system("cls");
    gotoxy(x, y); cout<<"------------------------------------------------------------------------------------------------------------------------";
    gotoxy(x, y + 1); cout<<"|          ILMS          |";      gotoxy(x+85, y+1); cout << "LOGIN Time : " << _LOGIN_TIME; gotoxy(x + 119, y + 1);cout << "|";
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
bool specimenPage(int x, int y, string title, string menu[], int option, int size)
{
    if (mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New Specimen";
            gotoxy(x + 30, y); cout<<"[ ] EDIT Specimen";
            gotoxy(x + 60, y); cout<<"[ ] DELETE Specimen";
            y++;
            gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
            gotoxy(x, y + 2); cout<<"| \033[4mSr\033[0m |  \033[4mID\033[0m  |                 \033[4mNAME\033[0m                 |             \033[4mDESCRIPTION\033[0m              |";
            gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
            int j = 4;
            if(specimenCount == 0)
            {
                gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < specimenCount ; i++){
                    gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                    gotoxy(x + 7 , y + j); cout << specimenID[i]; gotoxy(x + 12, y + j); cout << "|";
                    gotoxy(x + 14 , y + j); cout << specimenName[i]; gotoxy(x + 51, y + j); cout << "|";
                    gotoxy(x + 53 , y + j); cout << specimenDescription[i]; gotoxy(x + 90, y + j); cout << "|";
                    gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                    j+=2;
                }
            }
            
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menu, option, size);
            
            if(specimenCount < dataSize){
                string id, name, description;
                gotoxy(x, y); cout << "\033[4mADD NEW SPECIMEN\033[0m";
                gotoxy(x, y+3); cout << "Enter the Specimen Name : ";
                gotoxy(x + 40, y+3);
                cin.ignore();
                getline(cin, name);
                gotoxy(x, y+5); cout << "Enter the Specimen Description : ";
                gotoxy(x + 40, y+5); getline(cin, description);

                /* id calculation */
                if(specimenCount == 0)
                    id = "S001";
                else{
                    id = specimenID[specimenCount-1];
                    int idx = id.length() - 1;
                    while(idx >= 1)
                    {
                        if(id[idx] < '9')
                        {
                            id[idx] += 1;
                            break;
                        }
                        else{
                            id[idx] = '0';
                            idx--;
                        }
                    }
                }
                
                specimenID[specimenCount] = id;
                specimenName[specimenCount] = name;
                specimenDescription[specimenCount] = description;
                specimenCount++;
            }
            else{
                gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
            }
        }
        else if(_ACTIVE_ACTION == "EDIT")
        {
            sideBars(title, menu, option, size);
            if(specimenCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mEDIT SPECIMEN\033[0m";
                gotoxy(x, y+3); cout << "Enter the Specimen's ID to be EDITED : ";
                cin >> id;
                for(int i = 0; i < specimenCount ; i++)
                {
                    if(id == specimenID[i])
                    {
                        string name, description;
                        gotoxy(x, y+5); cout << "Change the Specimen Name from " << specimenName[i] << " to ";
                        gotoxy(x + 60, y+5);
                        cin.ignore();
                        getline(cin, name);
                        gotoxy(x, y+7); cout << "Change the Specimen Description from " << specimenDescription[i] << " to ";
                        gotoxy(x + 60, y+7); getline(cin, description);
                        
                        specimenName[i] = name;
                        specimenDescription[i] = description;
                        
                        msg = "DATA EDITED SUCCESSFULLY";
break;
                    }
                }
                gotoxy(x, y + 9); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO EDIT.";
            }
        }
        else if(_ACTIVE_ACTION == "DELETE")
        {
            sideBars(title, menu, option, size);
            if(specimenCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mDELETE SPECIMEN\033[0m";
                gotoxy(x, y+3); cout << "Enter the Specimen's ID to be DELETED : ";

                cin >> id;
                for(int i = 0; i < specimenCount ; i++)
                {
                    if(id == specimenID[i])
                    {
                        specimenID[i] = '\0';
                        specimenName[i] = '\0';
                        specimenDescription[i] = '\0';
                        specimenCount--;
                        msg = "DATA DELTED SUCCESSFULLY";
                        for(int x = i; x < specimenCount; x++)
                        {
                            specimenID[x] = specimenID[x + 1];
                            specimenName[x] = specimenName[x + 1];
                            specimenDescription[x] = specimenDescription[x + 1];
                        }

                        break;
                    }
                }
                gotoxy(x, y + 7); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO DELETE.";
            }
        }
    }
    return true;
}
bool labDepartmentPage(int x, int y, string title, string menu[], int option, int size)
{
    if (mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New Lab Department";
            gotoxy(x + 30, y); cout<<"[ ] EDIT Lab Department";
            gotoxy(x + 60, y); cout<<"[ ] DELETE Lab Department";
            y++;
            gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
            gotoxy(x, y + 2); cout<<"| \033[4mSr\033[0m |  \033[4mID\033[0m   |                   \033[4mNAME\033[0m                    |              \033[4mDATE\033[0m              |";
            gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
            
            int j = 4;
            if(labDepartmentCount == 0)
            {
                gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < labDepartmentCount ; i++){
                    gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                    gotoxy(x + 7 , y + j); cout << labDepartemtID[i]; gotoxy(x + 13, y + j); cout << "|";
                    gotoxy(x + 15 , y + j); cout << labDepartmentName[i]; gotoxy(x + 57, y + j); cout << "|";
                    gotoxy(x + 59 , y + j); cout << labDepartmentDate[i]; gotoxy(x + 90, y + j); cout << "|";
                    gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                    j+=2;
                }
            }
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menu, option, size);
            
            if(labDepartmentCount < dataSize){
                string id, name;
                gotoxy(x, y); cout << "\033[4mADD New Lab Department\033[0m";
                gotoxy(x, y+3); cout << "Enter the Lab Department Name : ";
                gotoxy(x + 40, y+3);
                cin.ignore();
                getline(cin, name);

                /* id calculation */
                if(labDepartmentCount == 0)
                    id = "LD001";
                else{
                    id = labDepartemtID[labDepartmentCount-1];
                    int idx = id.length() - 1;
                    while(idx >= 1)
                    {
                        if(id[idx] < '9')
                        {
                            id[idx] += 1;
                            break;
                        }
                        else{
                            id[idx] = '0';
                            idx--;
                        }
                    }
                }
                
                labDepartemtID[labDepartmentCount] = id;
                labDepartmentName[labDepartmentCount] = name;
                labDepartmentDate[labDepartmentCount] = dateTime("date");
                labDepartmentCount++;
            }
            else{
                gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
            }

        }
        else if(_ACTIVE_ACTION == "EDIT")
        {
            sideBars(title, menu, option, size);
            if(labDepartmentCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mEDIT Lab Department\033[0m";
                gotoxy(x, y+3); cout << "Enter the Lab Department's ID to be EDITED : ";
                cin >> id;
                for(int i = 0; i < labDepartmentCount ; i++)
                {
                    if(id == labDepartemtID[i])
                    {
                        string name;
                        gotoxy(x, y+5); cout << "Change the Lab Department Name from " << labDepartmentName[i] << " to ";
                        gotoxy(x + 60, y+5);
                        cin.ignore();
                        getline(cin, name);
                        
                        labDepartmentName[i] = name;
                        
                        msg = "DATA EDITED SUCCESSFULLY";
                        break;
                    }
                }
                gotoxy(x, y + 9); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO EDIT.";
            }
        }
        else if(_ACTIVE_ACTION == "DELETE")
        {
            sideBars(title, menu, option, size);
            if(labDepartmentCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mDELETE LAB DEPARTMENT\033[0m";
                gotoxy(x, y+3); cout << "Enter the Lab Department's ID to be DELETED : ";
                cin >> id;
                for(int i = 0; i < labDepartmentCount ; i++)
                {
                    if(id == labDepartemtID[i])
                    {
                        labDepartemtID[i] = '\0';
                        labDepartmentName[i] = '\0';
                        labDepartmentDate[i] = '\0';
                        labDepartmentCount--;
                        msg = "DATA DELTED SUCCESSFULLY";
                        for(int x = i; x < labDepartmentCount; x++)
                        {
                            labDepartemtID[x] = labDepartemtID[x + 1];
                            labDepartmentName[x] = labDepartmentName[x + 1];
                            labDepartmentDate[x] = labDepartmentDate[x + 1];
                        }

                    }
                }
                gotoxy(x, y + 7); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO DELETE.";
            }
        }
    }
    return true;
}
bool labTestPage(int x, int y, string title, string menus[], int option, int size)
{
    if(mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New Lab Test";
            gotoxy(x + 30, y); cout<<"[ ] EDIT Lab Test";
            gotoxy(x + 60, y); cout<<"[ ] DELETE Lab Test";
            y++;
            
            int j = 1;

            if(labTestCount == 0)
            {
                gotoxy(x, y + j);     cout<<"-------------------------------------------------------------------------------------------";
                gotoxy(x, y + j + 1); cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 2); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < labTestCount ; i++){
                    
                    gotoxy(x, y + j); cout << "-------------------------------------------------------------------------------------------"; j++;
                    gotoxy(x, y + j); cout << "| Sr. " << i + 1; gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "|                                                                                         |"; j++;
                    gotoxy(x, y + j); cout << "| ID : " << labTestID[i]; gotoxy(x+20, y + j); cout << "NAME : " << labTestName[i]; gotoxy(x+65, y + j);  cout << "RATE : " << labTestRate[i]; gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "| GROUP : " << labTestGroup[i];gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "| SPECIMEN : " << labTestSpecimen[i];gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "| UNITS : " << labTestUnit[i]; gotoxy(x+40, y + j); cout << "MACHINE: " << labTestMachine[i];gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "| Test Perform Frequency : "<< labTestFreq[i]; gotoxy(x+50, y + j); cout << "Delivery Time : " << labTestTime[i];gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "| COMMENTS : " << labTestComments[i]; gotoxy(x+90, y + j); cout << "|"; j++;
                    gotoxy(x, y + j); cout << "-------------------------------------------------------------------------------------------"; j++;
                }
            }
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menus, option, size);
            
            if(labTestCount < dataSize){
                bool speci = false, labD = false, mach = false;
                string id, name, group, rate, specimen, units, machine, frequency, deliveryTime, comments;
                gotoxy(x, y); cout << "\033[4mADD New Lab Test\033[0m";

                gotoxy(x, y+3); cout << "Enter Lab Test Name : ";
                gotoxy(x + 40, y+3);cin.ignore();getline(cin, name);

                gotoxy(x, y+5); cout << "Enter Lab Test Rate : ";
                gotoxy(x + 40, y+5); getline(cin, rate);

                gotoxy(x, y+7); cout << "Enter Lab Test Result Unit : ";
                gotoxy(x + 40, y+7); getline(cin, units);

                gotoxy(x, y+9); cout << "Enter Lab Test Result Frequency : ";
                gotoxy(x, y+10); cout << "(daily, weekly, monthly, etc.)";
                gotoxy(x + 40, y+9); getline(cin, frequency);

                gotoxy(x, y+12); cout << "Enter Lab Test Delivery Time : ";
                gotoxy(x, y+13); cout << "(xHrs, xDays, etc)";
                gotoxy(x + 40, y+12);getline(cin, deliveryTime);

                gotoxy(x, y+15); cout << "Enter Lab Test Comments : ";
                gotoxy(x + 40, y+15);getline(cin, comments);

                if(clear(x, y+3, 117, 37))
                {
                    gotoxy(x, y+3); cout << "Select the Required Specimen for Lab Test: ";
                    int op = 0;
                    int size = specimenCount;
                    if(size > 0)
                    {
                        while(true)
                        {
                            menu(specimenName, "selection", size, op, x + 7, y+5);
                            if (GetAsyncKeyState(VK_DOWN))
                            {
                                if (op < size - 1)
                                {
                                    op++;
                                }
                            }
                            else if (GetAsyncKeyState(VK_UP))
                            {
                                if (op > 0)
                                {
                                    op--;
                                }
                            }
                            else if(GetAsyncKeyState(VK_SPACE))
                            {
                                specimen = specimenName[op];
                                speci = true;
                                Sleep(200);
                                break;
                            }
                            menu(specimenName, "selection", size, op, x + 7, y+5);
                            Sleep(200);
                        }
                    }
                    else
                    {
                        gotoxy(x, y + 5); cout << "You have not ADDED any SPEDCIMEN YET!";
                    }

                }
                if(speci && clear(x, y+3, 117, 37))
                {
                    gotoxy(x, y+3); cout << "Select the Lab Department / Group for Lab Test: ";
                    int op = 0;
                    int size = labDepartmentCount;
                    if(size > 0)
                    {
                        while(true)
                        {
                            menu(labDepartmentName, "selection", size, op, x + 7, y+5);
                            if (GetAsyncKeyState(VK_DOWN))
                            {
                                if (op < size - 1)
                                {
                                    op++;
                                }
                            }
                            else if (GetAsyncKeyState(VK_UP))
                            {
                                if (op > 0)
                                {
                                    op--;
                                }
                            }
                            else if(GetAsyncKeyState(VK_SPACE))
                            {
                                group = labDepartmentName[op];
                                labD = true;
                                Sleep(200);
                                break;
                            }
                            menu(labDepartmentName, "selection", size, op, x + 7, y+5);
                            Sleep(200);
                        }
                    }
                    else
                    {
                        gotoxy(x, y + 5); cout << "You have not ADDED any Lab Department YET!";
                    }

                }
                if(labD && clear(x, y+3, 117, 37))
                {
                    gotoxy(x, y+3); cout << "Select the Machine for Lab Test: ";
                    int op = 0;
                    int size = machineCount;
                    if(size > 0)
                    {
                        while(true)
                        {
                            menu(machineName, "selection", size, op, x + 7, y+5);
                            if (GetAsyncKeyState(VK_DOWN))
                            {
                                if (op < size - 1)
                                {
                                    op++;
                                }
                            }
                            else if (GetAsyncKeyState(VK_UP))
                            {
                                if (op > 0)
                                {
                                    op--;
                                }
                            }
                            else if(GetAsyncKeyState(VK_SPACE))
                            {
                                machine = machineName[op];
                                mach = true;
                                Sleep(200);
                                break;
                            }
                            menu(machineName, "selection", size, op, x + 7, y+5);
                            Sleep(200);
                        }
                    }
                    else
                    {
                        gotoxy(x, y + 5); cout << "You have not ADDED any Machine YET!";
                    }

                }

                /* id calculation */
                if(labTestCount == 0)
                    id = "LT001";
                else{
                    id = labTestID[labTestCount-1];
                    int idx = id.length() - 1;
                    while(idx >= 1)
                    {
                        if(id[idx] < '9')
                        {
                            id[idx] += 1;
                            break;
                        }
                        else{
                            id[idx] = '0';
                            idx--;
                        }
                    }
                }
                
                if(speci && labD && mach)
                {
                    labTestID[labTestCount] = id;
                    labTestName[labTestCount] = name;
                    labTestRate[labTestCount] = rate;
                    labTestGroup[labTestCount] = group;
                    labTestMachine[labTestCount] = machine;
                    labTestUnit[labTestCount] = units;
                    labTestFreq[labTestCount] = frequency;
                    labTestTime[labTestCount] = deliveryTime;
                    labTestComments[labTestCount] = comments;
                    labTestSpecimen[labTestCount] = specimen;
                    labTestCount++;
                }
                else{
                    gotoxy(x, y+19); cout << "Lab Test is not added.";
                }
            }
            else{
                gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
            }
        }
        else if(_ACTIVE_ACTION == "EDIT")
        {
            sideBars(title, menus, option, size);
            if(labTestCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mEDIT LAB TEST\033[0m";
                gotoxy(x, y+3); cout << "Enter the Lab Test's ID to be EDITED : ";
                cin >> id;
                for(int i = 0; i < labTestCount ; i++)
                {
                    if(id == labTestID[i])
                    {
                        string name, group, rate, specimen, units, machine, frequency, deliveryTime, comments;
                        gotoxy(x, y+5); cout << "Change the LabTest Name from " << labTestName[i] << " to ";
                        gotoxy(x + 60, y+5);cin.ignore(); getline(cin, name);

                        gotoxy(x, y+7); cout << "Change the LabTest Rate from " << labTestRate[i] << " to ";
                        gotoxy(x + 60, y+7); getline(cin, rate);

                        gotoxy(x, y+9); cout << "Change the LabTest Unit from " << labTestUnit[i] << " to ";
                        gotoxy(x + 60, y+9); getline(cin, units);

                        gotoxy(x, y+11); cout << "Change the LabTest Frequency from " << labTestFreq[i] << " to ";
                        gotoxy(x, y+12); cout << "(daily, weekly, monthly, etc.)";
                        gotoxy(x + 60, y+11); getline(cin, frequency);

                        gotoxy(x, y+14); cout << "Change the LabTest Delivery Time from " << labTestTime[i] << " to ";
                        gotoxy(x, y+15); cout << "(xHrs, xDays, etc)";
                        gotoxy(x + 60, y+14);getline(cin, deliveryTime);

                        gotoxy(x, y+17); cout << "Change the LabTest Unit from " << labTestComments[i] << " to ";
                        gotoxy(x + 60, y+18);getline(cin, comments);
                        
                        if(clear(x, y+3, 117, 37))
                        {
                            gotoxy(x, y+3); cout << "Select the Required Specimen for Lab Test from " << labTestSpecimen[i] << " to ";
                            int op = 0;
                            int size = specimenCount;
                            if(size > 0)
                            {
                                while(true)
                                {
                                    menu(specimenName, "selection", size, op, x + 7, y+5);
                                    if (GetAsyncKeyState(VK_DOWN))
                                    {
                                        if (op < size - 1)
                                        {
                                            op++;
                                        }
                                    }
                                    else if (GetAsyncKeyState(VK_UP))
                                    {
                                        if (op > 0)
                                        {
                                            op--;
                                        }
                                    }
                                    else if(GetAsyncKeyState(VK_SPACE))
                                    {
                                        specimen = specimenName[op];
                                        Sleep(200);
                                        break;
                                    }
                                    menu(specimenName, "selection", size, op, x + 7, y+5);
                                    Sleep(200);
                                }
                            }
                            else
                            {
                                gotoxy(x, y + 5); cout << "You have not ADDED any SPEDCIMEN YET!";
                            }

                        }
                        if(clear(x, y+3, 117, 37))
                        {
                            gotoxy(x, y+3); cout << "Select the Lab Department / Group for Lab Testfrom " << labTestGroup[i] << " to ";
                            int op = 0;
                            int size = labDepartmentCount;
                            if(size > 0)
                            {
                                while(true)
                                {
                                    menu(labDepartmentName, "selection", size, op, x + 7, y+5);
                                    if (GetAsyncKeyState(VK_DOWN))
                                    {
                                        if (op < size - 1)
                                        {
                                            op++;
                                        }
                                    }
                                    else if (GetAsyncKeyState(VK_UP))
                                    {
                                        if (op > 0)
                                        {
                                            op--;
                                        }
                                    }
                                    else if(GetAsyncKeyState(VK_SPACE))
                                    {
                                        group = labDepartmentName[op];
                                        Sleep(200);
                                        break;
                                    }
                                    menu(labDepartmentName, "selection", size, op, x + 7, y+5);
                                    Sleep(200);
                                }
                            }
                            else
                            {
                                gotoxy(x, y + 5); cout << "You have not ADDED any Lab Department YET!";
                            }

                        }
                        if(clear(x, y+3, 117, 37))
                        {
                            gotoxy(x, y+3); cout << "Select the Machine for Lab Test from " << labTestMachine[i] << " to ";
                            int op = 0;
                            int size = machineCount;
                            if(size > 0)
                            {
                                while(true)
                                {
                                    menu(machineName, "selection", size, op, x + 7, y+5);
                                    if (GetAsyncKeyState(VK_DOWN))
                                    {
                                        if (op < size - 1)
                                        {
                                            op++;
                                        }
                                    }
                                    else if (GetAsyncKeyState(VK_UP))
                                    {
                                        if (op > 0)
                                        {
                                            op--;
                                        }
                                    }
                                    else if(GetAsyncKeyState(VK_SPACE))
                                    {
                                        machine = machineName[op];
                                        Sleep(200);
                                        break;
                                    }
                                    menu(machineName, "selection", size, op, x + 7, y+5);
                                    Sleep(200);
                                }
                            }
                            else
                            {
                                gotoxy(x, y + 5); cout << "You have not ADDED any Machine YET!";
                            }

                        }
                        
                        labTestName[labTestCount] = name;
                        labTestRate[labTestCount] = rate;
                        labTestGroup[labTestCount] = group;
                        labTestMachine[labTestCount] = machine;
                        labTestUnit[labTestCount] = units;
                        labTestFreq[labTestCount] = frequency;
                        labTestTime[labTestCount] = deliveryTime;
                        labTestComments[labTestCount] = comments;
                        labTestSpecimen[labTestCount] = specimen;
                        
                        msg = "DATA EDITED SUCCESSFULLY";
                        break;
                    }
                }
                gotoxy(x, y + 14); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO EDIT.";
            }
        }
        else if(_ACTIVE_ACTION == "DELETE")
        {
            sideBars(title, menus, option, size);
            if(labTestCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mDELETE LAB TEST\033[0m";
                gotoxy(x, y+3); cout << "Enter the LabTest's ID to be DELETED : ";
                cin >> id;
                for(int i = 0; i < labTestCount ; i++)
                {
                    if(id == labTestID[i])
                    {
                        labTestID[labTestCount] = '\0';
                        labTestName[labTestCount] = '\0';
                        labTestRate[labTestCount] = '\0';
                        labTestGroup[labTestCount] = '\0';
                        labTestMachine[labTestCount] = '\0';
                        labTestUnit[labTestCount] = '\0';
                        labTestFreq[labTestCount] = '\0';
                        labTestTime[labTestCount] = '\0';
                        labTestComments[labTestCount] = '\0';
                        labTestSpecimen[labTestCount] = '\0';
                        labTestCount--;
                        msg = "DATA DELETED SUCCESSFULLY";
                        for(int x = i; x < labTestCount; x++)
                        {
                            labTestID[x] = labTestID[x + 1];
                            labTestName[x] = labTestName[x + 1];
                            labTestRate[x] = labTestRate[x + 1];
                            labTestGroup[x] = labTestGroup[x + 1];
                            labTestMachine[x] = labTestMachine[x + 1];
                            labTestUnit[x] = labTestUnit[x + 1];
                            labTestFreq[x] = labTestFreq[x + 1];
                            labTestTime[x] = labTestTime[x + 1];
                            labTestComments[x] = labTestComments[x + 1];
                            labTestSpecimen[x] = labTestSpecimen[x + 1];
                        }
                        
                        break;
                    }
                }
                gotoxy(x, y + 7); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO DELETE.";
            }
        }
    }
    return true;
}
bool machinesPage(int x, int y, string title, string menu[], int option, int size)
{
    if (mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New Machine";
            gotoxy(x + 30, y); cout<<"[ ] EDIT Machine";
            gotoxy(x + 60, y); cout<<"[ ] DELETE Machine";
            y++;
            gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
            gotoxy(x, y + 2); cout<<"| \033[4mSr\033[0m |  \033[4mID\033[0m  |               \033[4mNAME\033[0m              |          \033[4mDESCRIPTION\033[0m           | \033[4mQUANTITY\033[0m |";
            gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
            int j = 4;
            if(machineCount == 0)
            {
                gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < machineCount ; i++){
                    gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                    gotoxy(x + 7 , y + j); cout << machineID[i]; gotoxy(x + 12, y + j); cout << "|";
                    gotoxy(x + 14 , y + j); cout << machineName[i]; gotoxy(x + 46, y + j); cout << "|";
                    gotoxy(x + 48 , y + j); cout << machineDescription[i]; gotoxy(x + 79, y + j); cout << "|";
                    gotoxy(x + 81 , y + j); cout << machineQuantity[i]; gotoxy(x + 90, y + j); cout << "|";
                    gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                    j+=2;
                }
            }
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menu, option, size);
            
            if(machineCount < dataSize){
                string id, name, description, quantity;
                gotoxy(x, y); cout << "\033[4mADD New Machine\033[0m";
                gotoxy(x, y+3); cout << "Enter the Machine Name : ";
                gotoxy(x + 40, y+3);
                cin.ignore();
                getline(cin, name);
                gotoxy(x, y+5); cout << "Enter the Machine Description : ";
                gotoxy(x + 40, y+5); getline(cin, description);
                gotoxy(x, y+7); cout << "Enter the Machine Quantity : ";
                gotoxy(x + 40, y+7); getline(cin, quantity);

                /* id calculation */
                if(machineCount == 0)
                    id = "M001";
                else{
                    id = machineID[machineCount-1];
                    int idx = id.length() - 1;
                    while(idx >= 1)
                    {
                        if(id[idx] < '9')
                        {
                            id[idx] += 1;
                            break;
                        }
                        else{
                            id[idx] = '0';
                            idx--;
                        }
                    }
                }
                
                machineID[machineCount] = id;
                machineName[machineCount] = name;
                machineDescription[machineCount] = description;
                machineQuantity[machineCount] = quantity;
                machineCount++;
            }
            else{
                gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
            }

        }
        else if(_ACTIVE_ACTION == "EDIT")
        {
            sideBars(title, menu, option, size);
            if(machineCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mEDIT MACHINES\033[0m";
                gotoxy(x, y+3); cout << "Enter the Machine's ID to be EDITED : ";
                cin >> id;
                for(int i = 0; i < machineCount ; i++)
                {
                    if(id == machineID[i])
                    {
                        string name, description, quantity;
                        gotoxy(x, y+5); cout << "Change the Machine Name from " << machineName[i] << " to ";
                        gotoxy(x + 60, y+5);
                        cin.ignore();
                        getline(cin, name);
                        gotoxy(x, y+7); cout << "Change the Machine Description from " << machineDescription[i] << " to ";
                        gotoxy(x + 60, y+7); getline(cin, description);
                        gotoxy(x, y+9); cout << "Change the Machine Quantity from " << machineQuantity[i] << " to ";
                        gotoxy(x + 60, y+9); getline(cin, quantity);
                        
                        machineName[i] = name;
                        machineDescription[i] = description;
                        machineQuantity[i] = quantity;
                        
                        msg = "DATA EDITED SUCCESSFULLY";
                        break;
                    }
                }
                gotoxy(x, y + 9); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO EDIT.";
            }
        }
        else if(_ACTIVE_ACTION == "DELETE")
        {
            sideBars(title, menu, option, size);
            if(machineCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mDELETE MACHINES\033[0m";
                gotoxy(x, y+3); cout << "Enter the Machine's ID to be DELETED : ";
                cin >> id;
                for(int i = 0; i < machineCount ; i++)
                {
                    if(id == machineID[i])
                    {
                        machineID[i] = '\0';
                        machineName[i] = '\0';
                        machineDescription[i] = '\0';
                        machineQuantity[i] = '\0';
                        machineCount--;
                        msg = "DATA DELTED SUCCESSFULLY";
                        for(int x = i; x < machineCount; x++)
                        {
                            machineID[x] = machineID[x + 1];
                            machineName[x] = machineName[x + 1];
                            machineDescription[x] = machineDescription[x + 1];
                            machineQuantity[x] = machineQuantity[x + 1];
                        }
                        break;
                    }
                }
                gotoxy(x, y + 7); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO DELETE.";
            }
        }
    }
    return true;
}
bool packagesPage(int x, int y, string title, string menu[], int option, int size)
{
    if (mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New Packages";
            gotoxy(x + 30, y); cout<<"[ ] EDIT Packages";
            gotoxy(x + 60, y); cout<<"[ ] DELETE Packages";
            y++;           
            gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
            gotoxy(x, y + 2); cout<<"| \033[4mSr\033[0m |  \033[4mID\033[0m  |         \033[4mNAME\033[0m         |    \033[4mLABTESTS\033[0m    |    \033[4mRATE\033[0m    | \033[4mDISCOUNT\033[0m |";
            gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
            int j = 4;
            if(packageCount == 0)
            {
                gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < packageCount ; i++){
                    gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                    gotoxy(x + 7 , y + j); cout << packageID[i]; gotoxy(x + 12, y + j); cout << "|";
                    gotoxy(x + 14 , y + j); cout << packageName[i]; gotoxy(x + 41, y + j); cout << "|";
                    gotoxy(x + 43 , y + j); cout << packageRate[i]; gotoxy(x + 90, y + j); cout << "|";
                    gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                    j+=2;
                }
            }
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menu, option, size);
            if(labTestCount > 1)
            {
                if(packageCount < dataSize){
                    string id, name, testCount, discount;
                    float price = 0;
                    gotoxy(x, y); cout << "\033[4mADD New Packages\033[0m";
                    gotoxy(x, y+3); cout << "Enter the Packages Name : ";
                    gotoxy(x + 40, y+3);
                    cin.ignore();
                    getline(cin, name);
                    gotoxy(x, y+5); cout << "You can add 2 - 5 tests in each package. Only Number Type Data Allowed";
                    gotoxy(x, y+7); cout << "Enter the number of tests you want to add : ";
                    gotoxy(x + 50, y+7); getline(cin, testCount);
                    while(!isValidate("packageTestCount", testCount))
                    {
                        gotoxy(x + 50, y+7); cout << "                            ";
                        gotoxy(x + 50, y+7); getline(cin, testCount);
                    }
                    int count = stoi(testCount), j = y + 9;
                    string tests[count];
                    for(int i = 0; i < count ; i++)
                    {
                        bool isTestRepeat = false;
                        gotoxy(x, j); cout << "Enter the valid LabTest ID for the test " << i+1 <<" : ";
                        gotoxy(x+50, j); cin >> tests[i];

                        tests[i] = labTestIDValid(tests, tests[i], i);
                        while(!isValidate("labTestIDExists", tests[i]))
                        {   
                            gotoxy(x + 50, j); cout << "                       ";
                            gotoxy(x + 50, j); cin >> tests[i];
                            tests[i] = labTestIDValid(tests, tests[i], i);
                        }

                        for(int k = 0; k < labTestCount; k++)
                        {
                            if(labTestID[k] == tests[i])
                            {
                                price += stof(labTestRate[i]);
                                break;
                            }
                        }
                        j++;
                    }

                    gotoxy(x, j+1); cout << "Enter the Discount(%) Only 10(%) - 70(%) : ";
                    gotoxy(x+50, j+1); getline(cin, discount);
                    while(!isValidate("packageDiscount", discount))
                    {
                        gotoxy(x + 50, j+1); cout << "                            ";
                        gotoxy(x + 50, j+1); getline(cin, discount);
                    }
                    
                    /* id calculation */
                    if(packageCount == 0)
                        id = "P001";
                    else
                    {
                        id = packageID[packageCount - 1];
                        int idx = id.length() - 1;
                        while(idx >= 1)
                        {
                            if(id[idx] < '9')
                            {
                                id[idx] += 1;
                                break;
                            }
                            else{
                                id[idx] = '0';
                                idx--;
                            }
                        }
                    }
                    
                    packageID[packageCount] = id;
                    packageName[packageCount] = name;
                    packageDisc[packageCount] = discount;
                    /* price calculation */
                    price = price - ((stof(discount) * price) / 100.0);
                    packageRate[packageCount] = to_string(static_cast<int>(round(price)));

                    for(int i = 0; i < count ; i++)
                        packageTests[packageCount][i] = tests[i];

                    packageTestCount[packageCount] = count;
                    packageCount++;
                }
                else{
                    gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
                }
            }
            else
            {
                gotoxy(x, y); cout << "ADD ATLEAST TWO OR MORE LABTESTS";
            }
        }
    }
    return true;
}
bool rateListPage(int x, int y)
{
    if (mainScreen(0, 0))
    {
        gotoxy(x, y); cout<<"-------------------------------------------------------------------------------------------";
        gotoxy(x, y + 1); cout<<"| \033[4mSr\033[0m |             \033[4mTEST NAME\033[0m                     |                  \033[4mRATE\033[0m                  |";
        gotoxy(x, y + 2); cout<<"-------------------------------------------------------------------------------------------";
        
        int j = 3;
        if(labTestCount == 0)
        {
            gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
            gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
        }
        else
        {
            for(int i = 0; i < labTestCount ; i++){
                gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                gotoxy(x + 7 , y + j); cout << labTestName[i]; gotoxy(x + 49, y + j); cout << "|";
                gotoxy(x + 51 , y + j); cout << labTestRate[i]; gotoxy(x + 90, y + j); cout << "|";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                j+=2;
            }
        }
    }
    return true;
}
bool sopsPage(int x, int y, string title, string menu[], int option, int size)
{
    if (mainScreen(0, 0))
    {
        if(_ACTIVE_ACTION == "VIEW")
        {
            gotoxy(x, y); cout<<"[ ] ADD New SOPS";
            gotoxy(x + 30, y); cout<<"[ ] EDIT SOPS";
            gotoxy(x + 60, y); cout<<"[ ] DELETE SOPS";
            y++;
            gotoxy(x, y + 1); cout<<"-------------------------------------------------------------------------------------------";
            gotoxy(x, y + 2); cout<<"| \033[4mSr\033[0m |  \033[4mID\033[0m   |                           \033[4mSOP\033[0m                             |      \033[4mDATE\033[0m      |";
            gotoxy(x, y + 3); cout<<"-------------------------------------------------------------------------------------------";
            
            int j = 4;
            if(sopCount == 0)
            {
                gotoxy(x, y + j);     cout<<"|                                  NO DATA ENTERED YET                                    |";
                gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
            }
            else
            {
                for(int i = 0; i < sopCount ; i++){
                    gotoxy(x, y + j); cout << "|"; gotoxy(x + 2, y + j); cout << i + 1 << "."; gotoxy(x + 5, y + j); cout << "|";
                    gotoxy(x + 7 , y + j); cout << sopID[i]; gotoxy(x + 13, y + j); cout << "|";
                    gotoxy(x + 15 , y + j); cout << sop[i]; gotoxy(x + 73, y + j); cout << "|";
                    gotoxy(x + 75 , y + j); cout << sopDate[i]; gotoxy(x + 90, y + j); cout << "|";
                    gotoxy(x, y + j + 1); cout<<"-------------------------------------------------------------------------------------------";
                    j+=2;
                }
            }
        }
        else if(_ACTIVE_ACTION == "ADD")
        {
            sideBars(title, menu, option, size);
            
            if(sopCount < dataSize){
                string id, sops;
                gotoxy(x, y); cout << "\033[4mADD New SOP\033[0m";
                gotoxy(x, y+3); cout << "Enter the SOP: ";
                gotoxy(x + 40, y+3);cin.ignore(); getline(cin, sops);

                /* id calculation */
                if(sopCount == 0)
                    id = "SP001";
                else{
                    id = sopID[sopCount-1];
                    int idx = id.length() - 1;
                    while(idx >= 1)
                    {
                        if(id[idx] < '9')
                        {
                            id[idx] += 1;
                            break;
                        }
                        else{
                            id[idx] = '0';
                            idx--;
                        }
                    }
                }
                
                sopID[sopCount] = id;
                sop[sopCount] = sops;
                sopDate[sopCount] = dateTime("date");
                sopCount++;
            }
            else{
                gotoxy(x, y); cout << "YOU HAVE REACHED MAX STORAGE LIMIT.";
            }
        }
        else if(_ACTIVE_ACTION == "EDIT")
        {
            sideBars(title, menu, option, size);
            if(sopCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mEDIT SOP\033[0m";
                gotoxy(x, y+3); cout << "Enter the SOP's ID to be EDITED : ";
                cin >> id;
                for(int i = 0; i < sopCount ; i++)
                {
                    if(id == sopID[i])
                    {
                        string sops;
                        gotoxy(x, y+5); cout << "Change sop from "; 
                        gotoxy(x + 60, y+7); cout << sop[i] << " to ";
                        gotoxy(x + 60, y+9);
                        cin.ignore();
                        getline(cin, sops);
                        
                        sop[i] = sops;
                        
                        msg = "DATA EDITED SUCCESSFULLY";
                        break;
                    }
                }
                gotoxy(x, y + 9); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO EDIT.";
            }
        }
        else if(_ACTIVE_ACTION == "DELETE")
        {
            sideBars(title, menu, option, size);
            if(sopCount > 0)
            {
                string id, msg = "INVALID ID";
                gotoxy(x, y); cout << "\033[4mDELETE SOP\033[0m";
                gotoxy(x, y+3); cout << "Enter the SOP's ID to be DELETED : ";
                cin >> id;
                for(int i = 0; i < sopCount ; i++)
                {
                    if(id == sopID[i])
                    {
                        sopID[i] = '\0';
                        sop[i] = '\0';
                        sopDate[i] = '\0';
                        sopCount--;
                        msg = "DATA DELTED SUCCESSFULLY";
                        for(int x = i; x < sopCount; x++)
                        {
                            sopID[x] = sopID[x + 1];
                            sop[x] = sop[x + 1];
                            sopDate[x] = sopDate[x + 1];
                        }

                    }
                }
                gotoxy(x, y + 7); cout << msg;
            }
            else
            {
                gotoxy(x, y); cout << "THERE IS NOTHING TO DELETE.";
            }
        }
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
bool sessionStart(string userID, string fname, string username, string role)
{
    if (!isSessionStated)
    {
        _SESSION[0] = userID;
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
            _SESSION[i] = '\0';
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
bool isValidate(string type, string content)
{
    if(type == "username")
    {
        for(int i = 0; content[i] != '\0' ; i++)
        {
            if(!((content[i] >= 48 && content[i] <= 57) || (content[i] >= 97 && content[i] <= 122) || content[i] == '_' || content[i] == '.'))
                return false;
        }
        return true;
    }
    else if(type == "packageTestCount")
    {
        int idx = 0;
        while (content[idx] != '\0')
        {
            if (!(content[idx] >= 50 && content[idx] <= 53))
                return false;
            if (content[idx] == ' ')
                return false;
            idx++;
        }
        if (idx != 1 )
            return false;
        
        return true;
    }
    else if(type == "labTestIDExists")
    {
        for(int i = 0; i < labTestCount; i++)
        {
            if(content == labTestID[i])
                return true;
        }
        return false;
    }
    else if(type == "packageDiscount")
    {
        int idx = 0;
        while (content[idx] != '\0')
        {
            if (!(content[idx] >= 48 && content[idx] <= 57))
                return false;
            if (content[idx] == ' ')
                return false;
            idx++;
        }
        if (idx != 2 || content[0] == 48)
            return false;
        return true;
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
string dateTime(string requirement)
{
    // Get the current time in seconds since the Unix epoch
    time_t now = time(0);

    // Convert the time_t to a string representation in the format YYYY-MM-DD
    char date[11];  // Buffer to hold the formatted date
    char time[11];  // Buffer to hold the formatted time
    strftime(time, sizeof(time), "%H:%M:%S", localtime(&now));
    strftime(date, sizeof(date), "%Y-%m-%d", localtime(&now));

    if(requirement == "date")
        return date;
    else if(requirement == "time")
        return time;
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