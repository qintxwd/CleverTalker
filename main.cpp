/*
 * main2.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: qinyinghao
 */
#include <iostream>
#include <string>

#include "BaiduTuling.h"

using namespace std;
using namespace qyh;
int main(int argc, char const *argv[]) {

    BaiduTuling bt;
    std::string err;
    if (!bt.init(err)) {
        cout << err << endl;
        return -1;
    }
    while (true) {
        sleep(5);
    }
    return 0;
}
