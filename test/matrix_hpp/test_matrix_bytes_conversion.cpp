#include <iostream>
#include <fstream>
#include <sstream>
#include <matrix.hpp>

using namespace std;
using namespace nm;

int main() {
    Matrix mat(4,4,{0,1,2,3,4,5,6,7,8,9,10,11});
    string dat=mat.toBytes();
    cout<<string(mat)<<endl;
    ofstream fo("dat",ios::binary);
    fo.write(dat.c_str(),dat.size());
    fo.close();
    ifstream fi("dat",ios::binary);
    stringstream t;
    t<<fi.rdbuf();
    string s=t.str();
    Matrix matr=Matrix::fromBytes(s);
    cout<<string(matr)<<endl;
}
