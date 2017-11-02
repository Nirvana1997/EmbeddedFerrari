#include "GPIOlib.h"

using namespace GPIO;

int main()
{
    init();
    
    //Move forward
    controlLeft(FORWARD,50);
    controlRight(FORWARD,50);

    int left = 0;
    int right = 0;

    for(int i=0;i<25;i++){
        delay(50);
        getCounter(&left,&right);
        if(left>right){
            turnTo(-2);
        } else if(left<right){
            turnTo(2);
        } else{
            turnTo(0);
        }
    }
    init();

    
    return 0;
}
