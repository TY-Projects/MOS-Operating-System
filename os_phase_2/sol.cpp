#include <iostream>
#include <cstring>
#include <fstream>
#include<time.h>
#define N 40
#define SIZE 300
#define K 4
using namespace std;
struct PCB{
	int TTL;
	int TLL;
	int TTC;
	int LLC;
};
string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i] ;
        //cout<<a[i]<<endl;
    }
    return s;
}

class MOS{
    private:
    char memory[SIZE][K], IR[K], R[K], buffer[N];
    int m ;
    short IC,RA ;
    bool C;//toggle
    int SI = 0 , PI = 0 , TI = 0; //TODO SI = 3 , IN PDF
    ifstream fi;
    ofstream fo;
    bool mark[30];
    int PTR;
    int pte;
    PCB pcb;
    
    

    void read();
	void write();
    void terminate(int,int);
    int fill_buffer();
    void startExecution();
    void LR();
    void SR();
    void CR();
    void BT();
    void masterMode();
    void executeUserProgram();
    void store_buffer();
    void Map(int);
    bool check_operand(char , char);

    public:
    MOS();
    void initialize();
    void show(int m , int n);
    void Load();
    void close_all();
    int Allocate();
    void Map(int , int*);
};


int main()
{
	srand(time(0));
    MOS my_os ;
    my_os.Load();
    my_os.close_all();
    return 0;
}

int MOS::Allocate()
{
	int random = rand() % 30;
	while(mark[random])
	{
		random = rand() % 30;
	}
	mark[ random ] = true;
	return random;
}
bool MOS::check_operand(char a , char b)
{
    if(!( ( (a<='9'&&a>='0') || a==' ') && ( (b<='9'&&b>='0')|| b==' ') )){
        PI = 2;
        return false;
    }
    return true;
}
void MOS::Map(int VA)
{
    int i = 0 , pte_i ;
	pte = PTR+ (int)VA/10;
    string temp;
    for(i = 0 ;i<K;i++)
    { 
        temp+=memory[pte][i];
    }
    temp[i]='\0';
    if(temp == "    ")
    {
        PI = 3;//Page Fault
        //cout<<"page fault for VA : "<<VA<<endl;
        return;
    }
    pte_i = stoi(temp);
    
	RA = pte_i*10 + VA%10;
}
void MOS::read(){
    string data;
    getline(fi, data);
    int k  =0;
    if(data.substr(0,4) == "$END")
    {
        cout<<"EM:"<<1<<" Out of Data Error=> Terminating process"<<endl<<endl;
        terminate(1,-1);
        //Load();
        return;
    }
    for(int i = 0 ; i< 10 ; i++){
        for(int j = 0 ; j<K ; j++){
            if(data[k]=='\0')
                break;
            memory[RA+i][j] = data[k++];
        }
    }	
    cout<<"Data read successfully\n\n";
}

