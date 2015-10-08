#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;

typedef struct{ // data struct
    long int  sequence_size;
    long long int  fibonacci[93];
} Shared_data;

int main()
{
    cout << "Digite  0<= numero < 93 da sequencia de fibonacci:" << endl;
    int tam;
    cin>> tam;
    while(tam<=-1 || tam > 92){
        cout << "Digite 0<= numero < 93 da sequencia de fibonacci: " << endl;
        cin>> tam;
    }

    pid_t pid; // data type for represent process id
    pid= fork();
    cout << "Pid pos chamada do fork: "<< pid<<endl;

    if(pid<0){  // sinalize fork errors
        perror("fork");
        exit(1);
    }

    if(pid==0){
        cout << "   ===Inicio processo filho==="<<endl;
        cout << "   Pid filho dentro do filho: "<< pid<<endl;
        cout << "   Getpid filho: "<< getpid()<<endl;
        sleep(0); // wait father do  shared_memory->sequence_size=tam

        int segment_id;
        key_t key;
        key=5670; // can be any value, but it must be the same for all processes
        unsigned long size = sizeof(Shared_data);
        segment_id = shmget( key,  size, S_IRUSR | S_IWUSR); // get allocated segment with the key

        if (segment_id == -1) { //analyse possible error with shared memory identifier associated with key
                perror("shmget falhou");
                return 0;
        }
        //attaches the shared memory segment associated with the shared memory identifier
        Shared_data *shared_memory =   (Shared_data*)shmat(segment_id, NULL, 0);

        if ((void*)shared_memory == (void*)-1) {  //analyse possible error with attaches the shared memory segment
                perror("shmat fhalou");           //associated with the shared memory identifier
            }
        cout<<"   Segmento de memoria compartilhada dentro  do filho "<<segment_id<<endl;
        cout<<"   Anexada no endereco "<<(void*)shared_memory<<endl;
        long long  int fibo=1, aux1=1, aux2=0;
        shared_memory->fibonacci[0]=fibo;
        for(int x=2; x<=shared_memory->sequence_size; x++){
            fibo=aux1+aux2;
            aux2=aux1;
            aux1=fibo;
            shared_memory->fibonacci[x-1]=fibo;
        }
        shmdt(shared_memory);// detach memory segment
        cout << "   ===  Fim processo filho  ==="<<endl;
        return 0;
    }


    if (pid>0){
        cout << "======Inicio processo Pai======"<<endl;
        cout << "Pid filho dentro do pai: "<< pid<<endl;
        cout << "Getpid pai: "<< getpid()<<endl;

        int segment_id;
        key_t key;
        key=5670; // can be any value, but it must be the same for all processes
        unsigned long size = sizeof(Shared_data);
        segment_id = shmget( key,  size, IPC_CREAT | S_IRUSR | S_IWUSR); // create and allocates segment with the key

        if (segment_id == -1) { //analyse possible error with shared memory identifier associated with key
                perror("shmget falhou");
                return 0;
        }
        //attaches the shared memory segment associated with the shared memory identifier
        Shared_data *shared_memory =   (Shared_data*)shmat(segment_id, NULL, 0);

        if ((void*)shared_memory == (void*)-1) {//analyse possible error with attaches the shared memory segment
            perror("shmat fhalou");           //associated with the shared memory identifier
            }
        shared_memory->sequence_size=tam;
        cout << "Segmento de memoria compartilhada dentro  do pai "<<segment_id<<endl;
        cout << "Anexada no endereco "<<(void*)shared_memory<<endl;
        cout << "Pai espera filho"<<endl;
        wait(NULL);
        cout << "Filho acabou, pai continua"<<endl;
        cout << "Sequencia:  ";
        for (int i=0; i<shared_memory->sequence_size-1;i++){
            cout << shared_memory->fibonacci[i]<<", ";
        }
        cout << shared_memory->fibonacci[shared_memory->sequence_size-1];
        cout <<endl;

        cout << "Fibo dentro do pai, Fibonaccci("<<shared_memory->sequence_size<<"): "<<shared_memory->fibonacci[shared_memory->sequence_size-1]<<endl;
        shmctl(segment_id, IPC_RMID, NULL); //  remove shared memory segment
        cout << "====== Fim processo pai  ======"<<endl;
    }

    return 0;
}
