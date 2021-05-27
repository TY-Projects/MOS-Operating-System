#include <iostream>
#include <cstring>
#include <fstream>
#define N 40
#define SIZE 100
#define K 4
using namespace std;

class MOS{
    private:
    char memory[SIZE][K], IR[K], R[K], buffer[N];
    int m ;
    short IC;
    bool C;//toggle
    int SI = 0;
    ifstream fi;
    ofstream fo;

    void read();
	void write();
    void terminate();
    int  fill_buffer();
    void store_buffer();
    void startExecution();
    void LR();
    void SR();
    void CR();
    void BT();
    void masterMode();
    void executeUserProgram();

    public:
    MOS();
    void initialize();
    void show(int m , int n);
    void Load();
    void close_all();
};


int main()
{
    MOS my_os ;
    my_os.Load();
    my_os.close_all();
    return 0;
}




void MOS::read(){
    string data;
    getline(fi, data);
    int k  =0;
    if(data.substr(0,4) == "$END")
        return;
    for(int i = 0 ; i<=9 ; i++){
        for(int j = 0 ; j<K ; j++){
            if(data[k]=='\0')
                break;
            memory[(IR[2]-'0')*10+(IR[3]-'0')+i][j] = data[k++];
        }
    }	
}

void MOS::write(){
    string data = "";
    for(int i = 0 ; i < 10 ; i++){
        for(int j = 0 ; j<K ; j++){
        data +=  memory[(IR[2]-'0')*10+(IR[3]-'0')+i][j];
        }
    }
    fo<<data<<endl;
    return;
} 

int MOS::fill_buffer(){
    int i = 0;
    char c;

    memset(buffer, '\0', sizeof(buffer));
    
    if(fi.is_open()) {
        int count = 0;
        while(fi.good()) {
            fi.get(c);
            buffer[count] = c;
            count++;
            if(count == 40){
                return 0;
            }
            if(c=='H'){
                buffer[count] = ' ';
                count++;
                buffer[count] = ' ';
                count++;
                buffer[count] = ' ';
                count++;
            }
            if(c=='\n'){
                // buffer[count] = '\n';
                return 0;
            }
        }
    }


    return 1;
}

void MOS::show( int m = 100 , int n = 4){
    cout<<"Memory looks like: ";
    for(int i = 0 ; i<m ; i++)
    {
        if(i%10==0){
            cout<<endl;
            cout<<i<<'\t';
        }
        for(int j = 0 ; j<n ; j++)
        {
            if(memory[i][j]==' ' || memory[i][j]=='\n')
                cout<<'_';
            else
                cout<<memory[i][j];
        }
        cout<<'\t';
        
    }
    cout<<endl;
}

void MOS::Load(){
    m = 0;
    string temp;
    int eof = 0;
    while(eof != 1){ 
        eof = fill_buffer();
        if(buffer[0] == '$')
        {
            if(buffer[1] == 'A')//$AMJ 
            {
                // for(int i = 0; i<40; i++)
                //     cout<<buffer[i];
                // cout<<endl;
                fo<<"Job: "<<buffer[4]<<buffer[5]<<buffer[6]<<buffer[7]<<endl<<endl;
                initialize();
                
                continue;
            }
            else if(buffer[1] == 'D')//$DTA
            {
                startExecution();
            }
            else //$END
            {
                fo<<"Job "<<buffer[4]<<buffer[5]<<buffer[6]<<buffer[7]<<" completed\n";
                fo<<"--------------------------------------------------------------\n\n";
                continue;
            }
        }
        else //its a prgm card
        {
            if(m == 100){
                cout<<"memory exceeded"<<endl;
                exit(0);
                
            }
            else{
                if(buffer[0]!='\n'){
                    store_buffer(); 
                       
                    m+=10;
                }
            }
            
        }
    }  
}

void MOS::executeUserProgram(){
    while(memory[IC][0]!='H')
    {
        //initilaize IR
        for(int k = 0 ; k<K ; k++){
            IR[k] = memory[IC][k];
        }
        //inc IC
        IC++;
        string temp =  string(1,IR[0])+string(1, IR[1]);
        if(temp == "LR")
            LR();                
        else if(temp == "SR")
            SR();
        else if(temp == "CR")
            CR();
        else if(temp == "BT")
            BT();
        else{
            if(temp == "GD")
                SI = 1;
            else if(temp == "PD")
                SI = 2;    
            if(SI != 0)
                masterMode();                          
        }         
        
    }

    if(IR[0] == 'H')
        SI = 3; 
    masterMode();    
}

void MOS::startExecution(){
    IC = 0;
    executeUserProgram();
    show();

}

void MOS::LR(){
    for(int i = 0 ; i<K ; i++){
        R[i] = memory[(IR[2]-'0')*10+(IR[3]-'0')][i];
    }
    
}

void MOS::SR(){
    for(int i = 0 ; i<K ; i++){
        memory[(IR[2]-'0')*10+(IR[3]-'0')][i] = R[i];
    }
}

void MOS::CR(){
    C = true;
    for(int i = 0 ; i<K ; i++){
        if (memory[(IR[2]-'0')*10+(IR[3]-'0')][i] != R[i]){
            C = false;
            return;
        }
    }
}

void MOS::BT(){
    if(C){
        IC = (IR[2]-'0')*10+(IR[3]-'0');
    } 
}

void MOS::masterMode(){
    if(SI == 1)
        read();
    else if(SI == 2)
        write();
    else{
        terminate();
    }
    SI = 3;
}

void MOS::store_buffer(){
    int k= 0;
    for(int i = 0 ; i<=9 ; i++){
        for(int j = 0 ; j<K ; j++){
            if(buffer[k]=='\0')
                break;
            memory[m+i][j] = buffer[k++];
            
        }
    }
}

MOS::MOS(){
    fi.open("input.txt",ios::in); 
    fo.open("output.txt",ios::out); 
}
void MOS::initialize(){
        m = 0;
    for(int i = 0 ; i<SIZE ; i++)
        for(int j = 0 ; j<K; j++)
            memory[i][j] = ' ';
    
}

void MOS::close_all(){
    fi.close();
    fo.close();
}

void MOS::terminate(){
    fo<<endl;
} 