#include <iostream>

using namespace std;

//P功率(瓦特)=V電壓(伏特)*I電流(安培)
//E電能(瓦特小時)=P功率(瓦特)*t時間(小時)
//1度電=1千瓦時=1000瓦特小時 所以E(度)=P(瓦特)*t(小時)/1000

int main() {
    double usage;
    cout << "enter number: ";
    cin >> usage;


    double cost;
    
    
    
    if (usage <= 120) {
        cost = usage * 1.68;
    } else if (usage <= 330 && usage >=121) {
        cost = 120 * 1.68 + (usage - 120) * 2.45;
    } else if (usage <= 500 && usage >=331) {
        cost = 120 * 1.68 + 210 * 2.45 + (usage - 330) * 3.7;
    } else if (usage <= 700 && usage >=501) {
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + (usage - 500) * 5.04;
    } else if (usage <= 1000 && usage >=701){
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + 200 * 5.04 + (usage - 700) * 6.24;
    }else if (usage >=1001){
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + 200 * 5.04 + 300 * 6.24 + (usage - 1000) *8.46;
    
    }
    
    cout << "you shold cost: " << cost << " NTD" << endl;

    return 0;
}
