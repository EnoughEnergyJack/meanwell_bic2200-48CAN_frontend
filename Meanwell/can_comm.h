#ifndef CAN_COMM_H
#define CAN_COMM_H

void initCAN();
void checkOutputReal();
void sendSetpoints();
void sendOutputState();
void checkSetpointEcho();

#endif // CAN_COMM_H 