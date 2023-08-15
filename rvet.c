#include <stdio.h>
#include <string.h>
#include <mpi.h>

typedef struct Clock {
    int p[3];
} Clock;

void Event(int pid, Clock *clock) {
    clock->p[pid]++;
}

void Send(int sender_pid, int receiver_pid, Clock *clock) {
    Event(sender_pid, clock);
    printf("P%d (%d, %d, %d)\n", sender_pid, clock->p[0], clock->p[1], clock->p[2]);
    MPI_Send(clock, sizeof(Clock), MPI_BYTE, receiver_pid, 0, MPI_COMM_WORLD);
}

void Receive(int sender_pid, int receiver_pid, Clock *clock) {
    Clock received_clock;
    MPI_Recv(&received_clock, sizeof(Clock), MPI_BYTE, sender_pid, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < 3; i++) {
        clock->p[i] = (clock->p[i] > received_clock.p[i]) ? clock->p[i] : received_clock.p[i];
    }

    Event(receiver_pid, clock);
    printf("P%d (%d, %d, %d)\n", receiver_pid, clock->p[0], clock->p[1], clock->p[2]);
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
