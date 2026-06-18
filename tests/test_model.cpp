#include "Models.h"
#include "Repository.h"
#include "Utils.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
    // 1) Utils
    assert(Utils::field("S001,Blood,desc here", 1) == "Blood");
    assert(Utils::field("a,b,c", 2) == "c");
    assert(Utils::nextId("S009") == "S010");
    assert(Utils::nextId("LT099") == "LT100");
    assert(Utils::nextId("M001") == "M002");
    cout << "[ok] Utils field/nextId\n";

    // 2) Load the REAL specimen file (read-only) and show parsing works
    Repository<Specimen> specimens("DataBase/specimen.txt", "S001");
    specimens.load();
    cout << "[info] loaded " << specimens.count() << " specimens from disk:\n";
    for (int i = 0; i < specimens.count(); i++)
        cout << "       " << specimens.at(i).id << " | " << specimens.at(i).name
             << " | " << specimens.at(i).description << "\n";
    assert(specimens.count() >= 1);
    assert(specimens.at(0).id == "S001");

    // 3) Full CRUD round-trip on a TEMP file (never touches real data)
    Repository<Specimen> tmp("test_tmp.txt", "S001");
    tmp.load(); // empty
    assert(tmp.empty());
    assert(tmp.nextId() == "S001");

    Specimen a; a.id = tmp.nextId(); a.name = "Urine"; a.description = "sample A";
    tmp.add(a);
    Specimen b; b.id = tmp.nextId(); b.name = "Blood"; b.description = "sample B";
    tmp.add(b);
    assert(tmp.count() == 2);
    assert(tmp.at(1).id == "S002");
    assert(tmp.nextId() == "S003");

    tmp.store();
    Repository<Specimen> reload("test_tmp.txt", "S001");
    reload.load();
    assert(reload.count() == 2);
    assert(reload.at(0).name == "Urine");
    assert(reload.at(1).description == "sample B");

    assert(reload.indexOf("S002") == 1);
    assert(reload.indexOf("ZZZ") == -1);
    assert(reload.removeById("S001") == true);
    assert(reload.count() == 1);
    assert(reload.at(0).id == "S002");
    cout << "[ok] Repository CRUD + CSV round-trip\n";

    remove("test_tmp.txt");
    cout << "ALL MODEL TESTS PASSED\n";
    return 0;
}