void MOS::write(){
    string data = "";
    string loc(1,IR[2]);
    pcb.LLC++;
    if(pcb.LLC > pcb.TLL)
    {
        cout<<"EM:"<<2<<" Line Limit Error=> Terminating process"<<endl<<endl;
        terminate(2,-1);
        //Load();
        return;

    }
    for(int i = 0 ; i < 10 ; i++){
        for(int j = 0 ; j<K ; j++){
        data +=  memory[RA+i][j];
        }
    }
    fo<<data<<endl;
    cout<<"Data written successfully\n\n";
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

void MOS::show( int m = 300 , int n = 4){
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
    cout<<"----------Starting Operating System----------\n\n";
    m = 0;
    string temp;
    int eof = 0;
    while(eof != 1){ 
        eof = fill_buffer();
        
        if(buffer[0] == '$')
        {
            cout<<"Encountered Control card: ";
            if(buffer[1] == 'A')//$AMJ 
            {
                cout<<"$AMJ\n\nLoading new Job in to memory....!!!\n";
                fo<<"Job: "<<buffer[4]<<buffer[5]<<buffer[6]<<buffer[7]<<endl<<endl;
                //initialize PCB
                pcb.TTL = stoi(string(1,buffer[8])+string(1,buffer[9])+string(1,buffer[10])+string(1,buffer[11]));
                pcb.TLL = stoi(string(1,buffer[12])+string(1,buffer[13])+string(1,buffer[14])+string(1,buffer[15]));
                pcb.TTC = 0;
                pcb.LLC = 0;
                cout<<"Initializing PCB for job: "<<buffer[4]<<buffer[5]<<buffer[6]<<buffer[7]<<endl;
                cout<<"PCB=> TTL:"<<pcb.TTL<<" TLL:"<<pcb.TLL<<" TTC:"<<pcb.TTC<<" LLC:"<<pcb.LLC<<endl;
            
                initialize();
                PTR = Allocate()*10;
                cout<<"PTR allocated. PTR=>"<<PTR<<endl<<endl;
                //cout<<"PTR : "<<PTR<<endl;
                // continue;
            }
            else if(buffer[1] == 'D')//$DTA
            {
                cout<<"$DTA\nStarting job execution\n\n";
                startExecution();
            }
            else //$END
            {
                cout<<"$END\nJob Removed\n\n";
                // fo<<"Job "<<buffer[4]<<buffer[5]<<buffer[6]<<buffer[7]<<" completed\n";
                // fo<<"--------------------------------------------------------------\n\n";
                // continue;
            }
        }
        else //its a prgm card
        {

                if(buffer[0]!='\n'){
                cout<<"Encountered Program card.\n";
                		
               	    m = Allocate()*10;
                    //cout<<"Program card : "<<m<<endl;
               	    int itr = PTR;
               	    while(memory[itr][3] != ' ')
               	    {
               	    	itr++;
               	    }
                    cout<<"Allocating a frame for new program card (page no:"<<(itr-PTR)<<")\nFrame number: "<<m<<endl<<endl;
               	    memory[itr][3] = '0'+ (m%100)/10;  
               	    memory[itr][2] = '0'+ (m/100);
               	    memory[itr][1] = '0';
               	    memory[itr][0] = '0';

                    store_buffer(); 
                    //m+=10;
                }
                else{
                    continue;
                }
            // }
            
        }
        cout<<"-------------------------------------------------------------\n\n";
    }  
}

void MOS::executeUserProgram(){
    //cout<<"Inside Slave MOde"<<endl;
    while(1)
    {
        if(PTR == -1)
            return;

        //map IC to RA
        Map(IC);
        //cout<<"IC TO RA : "<<IC<<" : "<<RA<<endl;
        //initilaize IR
        for(int k = 0 ; k<K ; k++){
            IR[k] = memory[RA][k];
        }
        if(IR[0]=='H' && IR[1] ==' ')
        {
            //cout<<"H called"<<endl;
            SI = 3;
        }
        else{
            //inc IC
            IC++;
            
            if(check_operand(IR[2],IR[3])){
                int operand = stoi(string(1,IR[2])+string(1,IR[3]));
                Map(operand);
                if(!PI)
                {
                
                    string temp =  string(1,IR[0])+string(1, IR[1]);
                    if(temp == "LR")
                    {
                        LR();  
                    }               
                    else if(temp == "SR")
                    {
                        SR(); 
                    }
                    else if(temp == "CR")
                    {
                        CR();  
                    }
                    else if(temp == "BT")
                    {
                        BT();                       
                    }
                    else if(temp == "GD")
                    {
                        SI = 1;                      
                    }
                    else if(temp == "PD")
                    {
                        SI = 2;           
                    }
                    else
                    {
                        PI = 1; //operation error
                    } 
                    pcb.TTC++;

                }
                if(pcb.TTC > pcb.TTL)
                {                   
                    TI = 2; //Total Time Limit Exceeded     
                }
            }
        }
         if(SI || PI || TI) //anyone is non zero
        {
            masterMode();
        }
        else
        {
            //cout<<"all good"<<endl;
        }                                       
    }
 
}

void MOS::startExecution(){
    IC = 0;
    cout<<"-------------------------------------------------------------\n";
    cout<<"Memory Before execution: "<<endl;
    show();
    cout<<"-------------------------------------------------------------\n\n";
    executeUserProgram();
}

void MOS::LR(){
    for(int i = 0 ; i<K ; i++){
        R[i] = memory[RA][i];
    }
    
}

void MOS::SR(){
    for(int i = 0 ; i<K ; i++){
        memory[RA][i] = R[i];
    }
}

void MOS::CR(){
    C = true;
    for(int i = 0 ; i<K ; i++){
        if (memory[RA][i] != R[i]){
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
    cout<<"Inside master mode with PI : "<<PI<<" : SI : "<<SI<<" : TI : "<<TI<<endl;
    if(TI == 0)
    {
        // CASE TI=0 AND PI
        if(PI == 1)
        {
            cout<<"EM:"<<4<<" Operation Error=> Terminating process\n\n";
            terminate(4,-1);
            
        }
        else if(PI == 2)
        {
            cout<<"EM:"<<5<<" Operand Error=> Terminating process\n\n";
            terminate(5,-1);
           
        }
        else if(PI ==3)
        {
            string temp = string(1,IR[0])+string(1, IR[1]);
            if(temp=="PD" || temp=="LR" || temp=="CR") //invalid page fault
                {cout<<"EM:"<<6<<" Invalid Page Fault=> Terminating process\n\n";
                terminate(6,-1);}             
            else
            {
                //Page Fault
                int x = Allocate()*10;
                //cout<<"updated page table with : "<<x<<endl;
                int itr = PTR+stoi(string(1,IR[2])+string(1, IR[3]))/10;
                cout<<"Valid Page Fault.\nAllocating page "<<(itr-PTR)<<" to frame "<<x<<endl<<endl;
                // while(memory[itr][3] != ' ')
                // {
                //     itr++;
                // }
                //updated page table
                memory[itr][3] = '0'+ (x%100)/10;  
                memory[itr][2] = '0'+ (x/100);
                memory[itr][1] = '0';
                memory[itr][0] = '0';  

                IC--;
                PI = 0;
                executeUserProgram();           
            }            	    
        }
        // CASE TI=0 AND SI
        else if(SI == 1)
        {
            read();
            SI = 0;
            //executeUserProgram();            
        }
        else if(SI == 2)
        {
            write();
            SI = 0;
            //executeUserProgram();         
        }
        else{
            cout<<"Job Completed Successfully\n\n";
            terminate(0,-1);
        }

    }
    else
    {
        // CASE TI=2 AND PI
        if(PI == 1)
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded\nAnd EM:4 Operation Error=> Terminating process\n\n";
            terminate(3,4);
        }
        else if(PI == 2)
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded\nAnd EM:5 Operand Error=> Terminating process\n\n";
            terminate(3,5);
        }           
        else if(PI ==3)
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded\nAnd Page Fault=> Terminating process\n\n";
            terminate(3,-1);
        }           
        // CASE TI=2 AND SI
        else if(SI == 1)
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded=> Terminating process\n\n";
            terminate(3,-1);
        }           
        else if(SI == 2)
        {
            cout<<"EM:"<<3<<" Time Limit Exceeded\nAnd EM:2 Line Limit Error=> Terminating process\n\n";
            terminate(3,-1);          
        }
        else if(SI == 3)
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded=> Terminating process\n\n";
            terminate(0,-1);
        }
        else
        {    
            cout<<"EM:"<<3<<" Time Limit Exceeded=> Terminating process\n\n";
            terminate(3,-1);
        }

    }
    
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
    
    for(int i =  0 ; i<30 ; i++)
        mark[i] = false;
    

    PI = 0 , SI = 0 , TI = 0;
    
}

