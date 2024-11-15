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
    int connfd;
    int n;

    char buffer[1024];
    struct sockaddr_in serv_addr;
    //==========================================================

    // creating tcp socket...
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // specify ipv4, accept any connection, and properly set port # of serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);

    // binding socket to server address...
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // establishing tcp standards hence "connection-oriented"...
    listen(sockfd, 5);

    // while loop to essentially establish the connection... hence reminiscent of being TCP ...
    while (1)
    {
        // test...
        //  cout << "the val of buffer_2 is..." <<buffer_2<<endl;

        // clear the string to allow for successfully multiple different requests from client
        // while server is running
        buffer_2.clear();
        double loan_amount = 0;
        double annual_interest_rate = 0;
        int loan_period = 0;
        monthly_paym = 0;
        total_paym = 0;

        // accept the connection to establish communication...
        connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);

        // while loop interested in communicating with established individuals only .... hence,
        // adding to being TCP reminiscent...
        while (1)
        {
            string info_str;

            n = read(connfd, buffer, sizeof(buffer));
            if (n < 1)
            {
                close(connfd);
                break;
            }

            // append what was read to buffer_2...
            buffer_2.append(buffer, n);

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

            // write using connfd since we read the info over that direct established socket ...
            write(connfd, info_str.c_str(), info_str.length());

            info_str.push_back('\0');
        }
    }

    // close sock..
    close(sockfd);
    return 0;
}
