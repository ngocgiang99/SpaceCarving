#include <bits/stdc++.h>
#include <random>

using namespace std;
const int MAXN = 5e6;

float x[MAXN], y[MAXN], z[MAXN];

int main() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(-10.0, 10.0);
    //freopen("output.txt",stdout, "w");
    
    ifstream iFileT("inp1.txt",ios::in);
    cout<<fixed<<setprecision(15);

    int n;
    iFileT>> n;
    for(int i = 0; i < n; ++i) {
        //float x, y, z;
        iFileT>> x[i] >> y[i] >> z[i];
    }
    iFileT.close();

    ofstream oFileT("inp1.txt",ios::out);

    cout<< fixed <<setprecision(7);
    oFileT<< n <<endl;
    for(int i = 0; i < n; ++i) 
    oFileT<< x[i] <<' '<< y[i] <<' '<< z[i] <<endl;
    
    return 0;
}