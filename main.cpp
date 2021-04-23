#include <iostream>
#include <vector>
#include <math.h>

using namespace std;
int hasharray[5][5][64];
//I wasn't able to create a 3d array as an input or output easily, so I created a public one that gets modifed by every process at the end

void to3D(vector<int> message);
vector<int> to1D();
vector<int> keccak(vector<int> message);
void theta();
void rho();
void pi();
void chi();
vector<int> generateRC();
void iota(int round);

int main()
{
    vector<int> message = {0};//substitute this for real input
    if(message.size() % 1088 == 1087){//if length mod 1088 = 1087, append 10*1 with 1087 0s
        message.push_back(1);
        for(int i = 0; i < 1087; i++){
            message.push_back(0);
        }
        message.push_back(1);
    }
    else{//if length mod 1088 = 0, append 10*1 with 1086 0s.  Append 10*1 where * number of 0s is equal to 1086 - length mod 1088
        message.push_back(1);
        for(int i = (message.size()%1088); i < 1086; i++){//check length mod 1088. If length mod 1088 = 1086, append 11 to end
            message.push_back(0);
        }
        message.push_back(1);
    }
    int remain = message.size();
    vector<int> prev;//previous hash doesn't exist, so create an empty vector
    for(int i = 0; i < 1600; i++){
        prev.push_back(0);
    }
    vector<int> chunk;
    while(remain > 0){//run keccak until empty
        vector<int> chunk;
        //each chunk of 1088 messages has a backpad of 512 0s
        for(int i = 0; i < 512; i++){
            chunk.push_back(0);
        }
        for(int i = 0; i < 1600; i++){
            chunk[i] = (chunk[i] + prev[i])% 2;// xor previous hash result
        }
        prev = keccak(chunk);
        remain = remain -1088;
    }
    for(int i = 0; i < 256; i++){
        cout<<chunk[i]<<endl;//choose first 256 bits of 1600bit output as output hash
    }
    return 0;
}

void to3D(vector<int> message){
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                hasharray[i][j][k] = message[64*((5*j)+i)+k];
            }
        }
    }
}
vector<int> to1D(){
    vector<int> message;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                message[64*((5*j)+i)+k] = hasharray[i][j][k];
            }
        }
    }
    return message;
}
vector<int> keccak(vector<int> message){
    to3D(message);//create a 3d array from the message
    for(int i = 0; i < 24; i++){
        theta();
        rho();
        pi();
        chi();
        iota(i);
        //24 rounds performing every function
    }
    return to1D();
    //turn the 3d array into 1d and save the result
}

void theta(){
    //cannot directly set 3d arrays to be equal, so have to do the slow way
    int aout[5][5][64];
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[i][j][k] = hasharray[i][j][k];
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                for(int l = 0; l < 5; l++){
                    aout[i][j][k] += hasharray[(i+4)%5][l][k] + hasharray[(i+1)%5][l][(k+63)%64];
                }
                aout[i][j][k] = (aout[i][j][k] + hasharray[i][j][k])%2;
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                hasharray[i][j][k] = aout[i][j][k];
            }
        }
    }
    //aout[i][j][k] = ain[i][j][k]+a[i-1][j from 0 to 4][k] + a[i+1][j from 0 to 4][k-1]
    //in case of negative, works mod 5 or mod 64 respectively. add 5 or 64 to math for ease of access
}

void rho(){
    int aout[5][5][64];
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[i][j][k] = hasharray[i][j][k];
            }
        }
    }
    int a[5][5] = {{0,36,3,41,8},
                   {1,44,10,45,2},
                   {62,6,43,15,61},
                   {28,55,25,21,56},
                   {27,20,39,8,14}};
    //bitwise rotate
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                int t = a[i][j];
                aout[i][j][k] = hasharray[i][j][(k+64-(t+1)*(t+2)/2)%64];
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                hasharray[i][j][k] = aout[i][j][k];
            }
        }
    }
    //aout[i][ j][k]  = ain[i][ j][k−(t+1)(t+2)/2]
    //t is equal to row i,column j in this array
}

void pi(){
    int aout[5][5][64];
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[i][j][k] = hasharray[i][j][k];
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[j][(3*j+2*i)%5][k] = hasharray[i][j][k];
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                hasharray[i][j][k] = aout[i][j][k];
            }
        }
    }
    //aout[j][3*j+2*i][k] = ain[i][j][k]
}

void chi(){
    int aout[5][5][64];
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[i][j][k] = hasharray[i][j][k];
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                aout[i][j][k] = (hasharray[i][j][k]+
                                 ((hasharray[(i+1)%5][j][k]+1)%2)*((hasharray[(i+2)%5][j][k])%2))%2;
            }
        }
    }
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0; k < 64; k++){
                hasharray[i][j][k] = aout[i][j][k];
            }
        }
    }
    //aout[i][j][k] = ain[i][j][k] + ((ain[i+1][j][k] + 1) x (ain[i+2][j][k]))
}

vector<int> generateRC(){
    vector<int> w = {1,0,0,0,0,0,0,0};
    vector<int> returnvalue = {1};
    for(int i = 0; i < 168; i++){
        int temp = w[0];
        for(int j = 1; j < 8; j++){
            w[j-1] = w[j];
        }
        w[7] = (temp+w[3]+w[4]+w[5])%2;//last bit is equal to oringal 0th bit xor 4th bit xor 5th bit xor 6th bit
        returnvalue.push_back(w[0]);
    }
    return returnvalue;
    //generates an array of rc numbers from 0 to 167
    //could be implemented differently if 24 rounds aren't used
}
void iota(int round){
    vector<int> rc = generateRC();
    for(int l = 0; l < 7; l++){
        hasharray[0][0][(int) pow(2,l)-1] = rc[l + 7*round];
    }

    //only changing at most 7 bits in the hash array
    //bit[0][0][2^l-1] = rc[l+7*(round)]
    //rc is x^t reduced in F2x [ x8 + x6 + x5 + x4 + 1]
    //means whenver we get x^8, we get x^6+x^5+x^4+1
    //x^9 becomes x7 + x6 + x5 + x
    //and x^10 becomes x8 + x7 + x6 + x2 -> (x6 + x5 + x4 + 1) + x7 + x6 + x2-> x^7+x^5+x^4+x^2+1 and so on
    //this is linear, so it can be written
    //use of a lfsr
    //generate the values of rc in generateRC


}
