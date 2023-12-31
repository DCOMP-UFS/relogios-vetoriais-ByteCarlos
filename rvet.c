#include <stdio.h>
#include <string.h>
#include <mpi.h>

typedef struct Clock {
    int p[3];
} Clock;

// Incrementa o clock
void Event(int pid, Clock *clock) {
    clock->p[pid]++;
}

void Send(int sender_pid, int receiver_pid, Clock *clock) {
    clock->p[sender_pid]++; // Incrementa o clock do processo que manda
    printf("P%d (%d, %d, %d)\n", sender_pid, clock->p[0], clock->p[1], clock->p[2]); // Imprimindo o clock do processo que manda
    MPI_Send(clock, sizeof(Clock), MPI_BYTE, receiver_pid, 0, MPI_COMM_WORLD); // envia para o processo que irar receber (receiver_pid)
}

void Receive(int sender_pid, int receiver_pid, Clock *clock) {
    Clock received_clock; // Clock auxiliar para o processo que recebe
    MPI_Recv(&received_clock, sizeof(Clock), MPI_BYTE, sender_pid, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recebe do processo que mandou (sender_pid), e armazena no received_clock
    
    for (int i = 0; i < 3; i++) { // compara o clock local com o que foi recebido
        clock->p[i] = (clock->p[i] > received_clock.p[i]) ? clock->p[i] : received_clock.p[i]; // se o clock recebido for maior que o clock local ele substitui pelo clock recebido
    }
    
    clock->p[receiver_pid]++; // Incrementa o clock do processo que recebe (receiver_pid)
    printf("P%d (%d, %d, %d)\n", receiver_pid, clock->p[0], clock->p[1], clock->p[2]); // Imprime o clock do que recebe
}

void process0() {
    Clock clock = {{0, 0, 0}};
    Event(0, &clock);
    printf("P%d (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);

    Send(0, 1, &clock);
    Receive(1, 0, &clock);

    Send(0, 2, &clock);
    Receive(2, 0, &clock);

    Send(0, 1, &clock);

    Event(0, &clock);
    printf("P%d (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
}

void process1() {
    Clock clock = {{0, 0, 0}};

    Send(1, 0, &clock);
    Receive(0, 1, &clock);
    Receive(0, 1, &clock);
}

void process2() {
    Clock clock = {{0, 0, 0}};
    
    Event(2, &clock);
    printf("P%d (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
    Send(2, 0, &clock);
    Receive(0, 2, &clock);
}

int main(void) {
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        process0();
    } else if (my_rank == 1) {
        process1();
    } else if (my_rank == 2) {
        process2();
    }

    MPI_Finalize();

    return 0;
}
