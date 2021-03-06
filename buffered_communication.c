#include "mbed.h"
#include "C12832.h"
 
//RawSerial USART(PA_11, PA_12);

C12832 lcd(D11, D13, D12, D7, D10);
enum Direction {sender, receiver, senderReceiver};
class Buffer {
     private:
        int* buf;
        int size, count;
        int in;
        int out;
    
    public:
        Buffer();
        Buffer (int s);
        ~Buffer();
        
        void put (int i){
            
            buf[in] = i;
            ++count;
            in=(in+1) % size;
        }
        int get(){
            int x;
            
            x = buf[out];
            buf[out]=0;
            --count;
            out=(out+1) % size;
            return (x);
            }
        bool isFull(void){
            return (count >= size);
            }
        bool isEmpty(void){
            return (count == 0);
            }
    
    
    
   
};
Buffer::Buffer(int s){
    size = s;
    in = 0;
    out = 0;
    count = 0;
    buf = new int[size];
    }
class ASCommunicator {
    private:
        RawSerial rs;
        int baudRate;
        Direction commType;
        //Buffer buf;
    public:
        //enum Direction {sender, receiver, senderReceiver}
        ASCommunicator(PinName txp, PinName rxp,int br, Direction commRole):  rs(txp, rxp){
            baudRate = br;
            rs.baud(baudRate);
            commType = commRole ;
            }
        
        void setBaudRate(float br){rs.baud(br);}
        
        float getBaudRate(void){return baudRate;}
        
        void setDirection(Direction commRole){commType = commRole;}
        
        Direction getDirection(void){return commType; }
        
        bool canTx(void){
            if(rs.writeable()){
                return 1;
                }
            else{
                return 0;
                }
            
            }// returns true if USART can send
        
        bool canRx(void){
            if(rs.readable()){
                return 1;
                }
            else {
                return 0;
                }
            
            } // returns true if USART can receive
        
        void Tx(char c){
            rs.putc(c); // send a char
         
        }
        char Rx(void){
            char s = rs.getc(); 
            return s;
            } // receive a char
};
class BufferedASCommunicator : public ASCommunicator {
    private:
        
        Buffer Rxbuf;
        Buffer Txbuf; 
        Timeout to;
        Timeout tt;
    public:
        BufferedASCommunicator(PinName txp, PinName rxp, Direction commRole ): ASCommunicator(txp, rxp, 115200, senderReceiver), Rxbuf(10), Txbuf(10){
            
            }
        void Rxcallback(void){
            Rxbuf.put(Rx());
            to.attach(callback(this, &BufferedASCommunicator::Rxcallback),0.1);
            }
        void Txcallback(void){
            Tx(Txbuf.get());
            tt.attach(callback(this, &BufferedASCommunicator::Txcallback),0.1);
            }
        char getvalue(void){
            Rxcallback();
            return Rxbuf.get();
            }
        void setvalue(char s){
            Txbuf.put(s);
            Txcallback();
            }
        
        
};

int main() {
    int i = 2;
    lcd.locate(0,15);
    ASCommunicator a(PA_11, PA_12, 115200, senderReceiver);
    BufferedASCommunicator asc(PA_11, PA_12,  senderReceiver);
    while(true) {
        asc.setvalue(i);
        //wait (1.0);
        i++;
        lcd.locate(0,15);
        lcd.printf("Rx %d ", asc.getvalue());
       
    }
}

   

