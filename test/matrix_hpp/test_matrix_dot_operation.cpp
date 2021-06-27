#include <iostream>
#include <matrix.hpp>

using namespace std;
using namespace nm;

int main() {
    Matrix a=Matrix::random(3,3),b=Matrix::random(3,3);
    Matrix c=dot(a,b);
    cout<<string(a)<<endl;
    cout<<string(b)<<endl;
    cout<<string(c)<<endl;
}
