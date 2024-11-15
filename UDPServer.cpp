#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cmath>

using namespace std;

// to calculate the monthly payment... = (L*R)/1-[1/1+R)]^N

double Monthly_payment_calc(double loan_amount, int loan_period, double annual_interest_rate)
{
    double monthly_interest_rate = 0;
    int num_of_payments = 0;
    double denom = 0;
    double monthly_paym = 0;

    monthly_interest_rate = annual_interest_rate / 12;
    // get decimal....
    monthly_interest_rate = monthly_interest_rate / 100;

    // get num of payments by load period * # of months in a year...
    num_of_payments = loan_period * 12;

    //(1/1+R)^N == 1/(1+R)^N ... hence, ...
    denom = 1 - 1 / pow(1 + monthly_interest_rate, num_of_payments);
    monthly_paym = (loan_amount * monthly_interest_rate) / denom;
    return monthly_paym;
}

int main()
{

    // variables:
    //==========================================================
    double monthly_paym = 0;
    double total_paym = 0;

    // string that will contain contents sent from client
    string buffer_2;

    int sockfd;
    char buffer[1024];
    int n = 0;
    double annual_interest_rate, loan_amount;
    int loan_period;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t len;
    socklen_t len2;
    string info_str;
    //==========================================================

    // assign length of client_addr and serv_addr to len/len2
    len = sizeof(client_addr);
    len2 = sizeof(serv_addr);

    // creating UDP socket ...
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // initialize struct...
    memset(&client_addr, 0, len);
    memset(&serv_addr, 0, len2);

    // specify ipv4, accept any connection, and properly set port # of serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    // binding socket to server address...
    bind(sockfd, (const struct sockaddr *)&serv_addr, len2);

    while (1)
    {

        // this is essential, makes so that the correct values will be outputted
        // from after 1st prompt of client on same server session
        buffer_2.clear();

        // getting data from client side interaction of UDP process...
        // recvfrom() function gets the data that was sent by the client...
        // gets the data sent to the given socktfd socket (ie senders IP address)
        n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);

        // essentially appends what the client sent through the socket to the buffer_2 string
        buffer_2.append(buffer, n);

        // test...
        // cout << buffer_2 <<endl<<endl;

        // we are just reading what we expected to be sent to us, in this instance we know we appended the info to buffer_2
        // string, that info was essentially the CML args the user gave on the client side. We constructed it earlier by
        // separating it by spaces, so we shall read it like that and read it into the correct order of vars
        // while being mindful of their respective data types
        sscanf(buffer_2.c_str(), "%lf %d %lf", &loan_amount, &loan_period, &annual_interest_rate);

        // calling our constructed function with the given vars that we inputted values to (based on client side)
        monthly_paym = Monthly_payment_calc(loan_amount, loan_period, annual_interest_rate);

        // we have monthly, easy calculation for total payment...
        total_paym = monthly_paym * (loan_period * 12);

        // appropriately convert vars to type string assign it to main string we want to send back to client
        info_str = to_string(monthly_paym) + " " + to_string(total_paym);

        // sends the string with the computed values (from our Monthly_payment_calc() function) that was computed on the server side and is sending it to the client
        // hence, "I am assumming the client program is not capable of doing the math itself".
        sendto(sockfd, info_str.c_str(), info_str.length(), 0, (const struct sockaddr *)&client_addr, len);
    }
    // close sock...
    close(sockfd);
    return 0;
}