void MOS::close_all(){
    fi.close();
    fo.close();
}

void MOS::terminate(int EM1 , int EM2){
    PTR = -1;
    //cout<<"Terminated with EM : "<<EM1<<" "<<EM2<<endl;
    string temp = "Terminated due to ";
    if(EM1 == 0)
        fo<<"Terminated Succesfully"<<endl;
    else if(EM1 == 1)
        fo<<temp<<"Out of Data Error"<<endl;
    else if(EM1 == 2)
        fo<<temp<<"Line Limit Error"<<endl;
     else if(EM1 == 6)
        fo<<temp<<"Invalid Page Fault"<<endl;
     else if(EM1 == 4)
        fo<<temp<<"Operation Code Error"<<endl;
     else if(EM1 == 5)
        fo<<temp<<"Operand Error"<<endl;
     else {
        string temp2 = "Time Limit Exceeded Error & ";
        if(EM2 == -1)
        fo<<temp<<"Time Limit Exceeded Error"<<endl;
        else if(EM2 == 4)
            fo<<temp<<temp2<<" Operation Code Error"<<endl;
        else 
            fo<<temp<<temp2<<" Operand Error"<<endl;      
     }
    
    fo<<"Register Status - ";
    fo<<"| IC : "<<IC<<"| IR : "<<convertToString(IR,K)<<" | TTC : "<<pcb.TTC<<" | LLC : "<<pcb.LLC<<" |"<<endl;
    fo<<"--------------------------------------------------------------\n\n";
    cout<<"--------------------------------------------------------------\n\n";
    cout<<"Memory After execution: "<<endl;
    show();
    fo<<endl;
} 

